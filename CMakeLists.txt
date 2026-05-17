cmake_minimum_required(VERSION 3.16)
project(TextEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ядро — статическая библиотека
add_subdirectory(core)
add_subdirectory(gtest)

# Консольное приложение
add_executable(text main.cpp)
target_link_libraries(text PRIVATE textcore)

# Тестовое приложение
add_executable(test test.cpp)
# Линкуем тесты с нашим ядром (textcore) и библиотеками GTest
target_link_libraries(test PRIVATE textcore gtest)