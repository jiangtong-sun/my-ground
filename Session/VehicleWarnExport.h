#pragma once

#include "FlightWarning.h"
#include "concurrentqueue.h"
#include <fstream>
#include <future>

#define EXPORT_DATA_CACHE 1024 * 32

struct FileExportCache {
    std::ofstream ofile;
    uint32_t _counts = 0;
    QString curTimestamp = "";
    char _cache[EXPORT_DATA_CACHE];

    FileExportCache();

    uint32_t counts() { return ++_counts; }
    bool createFile(const QString &file);
    void closeFile();
};

class VehilceWarnExport : public QObject {
    Q_OBJECT
  public:
    VehilceWarnExport() = default;
    ~VehilceWarnExport() = default;

    void add(PWarnElemNode);
    int exportDataToFile();

    int closeExport();
    void startExport();

  private:
    WarnElemNode node_header;
    //std::thread future_result;
    std::atomic_bool exit_flag = true;
    std::future<int> future_result;
    std::shared_ptr<FileExportCache> file_export_cache = nullptr;
    moodycamel::ConcurrentQueue<PWarnElemNode> warn_node_queue;
};
