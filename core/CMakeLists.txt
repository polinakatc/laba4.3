add_library(textcore STATIC
    TTextLink.cpp
    TTextLink.h
    TText.cpp
    TText.h
)

target_include_directories(textcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})