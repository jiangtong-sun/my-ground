cmake_minimum_required(VERSION 3.16)

project(BaseProto VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

add_definitions(-DQXlsx_SHAREDLIB)
add_definitions(-DCOMMON_SHAREDLIB)
add_definitions(-DBASEPROTO_SHAREDLIB)
add_definitions(-DBASEPROTO_EXPORTS)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Core REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/include)
include_directories(${CMAKE_SOURCE_DIR}/QXlsx/header)
include_directories(${CMAKE_SOURCE_DIR}/Common)

link_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/lib)
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB CPP_FILES *.cpp *.cxx *.h)

foreach (filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND src_files ${filename})
endforeach (filepath)

add_library(${PROJECT_NAME} SHARED ${src_files})

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    "QXlsx.lib"
    "Common.lib"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION 0.${PROJECT_VERSION}
)
