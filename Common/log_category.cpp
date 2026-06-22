#include "log_category.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <ctime>

static std::shared_ptr<spdlog::async_logger> asynclog;
static std::once_flag gcs_spdlog;

void close_def_logger() {
    spdlog::drop_all();
}

std::shared_ptr<spdlog::async_logger> cur_deft_logger(){
    return asynclog;
}

void init_stdout_file_spdlog(const char *name) {
    std::call_once(gcs_spdlog, [=]() {
        char tempfile[512] = "0";
        time_t timep;
        struct tm p;
        time(&timep); /*获得time_t结构的时间，UTC时间*/
        localtime_s(&p, &timep); /*转换为struct tm结构的当地时间*/
        sprintf_s(tempfile, 512, "logs/%s-%d-%d-%d.log", name, p.tm_year+1900, p.tm_mon, p.tm_yday);
        spdlog::init_thread_pool(256, 1);
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        stdout_sink->set_level(spdlog::level::debug);
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(tempfile, 1048576 * 5, 3);
        rotating_sink->set_level(spdlog::level::info);
        std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
        asynclog = std::make_shared<spdlog::async_logger>("gcslogger", sinks.begin(), sinks.end(),
                                                          spdlog::thread_pool(),
                                                          spdlog::async_overflow_policy::block);
        //设置日志记录级别，您需要用 %^ 和 %$  括上想要彩色的部分
        asynclog->set_level(spdlog::level::info);
        //设置格式
        asynclog->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%5t] %^[%l]%$ [%s:%#] %v");
        //设置当出发 err 或更严重的错误时立刻刷新日志到  disk
        asynclog->flush_on(spdlog::level::err);
        // 3秒刷新一次队列
        spdlog::flush_every(std::chrono::seconds(1));
        //spdlog::set_default_logger(asynclog);
    });
}
