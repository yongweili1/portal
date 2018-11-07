CMAKE_MINIMUM_REQUIRED(VERSION  2.8)

PROJECT (McsfLogger)

#step2: include internal header file directory 
INCLUDE_DIRECTORIES (".")
INCLUDE_DIRECTORIES ("include")

INCLUDE_DIRECTORIES ("../include/McsfLogger/")
#INCLUDE_DIRECTORIES ("../McsfLogBase/include")
INCLUDE_DIRECTORIES (../../external/)
link_directories("../../external/lib/")

#step3: define macro(preprocessor) needed 
ADD_DEFINITIONS (-DULIB=MCSF_SQLITE_INTERFACE)

#step4: find all source file 
AUX_SOURCE_DIRECTORY(./source McsfLogger_Src)
AUX_SOURCE_DIRECTORY(. McsfLogger_Src)

MESSAGE (STATUS "Sources: ${McsfLogger_Src}")


#step5: create target(execute or library) 
ADD_LIBRARY(McsfLogger SHARED ${McsfLogger_Src})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/)
#step6: link all required libraries including self developed and 3rd party library 
TARGET_LINK_LIBRARIES(McsfLogger rt pthread boost_date_time boost_system boost_thread boost_regex boost_filesystem tinyxml) 

#step7: set build type into debug or release 
#SET( CMAKE_BUILD_TYPE Debug ) 
#SET( CMAKE_BUILD_TYPE Release ) 



