cmake_minimum_required(VERSION 3.22.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(example)

# Verify that this is built through sysbuild to ensure MCUboot is automatically
# included and that sample specific MCUboot configurations are used when
# building MCUboot.
test_sysbuild()

target_sources(app PRIVATE src/main.cpp)
