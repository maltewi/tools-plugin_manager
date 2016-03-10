rock_library(plugin_manager_test_plugins
            SOURCES plugin_loader_data/FloatPlugin.cpp
                    plugin_loader_data/StringPlugin.cpp
            HEADERS plugin_loader_data/FloatPlugin.hpp
                    plugin_loader_data/StringPlugin.hpp
                    plugin_loader_data/BaseClass.hpp
            DEPS_PKGCONFIG class_loader)


rock_testsuite(test_suite suite.cpp
               test_PluginManager.cpp
               test_PluginLoader.cpp
   DEPS plugin_manager plugin_manager_test_plugins)