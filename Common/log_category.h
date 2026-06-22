#pragma once

#include "commacro.h"

#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

COMMON_EXPORT void close_def_logger();
COMMON_EXPORT std::shared_ptr<spdlog::async_logger> cur_deft_logger();
COMMON_EXPORT void init_stdout_file_spdlog(const char *name = nullptr);
#define COMMON_TRACE(...) SPDLOG_LOGGER_TRACE(cur_deft_logger(), __VA_ARGS__)
#define COMMON_DEBUG(...) SPDLOG_LOGGER_DEBUG(cur_deft_logger(), __VA_ARGS__)
#define COMMON_INFO(...) SPDLOG_LOGGER_INFO(cur_deft_logger(), __VA_ARGS__)
#define COMMON_WARN(...) SPDLOG_LOGGER_WARN(cur_deft_logger(), __VA_ARGS__)
#define COMMON_ERROR(...) SPDLOG_LOGGER_ERROR(cur_deft_logger(), __VA_ARGS__)
#define COMMON_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(cur_deft_logger(), __VA_ARGS__)
