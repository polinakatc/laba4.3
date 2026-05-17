file(GLOB hdrs "*.h*") #Добавляем в переменную hdrs все файлы с расширением .h
file(GLOB srcs "*.cpp")#Добавляем в переменную srcs все файлы с расширением .cpp


add_library(textcore ${srcs} ${hdrs})

target_include_directories(textcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})