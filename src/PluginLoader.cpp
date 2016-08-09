#include "PluginLoader.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <glog/logging.h>

using namespace plugin_manager;
using namespace std;

PluginLoader::PluginLoader() : PluginManager()
{
    loadLibraryPaths();
}

PluginLoader::~PluginLoader()
{
    loaders.clear();
}

PluginLoader* PluginLoader::getInstance()
{
    return base::Singleton<PluginLoader>::getInstance();
}

bool PluginLoader::hasClass(const string& class_name) const
{
    return isClassInfoAvailable(class_name);
}

bool PluginLoader::hasClassOfType(const string& class_name, const string& base_class_name) const
{
    std::string base_class;
    if(getBaseClass(class_name, base_class) && base_class == base_class_name)
        return true;
    return false;
}

void PluginLoader::addLibraryPath(const string& library_path)
{
    string lib_path_trimed = library_path;
    boost::trim_right_if(lib_path_trimed, boost::is_any_of("/"));
    library_paths.insert(lib_path_trimed);
}

void PluginLoader::loadLibraryPaths()
{
    const char* lib_path = std::getenv("LD_LIBRARY_PATH");
    vector<string> paths;
    if(lib_path != NULL)
    {
        const string lib_paths(lib_path);
        //":" is the separator in LD_LIBRARY_PATH
        boost::split(paths, lib_paths, boost::is_any_of(":"));
        //trim ":" and " " from the beginning and end of the string
        for(string& path : paths)
        {
            boost::trim_if(path, boost::is_any_of(": "));
            boost::trim_right_if(path, boost::is_any_of("/"));
            library_paths.insert(path);
        }
    }
}

bool PluginLoader::loadLibrary(const std::string& class_name)
{
    if(library_paths.empty())
    {
        LOG(ERROR) << "Have no valid library paths. Please set LD_LIBRARY_PATH or add an library path manually.";
        return false;
    }

    std::string lib_name;
    if(!getClassLibraryPath(class_name, lib_name))
    {
        LOG(ERROR) << "Couldn't find library name for given class " << class_name;
        return false;
    }

    // check if the library was already loaded
    if(loaders.find(lib_name) != loaders.end())
        return true;

    //try to load the plugin from all available paths
    for(const string& lib_path : library_paths)
    {
        string path = lib_path + "/lib" + lib_name + ".so";
        if(boost::filesystem::exists(path))
        {
            boost::shared_ptr<class_loader::ClassLoader> loader;
            loader.reset(new class_loader::ClassLoader(path, false));
            if(loader && loader->isLibraryLoaded())
            {
                loaders.insert(std::make_pair(lib_name, loader));
                return true;
            }
            else
                LOG(WARNING) << "Failed to load library in " << path;
        }
    }

    LOG(ERROR) << "Failed to load a plugin library " << lib_name << " for class " << class_name;
    return false;
}
