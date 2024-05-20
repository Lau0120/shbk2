#include "session.h"

namespace shbk2 {

bool Buffer::Alloc(std::uint32_t size) {
  if (buf_ != nullptr)
    return false;
  total_ = size;
  last_ = 0;
  buf_ = std::shared_ptr<char>(new char[total_]);
  return true;
}

void Buffer::Reset() {
  if (buf_ == nullptr)
    return;
  total_ = last_ = 0;
  buf_.reset();
}

void Buffer::Realloc(std::uint32_t size) {
  if (buf_ == nullptr) {
    Alloc(size);
    return;
  }
  total_ = size;
  last_ = 0;
  buf_.reset(new char[total_]);
}

void Session::set_bufev(bufferevent* bufev) {
  bufev_ = std::shared_ptr<bufferevent>(bufev, Session::free_bufferevent);
}

void Session::set_phone_number(const std::string& phone_number) {
  phone_number_ = phone_number;
}

const std::string& Session::phone_number() const {
  return phone_number_;
}

}