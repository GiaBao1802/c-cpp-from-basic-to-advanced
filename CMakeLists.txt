cmake_minimum_required(VERSION 3.16)
project(automotive_c_cpp_learning LANGUAGES C CXX)

option(ADAS_ENABLE_GTEST "Fetch GoogleTest and build ADAS gTest examples" OFF)

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

add_executable(adas_motion_sim final_project/app/motion_sim.cpp)
target_include_directories(adas_motion_sim PRIVATE final_project/include)

add_executable(vehicle_motion_tests final_project/tests/test_vehicle_motion.cpp)
target_include_directories(vehicle_motion_tests PRIVATE final_project/include)
add_test(NAME vehicle_motion_tests COMMAND vehicle_motion_tests)
add_test(NAME adas_motion_scenario COMMAND adas_motion_sim)

add_library(diag_server STATIC diagnostic_stack/src/diag_server.cpp)
target_include_directories(diag_server PUBLIC diagnostic_stack/include)
add_executable(diag_demo diagnostic_stack/app/diag_demo.cpp)
target_link_libraries(diag_demo PRIVATE diag_server)
add_executable(diag_tests diagnostic_stack/tests/test_diag.cpp)
target_link_libraries(diag_tests PRIVATE diag_server)
add_test(NAME diag_tests COMMAND diag_tests)

if(ADAS_ENABLE_GTEST)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  add_executable(vehicle_motion_gtests final_project/tests/vehicle_motion_gtest.cpp)
  target_include_directories(vehicle_motion_gtests PRIVATE final_project/include)
  target_link_libraries(vehicle_motion_gtests PRIVATE GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(vehicle_motion_gtests)
endif()
