/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#include <accentcolor.h>
#include <constants.h>
#include <gcscolors.h>
#include <gcsrectangle.h>
#include <gcstheme.h>
#include <gcstools.h>
#include <gcstrapezium.h>
#include <grouprectangle.h>
#include <textstyle.h>

void qml_register_types_GcsQuick()
{
    qmlRegisterTypesAndRevisions<AccentColor>("GcsQuick", 1);
    qmlRegisterTypesAndRevisions<GcsColors>("GcsQuick", 1);
    qmlRegisterTypesAndRevisions<GcsRectangle>("GcsQuick", 1);
    qmlRegisterTypesAndRevisions<GcsTheme>("GcsQuick", 1);
    qmlRegisterTypesAndRevisions<GcsTools>("GcsQuick", 1);
    qmlRegisterTypesAndRevisions<GcsTrapezium>("GcsQuick", 1);
    qmlRegisterTypesAndRevisions<GroupRectangle>("GcsQuick", 1);
    qmlRegisterTypesAndRevisions<TextStyle>("GcsQuick", 1);
    qmlRegisterNamespaceAndRevisions(&ThemeType::staticMetaObject, "GcsQuick", 1);
    qmlRegisterNamespaceAndRevisions(&TreeViewType::staticMetaObject, "GcsQuick", 1);
    qmlRegisterModule("GcsQuick", 1, 0);
}

static const QQmlModuleRegistration registration("GcsQuick", 1, qml_register_types_GcsQuick);
