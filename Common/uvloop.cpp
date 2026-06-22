#include "uvloop.h"

#include "log_category.h"

LoopHandle::LoopHandle()
    : by_file_record_res(), wait_resource_to_release(true),
      _async_destroy(nullptr), _thread(nullptr) {
    status.store(0);
}

LoopHandle::~LoopHandle() {
    if (_thread != nullptr) {
        uv_thread_join(&_thread);
        _thread = nullptr;
    }

    if (_async_destroy != nullptr) {
        free(_async_destroy);
        _async_destroy = nullptr;
    }
}

int LoopHandle::Initialize() {
    int res = 0;
    if (_async_destroy != nullptr) {
        COMMON_ERROR("has initlized");
        return UV_ENOMEM;
    }

    if ((res = uv_loop_init(&loop)) < 0) {
        COMMON_ERROR("init failed");
        return res;
    }

    _async_destroy = ((uv_async_t *) malloc(sizeof(uv_async_t)));
    res = uv_async_init(&loop, _async_destroy, [](uv_async_t *handle) {
        uv_close((uv_handle_t *) handle, [](uv_handle_t *handle) {
            COMMON_INFO("aync destroy loop");
            uv_stop(handle->loop);
        });
    });

    status.store(1);
    return res;
}

bool LoopHandle::AsyncRun(const char *by_file_record_res) {
    if (by_file_record_res != nullptr) {
        this->by_file_record_res = by_file_record_res;
    }

    uv_thread_create(
        &_thread,
        [](void *arg) {
            LoopHandle *handle = (LoopHandle *) arg;
            int rtn = uv_run(&handle->loop, UV_RUN_DEFAULT);
            COMMON_INFO("uv run result {}", rtn);
            if (!handle->by_file_record_res.empty()) {
                FILE *f = nullptr;
                fopen_s(&f, handle->by_file_record_res.c_str(), "w");
                if (f != nullptr) {
                    int resource_status = uv_loop_close(&handle->loop);
                    COMMON_INFO("resource status {}...", resource_status);
                    if (resource_status != 0) {
                        uv_print_all_handles(&handle->loop, f);
                    }
                    fclose(f);
                }
            }
            int resource_status = uv_loop_close(&handle->loop);
            COMMON_INFO("uv loop close {}", resource_status);
        },
        this);
    return true;
}

bool LoopHandle::SyncRun(const char *file_to_record_res_info) {
    uv_run(&loop, UV_RUN_DEFAULT);

    COMMON_INFO("sycn loop exit...");
    if (file_to_record_res_info != nullptr) {
        FILE *f = nullptr;
        fopen_s(&f, file_to_record_res_info, "w");
        if (f != nullptr) {
            int resource_status = uv_loop_close(&loop);
            COMMON_INFO("resource status {}...", resource_status);
            if (resource_status != 0) {
                uv_print_all_handles(&loop, f);
            }

            fclose(f);
        }
    }

    return true;
}

void LoopHandle::Destroy() {
    COMMON_INFO("call loop destroy function {}", (uint64_t) this);
    if (status.fetch_sub(1) > 0 && _async_destroy != nullptr) {
        uv_async_send(_async_destroy);
    }
}
