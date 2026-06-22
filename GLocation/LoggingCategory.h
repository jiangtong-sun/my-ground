#pragma once

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <spdlog/spdlog.h>

void location_stdout_file_spdlog(const char * name);

#define GCS_TRACE(...)     SPDLOG_TRACE(__VA_ARGS__)
#define GCS_DEBUG(...)     SPDLOG_DEBUG(__VA_ARGS__)
#define GCS_INFO(...)      SPDLOG_INFO(__VA_ARGS__)
#define GCS_WARN(...)      SPDLOG_WARN(__VA_ARGS__)
#define GCS_ERROR(...)     SPDLOG_ERROR(__VA_ARGS__)
#define GCS_CRITICAL(...)  SPDLOG_CRITICAL(__VA_ARGS__)
