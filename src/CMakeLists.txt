cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(sdk)
add_subdirectory(ui)
add_subdirectory(settings)

aux_source_directory(. SRCS)
add_library(main ${SRCS})