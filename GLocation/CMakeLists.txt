set(CMAKE_AUTOMOC ON)

add_definitions(-DQT_STATICPLUGIN)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui Core Location Network Positioning Qml Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Core Location Network Positioning Qml Sql)

include_directories(${CMAKE_SOURCE_DIR}/Common)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_library(GLocation STATIC
    #CLASS_NAME GCSServiceProviderPlugin
    #OUTPUT_TARGETS GCSServiceProviderPlugin
        CachedTileSet.h
        CachedTileSet.cpp
        GCSMapEngine.cpp
        GCSMapEngine.h
        GCSServiceProviderPlugin.cpp
        GCSServiceProviderPlugin.h
        GCSTileSet.h
        GCSTiledManagerEngine.h
        GCSTiledManagerEngine.cpp
        GeoTileFetcher.cpp
        GeoTileFetcher.h
        GeoTiledMapReply.cpp
        GeoTiledMapReply.h
        MapEngineData.cpp
        MapEngineData.h
        MapProvider.h
        MapProvider.cpp
        MapTask.h
        MapUrlEngine.h
        MapUrlEngine.cpp
        QmlObjectListModel.cpp
        QmlObjectListModel.h
        TBMapProvider.cpp
        TBMapProvider.h
        TileCacheWorker.cpp
        TileCacheWorker.h
)

target_link_libraries(GLocation
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Positioning
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Location
        Qt${QT_VERSION_MAJOR}::LocationPrivate
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Sql
)

target_include_directories(GLocation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
