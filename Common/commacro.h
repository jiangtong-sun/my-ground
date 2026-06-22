#ifndef GCS_COMM_H
#define GCS_COMM_H

#if defined(COMMON_SHAREDLIB)
#if defined(COMMON_EXPORTS)
#define COMMON_EXPORT __declspec(dllexport)
#else
#define COMMON_EXPORT __declspec(dllimport)
#endif
#else
#define COMMON_EXPORT
#endif

#ifdef _WIN64
#define common_offsetoff(s, m)                                                                     \
    (size_t)((ptrdiff_t) & reinterpret_cast<const volatile char &>((((s *) 0)->m)))
#else
#define common_offsetoff(s, m) (size_t) & reinterpret_cast<const volatile char &>((((s *) 0)->m))
#endif

#define CommonAlloc(size) (char *) calloc(1, size)

#define CommonFree(addr) free(addr)

#define CommonPort(a) ((((a >> 8) & 0xff)) | ((a & 0xff) << 8))

#define common_containerof(ptr, type, member) ((type *) ((char *) (ptr) -offsetoff(type, member)))

#define COMMON_ASSERT_HANDLE(H)                                                                    \
    do {                                                                                           \
        assert(H != nullptr);                                                                      \
        if (H == nullptr) {                                                                        \
            COMMON_WARN(#H);                                                                       \
        }                                                                                          \
    } while (false)

#define COMMON_RESET_HANDLE(handle)                                                                \
    do {                                                                                           \
        if (handle) {                                                                              \
            free(handle);                                                                          \
            handle = nullptr;                                                                      \
        }                                                                                          \
    } while (false);

#define COMMON_DELETE_HANDLE(handle)                                                               \
    do {                                                                                           \
        if (handle) {                                                                              \
            delete (handle);                                                                       \
            handle = nullptr;                                                                      \
        }                                                                                          \
    } while (false);

#define COMMON_CHECK_HANDLE(exp, handle)                                                           \
    do {                                                                                           \
        if (exp != handle)                                                                         \
            return false;                                                                          \
    } while (false);

#define MSG_PROCESS_COMMON_FREQ 10

#endif
