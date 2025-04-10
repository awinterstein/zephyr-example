cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(integration)


target_sources(app PRIVATE
  # application files
  ../../src/storage/non_volatile_storage.cpp
  ../../src/util/system_error.cpp
  ../../src/util/system_error/error_category.cpp

  # test files
  non_volatile_storage.cpp
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../src/)
