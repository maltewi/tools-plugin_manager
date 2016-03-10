#pragma once

#include <stdint.h>

namespace plugin_manager
{

class BaseClass
{
public:
    virtual ~BaseClass() {}

    uint64_t time;

protected:
    BaseClass() {}
};

}