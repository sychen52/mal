cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set the project name and version
project(Mal VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(lib SHARED reader.cpp types.cpp env.cpp core.cpp special_form.cpp)
#add_executable(step1_read_print step1_read_print.cpp)
#target_link_libraries(step1_read_print lib)
#set(OUTPUT_EXE step2_eval)
#set(OUTPUT_EXE step3_env)
#set(OUTPUT_EXE step4_if_fn_do)
set(OUTPUT_EXE step6_file)
add_executable(${OUTPUT_EXE} ${OUTPUT_EXE}.cpp)
target_link_libraries(${OUTPUT_EXE} lib)
