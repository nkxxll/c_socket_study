cmake_minimum_required(VERSION 3.12)
Project(sockets C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(client client.c)
add_executable(server server.c)

# Include the directory containing term_colors.h
include_directories(${CMAKE_SOURCE_DIR})

# Link any additional libraries if needed (none in this example)
# target_link_libraries(MyProject library_name)
