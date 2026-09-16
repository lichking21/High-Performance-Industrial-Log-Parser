cmake_minimum_required(VERSION 4.0)

project(LogParser C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(MPI REQUIRED)

set(APP_SOURCES
  src/main.c
  tools/generator/generator.c
  src/data.c
  src/parser.c
  src/stats.c
)

set(APP_HEADERS
  tools/generator/generator.h
  include/parser.h
  include/stats.h
  include/data.h
)

add_executable(app ${APP_SOURCES} ${APP_HEADERS})

# Говорим компилятору, где лежат .h файлы для инклудов
target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/generator
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(app PRIVATE MPI::MPI_C)
