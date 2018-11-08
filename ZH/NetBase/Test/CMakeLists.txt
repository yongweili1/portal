CMAKE_MINIMUM_REQUIRED(VERSION  2.8)

INCLUDE_DIRECTORIES( ../../external/ )
#step2: include internal header file directory
INCLUDE_DIRECTORIES(./)
link_directories( ${PROJECT_SOURCE_DIR}/../../bin/)
link_directories( ${PROJECT_SOURCE_DIR}/../../external/lib/)
link_directories(/home/lyw/anaconda2/lib/)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/)
#step4: find all source file
SET(SRC_LIST comproxy.cpp )



#step5: create target(execute or library)
add_executable(tllest ${SRC_LIST})

#step6: link all required libraries including self developed and 3rd party library
TARGET_LINK_LIBRARIES (tllest McsfNetBase protobuf python2.7)
