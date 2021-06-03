find_package(Boost COMPONENTS filesystem)

rock_library(plugin_manager
    SOURCES PluginManager.cpp
            PluginLoader.cpp
            Demangle.cpp
    HEADERS PluginInfo.hpp
            PluginManager.hpp
            PluginLoader.hpp
            Exceptions.hpp
            Demangle.hpp
    DEPS_PKGCONFIG tinyxml base-logging
    DEPS_CMAKE Glog 
    DEPS_PLAIN
        Boost_FILESYSTEM
        Boost_THREAD
        Boost_SYSTEM
        class_loader
        PocoFoundation
)

