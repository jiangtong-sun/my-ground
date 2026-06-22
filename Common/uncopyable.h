#pragma once
#include "commacro.h"
class COMMON_EXPORT Uncopyable {
  protected:
    Uncopyable(){}
    virtual ~Uncopyable(){}
  private:
    Uncopyable(const Uncopyable &o) = delete;
    Uncopyable &operator=(const Uncopyable &o) = delete;
    Uncopyable(const Uncopyable &&o) = delete;
    Uncopyable &operator=(const Uncopyable &&o) = delete;
};
