cmake_minimum_required (VERSION 3.6)
project(mini_asyncio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/event_loop.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/future.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/task.cpp
)

add_library(mini_asyncio STATIC ${SOURCES})
target_include_directories(mini_asyncio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
