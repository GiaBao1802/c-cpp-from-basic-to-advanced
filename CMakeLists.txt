cmake_minimum_required(VERSION 3.16)
project(automotive_c_cpp_learning LANGUAGES C CXX)

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
add_test(NAME bitfield_lab COMMAND bitfield_lab)
add_test(NAME fixed_buffer_lab COMMAND fixed_buffer_lab)
