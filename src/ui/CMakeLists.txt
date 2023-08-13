cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. UI_SRCS)

add_library(ui ${UI_SRCS})
target_link_libraries(ui Qt6::Widgets Qt6::Core Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Multimedia)
target_link_libraries(ui
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
