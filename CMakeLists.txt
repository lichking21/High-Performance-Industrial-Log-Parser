cmake_minimum_required(VERSION 4.0)

project(LogParser C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(MPI REQUIRED)

set(APP_SOURCES
  src/main.c
  src/execute.c
  tools/generator/generator.c
  src/data.c
  src/parser.c
  src/stats.c
  src/reader_stdio.c
  src/reader_mmap.c
)

set(APP_HEADERS
  tools/generator/generator.h
  include/parser.h
  include/stats.h
  include/data.h
  include/reader_stdio.h
  include/reader_mmap.h
)

add_executable(app ${APP_SOURCES} ${APP_HEADERS})

target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/generator
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(app PRIVATE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
  $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
)

target_link_libraries(app PRIVATE MPI::MPI_C)
