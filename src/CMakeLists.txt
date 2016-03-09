rock_library(plugin_manager
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(plugin_manager_bin Main.cpp
    DEPS plugin_manager)

