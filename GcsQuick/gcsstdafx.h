#pragma once

#if defined(CONTROL_SHAREDLIB)
#if defined(CONTROL_EXPORTS)
#define CONTROL_EXPORT __declspec(dllexport)
#else
#define CONTROL_EXPORT __declspec(dllimport)
#endif
#else
#define CONTROL_EXPORT
#endif

#define Q_PROPERTY_AUTO_P(TYPE, M)                   \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed) \
public:                                              \
    Q_SIGNAL void M##Changed();                      \
    void M(TYPE in_##M)                              \
    {                                                \
        _##M = in_##M;                               \
        Q_EMIT M##Changed();                         \
    }                                                \
    TYPE M() { return _##M; }                        \
                                                     \
private:                                             \
    TYPE _##M;

#define Q_PROPERTY_AUTO(TYPE, M)                     \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed) \
public:                                              \
    Q_SIGNAL void M##Changed();                      \
    void M(const TYPE& in_##M)                       \
    {                                                \
        _##M = in_##M;                               \
        Q_EMIT M##Changed();                         \
    }                                                \
    TYPE M() { return _##M; }                        \
                                                     \
private:                                             \
    TYPE _##M;
