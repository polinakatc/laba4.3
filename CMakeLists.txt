cmake_minimum_required(VERSION 3.16)
project(TextEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ядро — статическая библиотека
add_subdirectory(core)

# Консольное приложение
add_executable(text main.cpp)
target_link_libraries(text PRIVATE textcore)

enable_testing()
# Простейший тест: проверяем, что приложение запускается и успешно завершается
add_test(NAME AppLaunchTest COMMAND $<TARGET_FILE:text>)