#pragma once
#include "commacro.h"

class COMMON_EXPORT Uncopy {
  public:
    Uncopy() {}
    virtual ~Uncopy() {}
  private:
    Uncopy(const Uncopy &o) = delete;
    Uncopy &operator=(const Uncopy &o) = delete;
    Uncopy(const Uncopy &&o) = delete;
    Uncopy &operator=(const Uncopy &&o) = delete;
};
