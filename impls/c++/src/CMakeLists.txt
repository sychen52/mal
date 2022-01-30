cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set the project name and version
project(Mal VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(lib SHARED reader.cpp printer.cpp types.cpp env.cpp core.cpp special_form.cpp procedure.cpp)
set(OUTPUT_EXE0 step0_repl)
set(OUTPUT_EXE1 step1_read_print)
set(OUTPUT_EXE2 step2_eval)
set(OUTPUT_EXE3 step3_env)
set(OUTPUT_EXE4 step4_if_fn_do)
set(OUTPUT_EXE5 step5_tco)
set(OUTPUT_EXE6 step6_file)
add_executable(${OUTPUT_EXE6} ${OUTPUT_EXE6}.cpp)
target_link_libraries(${OUTPUT_EXE6} lib)

add_executable(${OUTPUT_EXE5} ${OUTPUT_EXE6}.cpp)
target_link_libraries(${OUTPUT_EXE5} lib)
add_executable(${OUTPUT_EXE4} ${OUTPUT_EXE6}.cpp)
target_link_libraries(${OUTPUT_EXE4} lib)
add_executable(${OUTPUT_EXE3} ${OUTPUT_EXE6}.cpp)
target_link_libraries(${OUTPUT_EXE3} lib)
add_executable(${OUTPUT_EXE2} ${OUTPUT_EXE6}.cpp)
target_link_libraries(${OUTPUT_EXE2} lib)
add_executable(${OUTPUT_EXE1} ${OUTPUT_EXE6}.cpp)
target_link_libraries(${OUTPUT_EXE1} lib)
add_executable(${OUTPUT_EXE0} ${OUTPUT_EXE6}.cpp)
target_link_libraries(${OUTPUT_EXE0} lib)
