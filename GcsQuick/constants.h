#pragma once

#include <QtQml/qqml.h>

#include <QObject>

namespace ThemeType {
Q_NAMESPACE
enum DarkMode {
    Light = 0x0001, // light mode
    Dark = 0x0002,  // dark mode
};
Q_ENUM_NS(DarkMode)
QML_NAMED_ELEMENT(ThemeType)

} // namespace ThemeType

namespace TreeViewType {
Q_NAMESPACE
enum SelectionMode {
    None = 0x0000,    // unsupport selection
    Single = 0x0001,  // single selection
    Multiple = 0x0002 // multiple selection
};
Q_ENUM_NS(SelectionMode)
QML_NAMED_ELEMENT(TreeViewType)

} // namespace TreeViewType
