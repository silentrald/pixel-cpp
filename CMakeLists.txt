cmake_minimum_required(VERSION 3.12.1)

project(pixel VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 17)


if (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -Wl,-allow-multiple-definition")
  # For dll files
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif (WIN32)

# Copy files
file(COPY assets DESTINATION .)
file(COPY themes DESTINATION .)
file(COPY locale DESTINATION .)
file(COPY keys.ini DESTINATION .)

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
  src/core/draw/types.cpp
  src/core/draw/anim.cpp
  src/core/draw/disk_backup.cpp
  src/core/draw/image_db.cpp
  src/core/draw/timeline_info.cpp
  src/core/draw/frame.cpp
  src/core/draw/image.cpp
)

# NOTE: Can be changed depending on the gui lib
set(view_srcs src/view/sdl3/manager.cpp
  src/view/sdl3/manager_ctx_menu.cpp
  src/view/sdl3/manager_event.cpp
  src/view/sdl3/manager_modal.cpp
  src/view/sdl3/renderer.cpp
  src/view/sdl3/cached_textures.cpp
  src/view/sdl3/texture.cpp
  
  # Boxes
  src/view/sdl3/box/draw.cpp
  src/view/sdl3/box/menu.cpp
  src/view/sdl3/box/preview.cpp
  src/view/sdl3/box/status.cpp
  src/view/sdl3/box/timeline.cpp
  src/view/sdl3/box/tool.cpp

  # Inputs
  src/view/sdl3/input/button.cpp
  src/view/sdl3/input/textbox.cpp

  # Modal
  src/view/sdl3/modal/file.cpp

  # Widgets
  src/view/sdl3/widget/color_picker.cpp
  src/view/sdl3/widget/text.cpp
  src/view/sdl3/widget/context_menu.cpp
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
  src/core/history/action.cpp
  src/core/history/caretaker.cpp
  src/core/history/arena.cpp
)

set(config_srcs
  src/core/cfg/locale.cpp
  src/core/cfg/shortcut.cpp
  src/core/cfg/theme.cpp
)

set(file_srcs
  src/core/file/pxl.cpp
  src/core/file/png.cpp
)

set(srcs src/entry.cpp
  src/presenter/var.cpp
  src/presenter/anim.cpp
  src/presenter/ctx_menu.cpp
  src/presenter/file.cpp
  src/presenter/tool.cpp
  src/presenter/view.cpp
  src/presenter/presenter.cpp
  src/colors.cpp
  src/math.cpp

  ${logger_srcs}
  ${draw_srcs}
  ${view_srcs}
  ${tool_srcs}
  ${history_srcs}
  ${config_srcs}
  ${file_srcs}
)

include_directories(src)

# SDL3 stuff
set(SDL3_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl3)
set(SDL3_TTF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl3-ttf)
set(SDL3_IMG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl3-img)
set(SDL_TEST OFF)
add_subdirectory(${SDL3_DIR})
add_subdirectory(${SDL3_TTF_DIR})
add_subdirectory(${SDL3_IMG_DIR})

# Png Lib
set(PNG_SHARED ON CACHE BOOL "" FORCE)
set(PNG_STATIC OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)
set(PNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/libpng)
include_directories(${PNG_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/external/libpng)
add_subdirectory(${PNG_DIR})

# Error Handling
include_directories(external/expected/include)

# Unit testing
add_subdirectory(external/catch2)
add_executable(pixel_draw test/draw.cpp
  src/math.cpp
  src/colors.cpp

  ${draw_srcs}
  ${logger_srcs}
)
target_link_libraries(pixel_draw PRIVATE Catch2::Catch2WithMain)

add_executable(pixel_vector test/vector.cpp ${logger_srcs})
target_link_libraries(pixel_vector PRIVATE Catch2::Catch2WithMain)

set(pxl_lib
  SDL3::SDL3
  SDL3_ttf-shared
  SDL3_image-shared
  png_shared
)

add_executable(${PROJECT_NAME} ${srcs})
target_link_libraries(${PROJECT_NAME} PRIVATE ${pxl_lib})

