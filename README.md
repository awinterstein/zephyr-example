# Zephyr Example

[![Build](https://github.com/awinterstein/zephyr-example/actions/workflows/build.yml/badge.svg)](https://github.com/awinterstein/zephyr-example/actions/workflows/build.yml)

This repository is mainly for me to try out and learn certain aspects of Zephyr and show their usage as an example.

## Features

### Infrastructure

- Docker container for build and development
  - Built and published via a workflow
  - Used for Development in Visual Studio Code
- Build pipeline (workflow)
  - Building the firmware
  - Check formatting with clang-format
  - Build and execute unit tests and integration tests

### Firmware

- Integration of std::system_error (without dynamic memory) and std::expected
- C++ interface for persistent storage
  - templated access to data
  - templated serialization / deserialization of Protobuf data (optional)
- Zephyr logging enabled including an example of how to use it in header files
