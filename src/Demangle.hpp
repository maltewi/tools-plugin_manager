#pragma once

#include <string>
#include <typeindex>

namespace plugin_manager
{
  
std::string demangleTypeName(const std::type_index& type);

}
