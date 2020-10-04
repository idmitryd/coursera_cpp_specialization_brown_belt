cmake_minimum_required(VERSION 3.8) # Проверка версии CMake.
                                    # Если версия установленой программы
                                    # старее указаной, произойдёт аварийный выход.
set(PROJECT priority_collection)            # Переменная и названием проекта
project(${PROJECT})                 # Название проекта
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall") # Флаги компилятора


include_directories(./include)      # Папка с хэдерами
set (SOURCES ./src/main.cpp) # Перечень всех сорцов


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT} ${SOURCES})
