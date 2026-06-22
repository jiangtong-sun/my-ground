#pragma once

#include "FLineController.h"
#include "concurrentqueue.h"
#include <QVector>

class MissionUpDownCache {
  public:
    ~MissionUpDownCache();
    MissionUpDownCache() = default;

    ///
    /// \brief downCache
    /// \param ix
    /// \return
    ///
    std::shared_ptr<MissionLineDataProto> downCache(int ix);
    int countDownCache() { return down_cache.size(); }

    ///
    /// \brief clearMissionCache
    ///
    void clearMissionCache();
    ///
    /// \brief uploadData
    /// \return
    ///
    std::shared_ptr<VehicleMissionProto> getUploadData(int);
    ///
    /// \brief nextUploadData
    /// \return
    ///
    std::shared_ptr<VehicleMissionProto> nextUploadData();
    ///
    /// \brief setDownloadData
    /// \param proto
    ///
    void setDownloadData(std::shared_ptr<MissionLineDataProto> proto);
    ///
    /// \brief produceUploadData
    ///     上传多条任务航线
    /// \param ftask
    ///
    void produceUploadData(FLineController *ftask);

    ///
    /// \brief produceUploadData
    /// \param fline
    /// \param type
    ///
    void produceUploadData(StageController *fline, int type);

    ///
    /// \brief produceUploadData
    ///     上传指定航线中某航线段
    /// \param ftask
    /// \param line
    /// \param number
    ///
    void produceUploadData(FLineController *ftask, int line, int number);

  protected:
    moodycamel::ConcurrentQueue<MissionLineDataProto *> lineTrans;
    QVector<std::shared_ptr<MissionLineDataProto>> cache_datas;
    QVector<std::shared_ptr<MissionLineDataProto>> down_cache;
};
