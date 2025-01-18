cmake_minimum_required(VERSION 3.25.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(example)

# Include NanoPB and add the protocol buffers definition.
list(APPEND CMAKE_MODULE_PATH ${ZEPHYR_BASE}/modules/nanopb)
include(nanopb)
zephyr_nanopb_sources(app protobuf/storage.proto)

add_subdirectory(src)
