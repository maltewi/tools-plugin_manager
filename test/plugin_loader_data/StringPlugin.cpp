#include "StringPlugin.hpp"
#include <class_loader/class_loader_register_macro.h>

namespace plugin_manager
{

StringPlugin::StringPlugin()
{

}

}

CLASS_LOADER_REGISTER_CLASS(plugin_manager::StringPlugin, plugin_manager::BaseClass);