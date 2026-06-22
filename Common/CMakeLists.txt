cmake_minimum_required(VERSION 3.16)

project(Common VERSION 1.01 LANGUAGES CXX)

add_definitions(-DCOMMON_SHAREDLIB)
add_definitions(-DCOMMON_EXPORTS)

find_package(Qt5 REQUIRED COMPONENTS Core Network)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/include)
link_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/lib)

file(GLOB SOURCE_FILES *.cpp *.cxx)

foreach (filepath ${SOURCE_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sourcefiles ${filename})
endforeach (filepath)

file(GLOB HEADER_FILES *.h)

foreach (filepath ${HEADER_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND headerfiles ${filename})
endforeach (filepath)

add_library(${PROJECT_NAME} SHARED ${sourcefiles} ${headerfiles})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} "uvd.lib" "Ws2_32.lib")
else ()
    target_link_libraries(${PROJECT_NAME} "uv.lib" "Ws2_32.lib")
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION 0.${PROJECT_VERSION}
)

