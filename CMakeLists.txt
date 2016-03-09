# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
set(ROCK_USE_CXX11 TRUE)
find_package(Rock)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

rock_init(plugin_manager 0.1)
rock_standard_layout()

# install PluginManager.cmake extension
configure_file(cmake/pluginmanager-config.cmake.in pluginmanager-config.cmake @ONLY)
install(FILES cmake/PluginManager.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/pluginmanager-config.cmake
        DESTINATION share/rock/cmake)
