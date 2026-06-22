#ifndef GCSQUICK_PLUGIN_H
#define GCSQUICK_PLUGIN_H

#include <QQmlExtensionPlugin>

class GcsQuickPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

  public:
    void registerTypes(const char *uri) override;
};

#endif // DOWNLOAD_PLUGIN_H
