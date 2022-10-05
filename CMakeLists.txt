cmake_minimum_required (VERSION 3.1)

project(mcpelauncher-extract LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  include(GNUInstallDirs)
  mark_as_advanced(CLEAR
    CMAKE_INSTALL_BINDIR
    CMAKE_INSTALL_DATADIR
  )
else()
  set(CMAKE_INSTALL_BINDIR bin)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
find_package(Zip REQUIRED)

add_library(mcpelauncher-extract-lib include/mcpelauncher/zip_extractor.h lib/zip_extractor.cpp)
target_include_directories(mcpelauncher-extract-lib PUBLIC include/)
target_include_directories(mcpelauncher-extract-lib PUBLIC ${LIBZIP_INCLUDE_DIRS})
target_link_libraries(mcpelauncher-extract-lib ${LIBZIP_LIBRARIES})

add_executable(mcpelauncher-extract src/main.cpp include/mcpelauncher/minecraft_extract_utils.h)
target_link_libraries(mcpelauncher-extract mcpelauncher-extract-lib)

install(TARGETS mcpelauncher-extract DESTINATION ${CMAKE_INSTALL_BINDIR})
