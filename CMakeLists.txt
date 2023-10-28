cmake_minimum_required(VERSION 3.12.1)

project(pixel VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 17)


if (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif (WIN32)

# Disable assert in release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("Being released")
  add_compile_definitions(NDEBUG)
  set(logger_srcs "")
else()
  set(logger_srcs src/core/logger/dev.cpp)
endif()

# Set the srcs
set(draw_srcs
  src/core/draw/anim.cpp
  src/core/draw/frame.cpp
  src/core/draw/layer.cpp
)

# NOTE: Can be changed depending on the gui lib
set(view_srcs src/view/sdl3/manager.cpp
  src/view/sdl3/manager_event.cpp
  src/view/sdl3/manager_modal.cpp
  src/view/sdl3/renderer.cpp
  src/view/sdl3/cached_textures.cpp
  src/view/sdl3/texture.cpp
  
  # Boxes
  src/view/sdl3/box/draw.cpp
  src/view/sdl3/box/menu.cpp
  src/view/sdl3/box/status.cpp
  src/view/sdl3/box/tool.cpp

  # Modal
  src/view/sdl3/modal/file.cpp

  # Widgets
  src/view/sdl3/widget/button.cpp
  src/view/sdl3/widget/menu_btn.cpp
  src/view/sdl3/widget/textbox.cpp
)

set(tool_srcs
  src/core/tool/fill.cpp
  src/core/tool/eraser.cpp
  src/core/tool/line.cpp
  src/core/tool/pan.cpp
  src/core/tool/pencil.cpp
  src/core/tool/select.cpp
  src/core/tool/utils.cpp
  src/core/tool/zoom.cpp
)

set(history_srcs
  src/core/history/caretaker.cpp
  src/core/history/snapshot.cpp
)

set(config_srcs
  src/core/cfg/shortcut.cpp
  src/core/cfg/theme.cpp)

set(srcs src/entry.cpp
  src/presenter/presenter.cpp
  src/colors.cpp
  src/math.cpp

  ${logger_srcs}
  ${draw_srcs}
  ${view_srcs}
  ${tool_srcs}
  ${history_srcs}
  ${config_srcs}
)

add_executable(${PROJECT_NAME} ${srcs})
include_directories(src)

# SDL3 stuff
set(SDL3_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl3)
set(SDL3_TTF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl3-ttf)
set(SDL3_IMG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl3-img)
set(SDL_TEST OFF)
add_subdirectory(${SDL3_DIR})
add_subdirectory(${SDL3_TTF_DIR})
add_subdirectory(${SDL3_IMG_DIR})

# Unit testing
add_subdirectory(external/catch2)
add_executable(pixel_draw test/draw.cpp src/math.cpp ${draw_srcs})
target_link_libraries(pixel_draw PRIVATE Catch2::Catch2WithMain)

add_executable(pixel_vector test/vector.cpp)
target_link_libraries(pixel_vector PRIVATE Catch2::Catch2WithMain)

if (UNIX)
  set(pxl_lib
    SDL3::SDL3
    SDL3_ttf-shared
    SDL3_image-shared
    # png
  )
elseif (WIN32)
  set(pxl_lib
    SDL3::SDL3
    SDL3_ttf-shared
    SDL3_image-shared
    # png z
  )
endif (UNIX)

target_link_libraries(${PROJECT_NAME} PRIVATE ${pxl_lib})

