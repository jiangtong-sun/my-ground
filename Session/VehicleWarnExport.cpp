#include "VehicleWarnExport.h"
#include "log_category.h"
#include <QDateTime>
#include <filesystem>

FileExportCache::FileExportCache() { memset(_cache, 0, sizeof(_cache)); }

void FileExportCache::closeFile() {
    if (ofile.is_open()) {
        ofile.close();
    }
}

bool FileExportCache::createFile(const QString &filename) {
    QByteArray groupArr = filename.toLocal8Bit();
    std::wstring datafile = filename.toStdWString();

    ofile.open(datafile.c_str());
    if (!ofile.is_open()) {
        COMMON_WARN("Could not to create file {}", groupArr.toStdString());
        return false;
    }

    ofile.rdbuf()->pubsetbuf(_cache, sizeof(_cache));
    COMMON_INFO("create data file {}", groupArr.toStdString());
    return true;
}

void VehilceWarnExport::add(PWarnElemNode elem) {
    if (!exit_flag) {
        warn_node_queue.enqueue(elem);
    }
}

int VehilceWarnExport::closeExport() {
    if (exit_flag) {
        return 0;
    }

    exit_flag = true;
    int rtn = future_result.get();
    //if (future_result.joinable()) {
    //    future_result.join();
    //}
    file_export_cache->closeFile();
    file_export_cache = nullptr;
    COMMON_INFO("warn export {}", rtn);
    return rtn;
}

int VehilceWarnExport::exportDataToFile() {

    while (!exit_flag) {
        std::shared_ptr<WarnElemNode> newElem = nullptr;
        if ((warn_node_queue.size_approx() > 0) &&
            warn_node_queue.try_dequeue(newElem) &&
            (newElem != nullptr)) {
            file_export_cache->ofile << newElem->content();
            //newElem->content(file_export_cache->ofile);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}

void VehilceWarnExport::startExport() {
    if (file_export_cache != nullptr) {
        return;
    }

    QDateTime filetime = QDateTime::currentDateTime();
    QDate cur = filetime.date();
    QTime ctm = filetime.time();
    QString datetm = QString("Datas/%1-%2-%3").arg(cur.year()).arg(cur.month()).arg(cur.day());
    QString timestr = QString("%1-%2-%3").arg(ctm.hour()).arg(ctm.minute()).arg(ctm.second());

    std::filesystem::path path = datetm.toStdString();
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
        COMMON_WARN("create folder {}", datetm.toStdString());
    }

    QString subfilder = datetm + "/" + timestr;
    path = subfilder.toStdString();
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
        COMMON_WARN("create folder {}", subfilder.toStdString());
    }

    exit_flag = false;
    file_export_cache = std::shared_ptr<FileExportCache>(new FileExportCache());
    file_export_cache->createFile(subfilder + "/" + QString("warninfo.txt"));
    node_header.header(file_export_cache->ofile);

    auto exportDataFunc = std::bind(&VehilceWarnExport::exportDataToFile, this);
    future_result = std::async(std::launch::async, exportDataFunc);
    //std::thread([=](){ exportDataToFile();}); //s
}
