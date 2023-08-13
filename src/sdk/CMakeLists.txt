cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

include_directories(${FFMPEG_INCLUDE_DIRS})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/deps/sailfish_protos/messages.proto)
message("${PROTO_SRCS} ")
message("${PROTO_HDRS} ")

aux_source_directory(. SDK_SRCS)

add_library(sdk ${SDK_SRCS} ${PROTO_SRCS})

target_link_libraries(sdk avutil avcodec yuv)
target_link_libraries(sdk protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)