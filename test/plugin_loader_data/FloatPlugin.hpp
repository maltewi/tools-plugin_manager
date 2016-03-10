#pragma once

#include "BaseClass.hpp"

namespace plugin_manager
{

class FloatPlugin : public BaseClass
{
public:
    FloatPlugin();

    float data;
};

}