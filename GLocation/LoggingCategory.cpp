#include "LoggingCategory.h"

static std::once_flag gcs_spdlog;

void location_stdout_file_spdlog(const char * name)
{
    std::call_once(gcs_spdlog, [=](){
        char tempfile[512] = "0";
        if (name == nullptr) {
            sprintf_s(tempfile, 512, "logs/location.log");
        } else {
            sprintf_s(tempfile, 512, "logs/%s", name);
        }

        //异步日志，具有8k个项目和1个后台线程的队列
        spdlog::init_thread_pool(8192, 1);

        //标准控制台输出
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        stdout_sink->set_level(spdlog::level::debug);

        //日志文件输出，0点0分创建新日志
        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(tempfile, 0, 0);
        file_sink->set_level(spdlog::level::info);

        std::vector<spdlog::sink_ptr> sinks{stdout_sink, file_sink};
        auto log = std::make_shared<spdlog::async_logger>(
            "logger", sinks.begin(), sinks.end(), spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);

        //设置日志记录级别，您需要用 %^ 和 %$  括上想要彩色的部分
        log->set_level(spdlog::level::trace);

        //设置格式
        log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%5t] %^[%l]%$ [%s:%#] %v");

        //设置当出发 err 或更严重的错误时立刻刷新日志到  disk
        log->flush_on(spdlog::level::err);

        // 3秒刷新一次队列
        spdlog::flush_every(std::chrono::seconds(1));
        spdlog::set_default_logger(log);
    });
}
