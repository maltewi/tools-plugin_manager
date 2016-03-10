#pragma once

#include <string>
#include "BaseClass.hpp"

namespace plugin_manager
{

class StringPlugin : public BaseClass
{
public:
    StringPlugin();

    std::string data;
};

}