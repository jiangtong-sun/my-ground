#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    // int rtn = QFontDatabase::addApplicationFont((":/iconfont.ttf"));
    // qDebug() << "addApplicationFont" << rtn;
    QVariantMap ttfIcons;

    ttfIcons.insert("add", QStringLiteral("\ue664"));
    ttfIcons.insert("addCircle", QStringLiteral("\ue665"));
    ttfIcons.insert("arrowRightCircle", QStringLiteral("\ue666"));
    ttfIcons.insert("browse", QStringLiteral("\ue667"));
    ttfIcons.insert("arrowDoubleLeft", QStringLiteral("\ue668"));
    ttfIcons.insert("arrowLeftCircle", QStringLiteral("\ue669"));
    ttfIcons.insert("arrowDoubleRight", QStringLiteral("\ue66a"));
    ttfIcons.insert("close", QStringLiteral("\ue66b"));
    ttfIcons.insert("complete", QStringLiteral("\ue66c"));
    ttfIcons.insert("copy", QStringLiteral("\ue66d"));
    ttfIcons.insert("dataView", QStringLiteral("\ue66e"));
    ttfIcons.insert("download", QStringLiteral("\ue66f"));
    ttfIcons.insert("exports", QStringLiteral("\ue670"));
    ttfIcons.insert("edit", QStringLiteral("\ue671"));
    ttfIcons.insert("error", QStringLiteral("\ue672"));
    ttfIcons.insert("favorite", QStringLiteral("\ue673"));
    ttfIcons.insert("fileDelete", QStringLiteral("\ue674"));
    ttfIcons.insert("home", QStringLiteral("\ue675"));
    ttfIcons.insert("history", QStringLiteral("\ue676"));
    ttfIcons.insert("fileOpen", QStringLiteral("\ue677"));
    ttfIcons.insert("screenShrink", QStringLiteral("\ue678"));
    ttfIcons.insert("screenExpand", QStringLiteral("\ue679"));
    ttfIcons.insert("map", QStringLiteral("\ue67a"));
    ttfIcons.insert("modular", QStringLiteral("\ue67b"));
    ttfIcons.insert("notification", QStringLiteral("\ue67c"));
    ttfIcons.insert("play", QStringLiteral("\ue67d"));
    ttfIcons.insert("navigation", QStringLiteral("\ue67e"));
    ttfIcons.insert("move", QStringLiteral("\ue67f"));
    ttfIcons.insert("pin", QStringLiteral("\ue680"));
    ttfIcons.insert("share", QStringLiteral("\ue681"));
    ttfIcons.insert("success", QStringLiteral("\ue682"));
    ttfIcons.insert("setting", QStringLiteral("\ue683"));
    ttfIcons.insert("text", QStringLiteral("\ue684"));
    ttfIcons.insert("upload", QStringLiteral("\ue685"));
    ttfIcons.insert("warning", QStringLiteral("\ue686"));
    ttfIcons.insert("zoomIn", QStringLiteral("\ue687"));
    ttfIcons.insert("zoomOut", QStringLiteral("\ue688"));
    ttfIcons.insert("closeBold", QStringLiteral("\ue689"));
    ttfIcons.insert("selectBold", QStringLiteral("\ue68a"));
    ttfIcons.insert("arrowUpFilling", QStringLiteral("\ue68b"));
    ttfIcons.insert("arrowDownFilling", QStringLiteral("\ue68c"));
    ttfIcons.insert("arrowLeftFilling", QStringLiteral("\ue68d"));
    ttfIcons.insert("arrowRightFilling", QStringLiteral("\ue68e"));
    ttfIcons.insert("square", QStringLiteral("\ue869"));

    engine.rootContext()->setContextProperty("ttfIcons", ttfIcons);
    engine.load("qrc:///Main.qml");
    return app.exec();
}
