cmake_minimum_required(VERSION 3.8) # Проверка версии CMake.
                                    # Если версия установленой программы
                                    # старее указаной, произойдёт аварийный выход.
set(PROJECT synchronized_2_v2)            # Переменная и названием проекта
project(${PROJECT})                 # Название проекта
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall") # Флаги компилятора
set(CMAKE_BUILD_TYPE Debug)


include_directories(/home/dmitryd/coursera/brown_belt/include ./include)    # Папка с хэдерами
set (SOURCES
	./src/${PROJECT}.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads)
add_executable(${PROJECT} ${SOURCES})
target_link_libraries (${PROJECT} ${CMAKE_THREAD_LIBS_INIT})
