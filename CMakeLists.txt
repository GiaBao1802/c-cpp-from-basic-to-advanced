cmake_minimum_required(VERSION 3.16)
project(automotive_c_cpp_learning LANGUAGES C CXX)

find_package(Threads REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Werror)
endif()

enable_testing()
add_executable(bitfield_lab labs/c/bitfield.c)
add_executable(fixed_buffer_lab labs/cpp/fixed_buffer.cpp)
add_executable(callback_demo examples/callback_dispatch.c)
add_test(NAME bitfield_lab COMMAND bitfield_lab)
add_test(NAME fixed_buffer_lab COMMAND fixed_buffer_lab)
add_test(NAME callback_demo COMMAND callback_demo)

add_library(adas_platform STATIC final_project/src_c/platform_c.c)
target_include_directories(adas_platform PUBLIC final_project/include)

add_library(adas_domain STATIC final_project/src_cpp/adas_controller.cpp
                               final_project/src_cpp/ipc_publisher.cpp)
target_include_directories(adas_domain PUBLIC final_project/include)
target_link_libraries(adas_domain PUBLIC adas_platform Threads::Threads)

add_executable(adas_ecu_demo final_project/app/main.cpp)
target_link_libraries(adas_ecu_demo PRIVATE adas_domain)

add_executable(adas_project_tests final_project/tests/test_project.cpp)
target_link_libraries(adas_project_tests PRIVATE adas_domain)
add_test(NAME adas_project_tests COMMAND adas_project_tests)
