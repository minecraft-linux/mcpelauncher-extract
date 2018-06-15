cmake_minimum_required (VERSION 2.6)

project(mcpelauncher-extract LANGUAGES CXX)

set(LIBZIP_INCLUDE_DIRS )
set(LIBZIP_LIBRARIES zip)

add_library(mcpelauncher-extract-lib include/mcpelauncher/zip_extractor.h lib/zip_extractor.cpp)
target_include_directories(mcpelauncher-extract-lib PUBLIC include/)
target_include_directories(mcpelauncher-extract-lib PRIVATE ${LIBZIP_INCLUDE_DIRS})
target_link_libraries(mcpelauncher-extract-lib ${LIBZIP_LIBRARIES})

add_executable(mcpelauncher-extract src/main.cpp)
target_link_libraries(mcpelauncher-extract mcpelauncher-extract-lib)

