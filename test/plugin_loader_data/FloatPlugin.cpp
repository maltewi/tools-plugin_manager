#include "FloatPlugin.hpp"
#include <class_loader/class_loader_register_macro.h>

namespace plugin_manager
{

FloatPlugin::FloatPlugin()
{

}

}

CLASS_LOADER_REGISTER_CLASS(plugin_manager::FloatPlugin, plugin_manager::BaseClass);