file(GLOB hdrs "*.h*")
file(GLOB srcs "*.cpp")

add_library(textcore ${srcs} ${hdrs})

target_include_directories(textcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})