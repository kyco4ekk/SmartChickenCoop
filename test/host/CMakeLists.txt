
cmake_minimum_required(VERSION 3.16)

project(BirdCounterTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(bird_counter_tests
    bird_counter_tests.cpp
    direction_detector_tests.cpp
    bird_tracker_tests.cpp
    bird_movement_processor_tests.cpp
    coop_config_tests.cpp
    coop_application_tests.cpp

    ../../components/bird_counter/bird_counter.cpp
    ../../components/direction_detector/direction_detector.cpp
    ../../components/bird_tracker/bird_tracker.cpp
    ../../components/bird_movement_processor/bird_movement_processor.cpp
    ../../components/coop_config/coop_config.cpp
    ../../components/coop_application/coop_application.cpp
)

target_include_directories(bird_counter_tests PRIVATE
    ../../components/bird_counter/include
    ../../components/direction_detector/include
    ../../components/bird_tracker/include
    ../../components/bird_movement_processor/include
    ../../components/coop_config/include
    ../../components/coop_application/include
)

target_link_libraries(bird_counter_tests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(bird_counter_tests)
