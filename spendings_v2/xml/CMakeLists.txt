cmake_minimum_required(VERSION 3.8) # Проверка версии CMake.
                                    # Если версия установленой программы
                                    # старее указаной, произойдёт аварийный выход.
set(PROJECT spendings_xml)            # Переменная и названием проекта
project(${PROJECT})                 # Название проекта
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall") # Флаги компилятора
set(CMAKE_BUILD_TYPE Debug)


include_directories(/home/dmitryd/coursera/brown_belt/include ./include)    # Папка с хэдерами
set (SOURCES ./src/main.cpp ./src/xml.cpp) # Перечень всех сорцов

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT} ${SOURCES})
