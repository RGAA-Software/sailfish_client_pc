cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(sdk)
add_subdirectory(ui)
add_subdirectory(test)

aux_source_directory(. SRCS)
add_library(main ${SRCS})
target_link_libraries(main Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Multimedia)