#pragma once

#define DRAW_PROPERTY_AUTO(TYPE, M)                                                                \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                               \
  public:                                                                                          \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(const TYPE &in_##M) {                                                                   \
        _##M = in_##M;                                                                             \
        Q_EMIT M##Changed();                                                                       \
    }                                                                                              \
    TYPE M() { return _##M; }                                                                      \
                                                                                                   \
  protected:                                                                                       \
    TYPE _##M;
