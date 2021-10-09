cmake_minimum_required (VERSION 3.6)
project(mini_asyncio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(./mini_asyncio)

add_executable(async_hello ./async_hello.cpp)
target_link_libraries(async_hello mini_asyncio)

add_executable(generator_test ./generator_test.cpp)
target_link_libraries(generator_test mini_asyncio)
