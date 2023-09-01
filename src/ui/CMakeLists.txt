cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

aux_source_directory(. UI_SRCS)
aux_source_directory(./ext EXT_SRCS)
aux_source_directory(./debug DEBUG_SRCS)
aux_source_directory(./widgets WIDGET_SRCS)

add_library(ui ${UI_SRCS} ${EXT_SRCS} ${FORM_UI_SRCS} ${DEBUG_SRCS} ${WIDGET_SRCS})
target_link_libraries(ui sdk)
target_link_libraries(ui Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Multimedia)
target_link_libraries(ui
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
