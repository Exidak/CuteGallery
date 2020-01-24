cmake_minimum_required(VERSION 3.13)

project(QtGallery)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH /home/anton/Qt/5.14.0/gcc_64/)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

file(GLOB QTGALLERY_SOURCES
        src/*.h
        src/*.cpp) 

add_executable(${PROJECT_NAME} ${QTGALLERY_SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core)
