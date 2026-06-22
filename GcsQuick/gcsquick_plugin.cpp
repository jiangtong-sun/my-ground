#include "gcsquick_plugin.h"

#include "accentcolor.h"
#include "gcscolors.h"
#include "gcsrectangle.h"
#include "gcstheme.h"
#include "gcstools.h"
#include "gcstrapezium.h"
#include "grouprectangle.h"
#include "textstyle.h"

#include <qqml.h>

void GcsQuickPlugin::registerTypes(const char *uri) {
    // @uri GcsQuick
    qmlRegisterType<AccentColor>(uri, 1, 0, "AccentColor");
    qmlRegisterType<GcsColors>(uri, 1, 0, "GcsColors");
    qmlRegisterType<GcsRectangle>(uri, 1, 0, "GcsRectangle");
    qmlRegisterType<GcsTrapezium>(uri, 1, 0, "GcsTrapezium");
    qmlRegisterType<GroupRectangle>(uri, 1, 0, "GroupRectangle");
    qmlRegisterType<GcsTheme>(uri, 1, 0, "GcsTheme");
    qmlRegisterType<GcsTools>(uri, 1, 0, "GcsTools");
    qmlRegisterType<TextStyle>(uri, 1, 0, "TextStyle");
}
