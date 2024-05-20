SRC_FILES = shbk2.pb.cpp client_simulator.cpp
TARGET = simulator
LINK_LIBS = -levent -lprotobuf -labsl_log_internal_message -labsl_log_internal_check_op
LIBRARY_DIR = /opt/homebrew/opt

$(TARGET) : shbk2.pb.h $(SRC_FILES)
	g++ -Wno-deprecated -std=c++17 -I $(LIBRARY_DIR)/libevent/include -I $(LIBRARY_DIR)/abseil/include -I $(LIBRARY_DIR)/protobuf/include -L $(LIBRARY_DIR)/libevent/lib -L $(LIBRARY_DIR)/abseil/lib -L $(LIBRARY_DIR)/protobuf/lib $(SRC_FILES) -o $(TARGET) $(LINK_LIBS)
