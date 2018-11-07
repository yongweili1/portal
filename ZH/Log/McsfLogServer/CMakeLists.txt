CMAKE_MINIMUM_REQUIRED(VERSION  2.8)

PROJECT (McsfLinuxLogServer)

#step1: include CMakeDefault.txt 
#INCLUDE(../../CMakeDefault.txt)

#step2: include internal header file directory 
INCLUDE_DIRECTORIES ("include")
INCLUDE_DIRECTORIES (".")
INCLUDE_DIRECTORIES ("../include/McsfLogger")
INCLUDE_DIRECTORIES ("../McsfLogBase/include")

#step3: define macro(preprocessor) needed 
#ADD_DEFINITIONS (-D__FOR_LINUX_PLATFORM__)

#step4: find all source file 
AUX_SOURCE_DIRECTORY(. McsfLinuxLogServer_Src)
AUX_SOURCE_DIRECTORY(./source McsfLinuxLogServer_Src)

MESSAGE (STATUS "Sources: ${McsfLinuxLogServer_Src}")


#step5: create target(execute or library) 
ADD_EXECUTABLE(McsfLinuxLoggerServer ${McsfLinuxLogServer_Src})


#step6: link all required libraries including self developed and 3rd party library 
TARGET_LINK_LIBRARIES(McsfLinuxLoggerServer McsfLogBase ${LIBRARY_PATH_BOOSTSYSTEM} ${LIBRARY_PATH_BOOSTTHREAD} ${LIBRARY_PATH_BOOSTFILESYSTEM} ${LIBRARY_PATH_TINYXML})

#step7: set build type into debug or release 
#SET( CMAKE_BUILD_TYPE Debug ) 
SET( CMAKE_BUILD_TYPE Release ) 



