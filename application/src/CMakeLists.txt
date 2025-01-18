target_sources(
  app
  PRIVATE main.cpp storage/non_volatile_storage.cpp protobuf/protobuf_error.cpp
          protobuf/protobuf_message.cpp util/system_error.cpp)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_LIST_DIR})
