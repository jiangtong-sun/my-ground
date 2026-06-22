QT  += location-private positioning-private network

INCLUDEPATH += $$QT.location.includes

HEADERS += \
    $$PWD/CachedTileSet.h \
    $$PWD/GCSMapEngine.h \
    $$PWD/GCSServiceProviderPlugin.h \
    $$PWD/GCSTileSet.h \
    $$PWD/GCSTiledManagerEngine.h \
    $$PWD/GeoTileFetcher.h \
    $$PWD/GeoTiledMapReply.h \
    $$PWD/MapEngineData.h \
    $$PWD/MapProvider.h \
    $$PWD/MapTask.h \
    $$PWD/MapUrlEngine.h \
    $$PWD/QmlObjectListModel.h \
    $$PWD/TBMapProvider.h \
    $$PWD/TileCacheWorker.h


SOURCES += \
    $$PWD/CachedTileSet.cpp \
    $$PWD/GCSMapEngine.cpp \
    $$PWD/GCSServiceProviderPlugin.cpp \
    $$PWD/GCSTiledManagerEngine.cpp \
    $$PWD/GeoTileFetcher.cpp \
    $$PWD/GeoTiledMapReply.cpp \
    $$PWD/MapEngineData.cpp \
    $$PWD/MapProvider.cpp \
    $$PWD/MapUrlEngine.cpp \
    $$PWD/QmlObjectListModel.cpp \
    $$PWD/TBMapProvider.cpp \
    $$PWD/TileCacheWorker.cpp


OTHER_FILES += \
    $$PWD/qgc_maps_plugin.json
