
CMAKE_MINIMUM_REQUIRED(VERSION  2.8)

INCLUDE_DIRECTORIES( ../../external/ )
INCLUDE_DIRECTORIES(../include/McsfNetBase/) 
INCLUDE_DIRECTORIES( ./)
INCLUDE_DIRECTORIES(../../Log/include/)
INCLUDE_DIRECTORIES(/home/lyw/anaconda2/include/python2.7/)
#step2: include internal header file directory
SET(CMAKE_CXX_FLAGS "-std=gnu++0x")
INCLUDE_DIRECTORIES(./)
link_directories( ${PROJECT_SOURCE_DIR}/../../bin/)
link_directories( ${PROJECT_SOURCE_DIR}/../../external/lib/)
link_directories(/home/lyw/anaconda2/lib/)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/)
#step4: find all source file
SET(SRC_LIST 
connection_i.cpp
connection_manager.cpp
io_service_pool.cpp
mcsf_communication_proxy.cpp
mcsf_error_code.cpp
mcsf_netbase_iclr_cmd_handler.cpp
mcsf_share_memory_receiver.cpp
mcsf_share_memory_sender.cpp
mcsf_socket_receiver.cpp
mcsf_socket_sender.cpp
pending_call.cpp
server.cpp
SystemCmdMessage.pb.cc
mcsf_netbase_wrap.cxx )



#step5: create target(execute or library)
add_library(McsfNetBase SHARED ${SRC_LIST})

#step6: link all required libraries including self developed and 3rd party library
TARGET_LINK_LIBRARIES (McsfNetBase protobuf python2.7 McsfLogger)

