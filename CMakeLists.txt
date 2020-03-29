cmake_minimum_required(VERSION 3.14)
project(AsynLog)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 ")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CMAKE_CXX_FLAGS} -DDebug -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CMAKE_CXX_FLAGS} -O3")
include_directories(./include/)
aux_source_directory(./source/ source_list)

#add_executable(AsynLog samples/waiter_thread.cpp ${source_list} source/logFile.cpp include/LogStream.h include/num2String.h source/num2String.cpp samples/num2String_unit_test.cpp)

file(GLOB_RECURSE mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/samples/*.cpp")
foreach(mainfile IN LISTS mains)
#     Get file name without directory
    get_filename_component(mainname ${mainfile} NAME_WE)
    add_executable(${mainname} ${mainfile} ${source_list})
endforeach()
