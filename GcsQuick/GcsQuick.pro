TEMPLATE = lib
TARGET = GcsQuick

isEmpty(QMAKE_MSC_VER) {
    QMAKE_MSC_VER = 1920
}

message("MSC version: $$QMAKE_MSC_VER")

QT += qml quick
CONFIG += plugin qmltypes c++11
DEFINES += CONTROL_SHAREDLIB
DEFINES += CONTROL_EXPORTS
uri = GcsQuick

CONFIG(Debug, Debug|Release) {
    DESTDIR = $$PWD/../bin/Debug/$$TARGET
    OUT_PWD = $$PWD/../bin/Debug/$$TARGET
} else {
    DESTDIR = $$PWD/../bin/Release/$$TARGET
    OUT_PWD = $$PWD/../bin/Release/$$TARGET
}

QML_IMPORT_NAME = $$TARGET
QML_IMPORT_MAJOR_VERSION = 1
QMLTYPES_FILENAME = $$DESTDIR/plugins.qmltypes

SOURCES += \
    accentcolor.cpp \
    gcscolors.cpp \
    gcsrectangle.cpp \
    gcstools.cpp \
    gcstheme.cpp \
    gcstrapezium.cpp \
    grouprectangle.cpp \
    textstyle.cpp \
    gcsquick_plugin.cpp

HEADERS += \
    accentcolor.h \
    constants.h \
    gcscolors.h \
    gcstheme.h \
    gcsrectangle.h \
    gcsstdafx.h \
    gcstools.h \
    gcstrapezium.h \
    grouprectangle.h \
    textstyle.h \
    gcsquick_plugin.h

DISTFILES = qmldir \
    GcsCheckBox.qml \
    GcsComboBox.qml \
    GcsHorizImage.qml \
    GcsNoticeButton.qml \
    iconfont.ttf \
    GcsRadioButton.qml \
    ControlBackground.qml  \
    FocusRectangle.qml  \
    GcsTableView.qml \
    GcsButton.qml  \
    GcsControl.qml  \
    GcsDivider.qml  \
    GcsFilledButton.qml  \
    GcsGroupBox.qml  \
    GcsHorizText.qml  \
    GcsHorizImage.qml \
    GcsIcon.qml  \
    GcsIconButton.qml  \
    GcsLoader.qml  \
    GcsObject.qml  \
    GcsScrollBar.qml  \
    GcsScrollIndicator.qml  \
    ToastComponent.qml \
    GcsShadow.qml  \
    GcsText.qml  \
    GcsTextBox.qml  \
    GcsTextButton.qml  \
    GcsTextFont.qml  \
    GcsDragElement.qml \
    GcsToggleButton.qml  \
    GcsToggleSwitch.qml  \
    GcsTooltip.qml  \
    GcsSimpleMenu.qml \
    GcsMenuItem.qml  \
    GcsTrapezium.qml  \
    GcsVertiText.qml  \
    VertLineText.qml  \
    GcsVertShow.qml

qmlcopy.files = $$DISTFILES
qmlcopy.path = $$OUT_PWD
COPIES += qmlcopy
