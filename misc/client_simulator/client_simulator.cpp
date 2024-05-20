#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string>
#include <sstream>
#include <cstdio>

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/util.h>

#include "shbk2.pb.h"


using u16 = unsigned short;

constexpr const char* g_ServerIP{ "127.0.0.1" };
constexpr u16 g_ServerPort{ 9000 };

struct Header {

public:
  Header(const std::string& id, u16 type, int msgLen) :
      m_Id{ id }, m_Type{ type }, m_MessageLen{ msgLen } {}
  std::string ToString() const {
    std::ostringstream oss;
    oss << m_Id << m_Type << m_MessageLen;
    return oss.str();
  }
  void write_evbuffer( struct evbuffer* evbuf ) const {
    evbuffer_add( evbuf, ( void* )m_Id.c_str(), m_Id.size() );
    evbuffer_add( evbuf, ( void* )&m_Type, sizeof( m_Type ) );
    evbuffer_add( evbuf, ( void* )&m_MessageLen, sizeof( m_MessageLen ) );
  }

public:
  std::string m_Id;
  u16 m_Type;
  int m_MessageLen;
};

void ClientEvCb( struct bufferevent* bufev, short events, void* ctx );
void ClientRdCb( struct bufferevent* bufev, void* ctx );
void ClientWrCb( struct bufferevent* bufev, void* ctx );


int main( void ) {
  /* create & write buffer */
  struct event_base* evBase = event_base_new();
  struct bufferevent* connBufev = bufferevent_socket_new( 
    evBase, -1, BEV_OPT_CLOSE_ON_FREE );
  bufferevent_setcb( connBufev, ClientRdCb, ClientWrCb, ClientEvCb, NULL );
  bufferevent_enable( connBufev, EV_READ|EV_WRITE|EV_PERSIST );

  /* create a connection */
  struct sockaddr_in serverAddr;
  bzero( &serverAddr, sizeof( serverAddr ) );
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr( g_ServerIP );
  serverAddr.sin_port = htons( g_ServerPort );
  if ( bufferevent_socket_connect( connBufev, ( struct sockaddr* )&serverAddr,
    sizeof( serverAddr ) ) < 0 ) {
    printf( "connect to server %s error.\n", g_ServerIP );
    bufferevent_free( connBufev );
  }
  event_base_dispatch( evBase );
}

void ClientEvCb( struct bufferevent* bufev, short events, void* ctx ) {
  if ( events & BEV_EVENT_CONNECTED ) {
    printf( "connect to server %s\n", g_ServerIP );
    /* generate message */
    shbk2::PhoneReq phoneReq;
    phoneReq.set_phone_number( "13835479597" );
    std::string message = phoneReq.SerializeAsString();

    /* generate header */
    Header headerReq( "SHBK", 0x01, phoneReq.ByteSize() );
    std::string header = headerReq.ToString();
    std::printf( "Send Message:\n%s(%ld)%s(%d)\n",
      header.c_str(), header.size(), message.c_str(), phoneReq.ByteSize() );

    /* fill event buffer */
    struct evbuffer* bufReq = evbuffer_new();
    headerReq.write_evbuffer(bufReq);  // NOTE: call header method first
    evbuffer_add( bufReq, message.c_str(), phoneReq.ByteSize() );

    /* write message */
    bufferevent_write_buffer( bufev, bufReq );
  } else if ( events & BEV_EVENT_ERROR ) {
    printf( "connect to server %s error.\n", g_ServerIP );
    bufferevent_free( bufev );
  }
}

void ClientRdCb( struct bufferevent* bufev, void* ctx ) {
  // Parse header
  char header[ 10 ]{ '\0' };
  bufferevent_read(bufev, header, sizeof(header));
  char* p = header;
  p += 4;
  unsigned short messageType = *( unsigned short* )p;
  p += 2;
  unsigned int messageSize = *( unsigned int* )p;

  // Read message
  void* message = new char[ messageSize ]{ 0 };
  bufferevent_read( bufev, message, messageSize );

  // Parse message
  if ( messageType & 0x02 ) {
    shbk2::PhoneRes phoneRes;
    phoneRes.ParseFromArray( message, messageSize );
    std::printf( "dynamic code is %d\n",
      phoneRes.identifier() );
    
    // prepare data
    shbk2::LoginReq loginReq;
    loginReq.set_phone_number( "13835479597" );
    loginReq.set_identifier( phoneRes.identifier() );
    std::string loginReqMessage = loginReq.SerializeAsString();
    
    /* generate header */
    Header loginReqHeader( "SHBK", 0x03, loginReq.ByteSize() );
    std::string loginReqHeaderStr = loginReqHeader.ToString();
    std::printf( "Send Message:\n%s(%ld)%s(%d)\n",
      loginReqHeaderStr.c_str(), loginReqHeaderStr.size(),
      loginReqMessage.c_str(), loginReq.ByteSize() );

    /* fill event buffer */
    struct evbuffer* bufReq = evbuffer_new();
    loginReqHeader.write_evbuffer(bufReq);  // NOTE: call header method first
    evbuffer_add( bufReq, loginReqMessage.c_str(), loginReq.ByteSize() );

    /* write message */
    bufferevent_write_buffer( bufev, bufReq );
  } else if ( messageType & 0x04 ) {
    shbk2::LoginRes loginRes;
    loginRes.ParseFromArray( message, messageSize );
    std::printf( "code: %d, desc: %s\n",
      loginRes.code(), loginRes.desc().c_str() );
  }
}

void ClientWrCb( struct bufferevent* bufev, void* ctx ) {
  printf("ClientWrCb() was called\n");
}
