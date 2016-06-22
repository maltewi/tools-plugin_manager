#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <class_loader/class_loader.h>
#include <base-logging/Singleton.hpp>
#include <glog/logging.h>

#include "PluginManager.hpp"
#include "Exceptions.hpp"

namespace plugin_manager
{

/**
 * @class PluginLoader
 * @brief A singleton class used to load class loader based plugins
 * This class inherits from the PluginManager
 */
class PluginLoader : public PluginManager, public boost::noncopyable
{
    friend class base::Singleton<PluginLoader>;
    typedef std::map<std::string, boost::shared_ptr<class_loader::ClassLoader> > LoaderMap;
    typedef std::map<std::string, boost::shared_ptr<void> > SingletonMap;

public:
    /**
     * @brief Returns the singleton instance of this class
     */
    static PluginLoader* getInstance();

    /**
     * @brief Returns true if the class is registerd
     * @param class_name the name of the plugin class
     * @returns True if class is available
     */
    bool hasClass(const std::string& class_name) const;

    /**
     * @brief Returns true if the class is registerd and inherits from the given base class.
     * @param class_name the name of the plugin class
     * @param base_class_name the name of the base class
     * @returns True if class is available
     */
    bool hasClassOfType(const std::string& class_name, const std::string& base_class_name) const;

    /**
     * @brief Creates an instance of the given class
     * @param class_name the name of the plugin class
     * @param instance pointer to the base class of the class
     * @return True if an instance of the class could be created
     */
    template<class BaseClass>
    bool createInstance(const std::string& class_name, boost::shared_ptr<BaseClass>& instance);

    /**
     * @brief Creates an instance of the given class and tries to down cast to the actual implementation.
     * @param class_name the name of the plugin class
     * @param instance pointer to the instance of the class
     * @return True if an instance of the class could be created
     * @throws DownCastException if the cast from BaseClass to InheritedClass isn't possible
     */
    template<class InheritedClass, class BaseClass>
    bool createInstance(const std::string& class_name, boost::shared_ptr<InheritedClass>& instance);

    /**
     * @brief Adds an additional library path to the set of library paths
     * Note: A set of paths is already looked up by using the environment variable LD_LIBRARY_PATH
     *       when this class is created.
     * @param library_path path to a library folder
     */
    void addLibraryPath(const std::string& library_path);

protected:
    /**
     * @brief Constructor for PluginLoader
     * It is protected because this class is a singleton class.
     */
    PluginLoader();

    /**
     * @brief Destructor for PluginLoader
     */
    virtual ~PluginLoader();

    /**
     * @brief Loads all paths set in the environment variable LD_LIBRARY_PATH
     *        to the set of library paths.
     */
    void loadLibraryPaths();

private:

    /**
     * @brief Uses the class_loader to create a new instance of the given class name.
     *        If the class is marked a singleton, only one instance will be created and
     *        returned on future queries.
     * @param derived_class_name name of the plugin class
     * @param loader the class loader
     * @param instance new or singleton instance
     */
    template<class BaseClass>
    void createInstanceIntern(const std::string& derived_class_name,
                                const boost::shared_ptr<class_loader::ClassLoader>& loader,
                                boost::shared_ptr< BaseClass >& instance);

    /**
     * @brief Loads the library of the given plugin class
     * @param class_name name of the plugin class
     * @return True if the library could be loaded
     */
    bool loadLibrary(const std::string& class_name);

private:
    /** Mapping between library name and class loader instances */
    LoaderMap loaders;

    /** Singleton instances that have been instantiated */
    SingletonMap singletons;

    /** Set of the known shared library folders */
    std::set<std::string> library_paths;
};

template<class BaseClass>
bool PluginLoader::createInstance(const std::string& class_name, boost::shared_ptr<BaseClass>& instance)
{
    // get library name of the class
    std::string lib_name;
    if(!getClassLibraryPath(class_name, lib_name))
    {
        LOG(ERROR) << "Could not find plugin library for class " << class_name;
        return false;
    }

    // find loader for the class
    LoaderMap::iterator it = loaders.find(lib_name);
    if(it == loaders.end() && loadLibrary(class_name))
        it = loaders.find(lib_name);

    if(it == loaders.end())
    {
        LOG(ERROR) << "Failed to load plugin library " << lib_name;
        return false;
    }

    // try to create an instance of the class
    if(it->second->isClassAvailable<BaseClass>(class_name))
    {
        createInstanceIntern<BaseClass>(class_name, it->second, instance);
        return true;
    }

    if(hasNamespace(class_name))
    {
        // try to create an instance of the class using the class name without namespace
        std::string short_class_name = removeNamespace(class_name);
        if(it->second->isClassAvailable<BaseClass>(short_class_name))
        {
            createInstanceIntern<BaseClass>(short_class_name, it->second, instance);
            return true;
        }
    }
    else
    {
        // try to create an instance of the class using the full class name
        std::string full_class_name;
        if(getFullClassName(class_name, full_class_name) && it->second->isClassAvailable<BaseClass>(full_class_name))
        {
            createInstanceIntern<BaseClass>(full_class_name, it->second, instance);
            return true;
        }
    }

    LOG(ERROR) << "Failed to create and instance of class " << class_name
                << ", it isn't available in the plugin library " << lib_name;
    return false;
}

template<class InheritedClass, class BaseClass>
bool PluginLoader::createInstance(const std::string& class_name, boost::shared_ptr<InheritedClass>& instance)
{
    boost::shared_ptr<BaseClass> base_instance;
    if(!createInstance<BaseClass>(class_name, base_instance))
        return false;

    instance = boost::dynamic_pointer_cast<InheritedClass>(base_instance);
    if(instance == NULL)
        throw DownCastException<InheritedClass, BaseClass>(class_name);
    return true;
}

template<class BaseClass>
void PluginLoader::createInstanceIntern(const std::string& derived_class_name,
                                        const boost::shared_ptr<class_loader::ClassLoader>& loader,
                                        boost::shared_ptr< BaseClass >& instance)
{
    bool singleton = false;
    if(getSingletonFlag(derived_class_name, singleton) && singleton)
    {
        // class is marked as singleton
        SingletonMap::iterator singleton_it = singletons.find(derived_class_name);
        if(singleton_it == singletons.end())
        {
            // create an stores a new instance
            instance = loader->createInstance<BaseClass>(derived_class_name);
            boost::shared_ptr< void > instance_ptr = boost::static_pointer_cast<void>(instance);
            singletons[derived_class_name] = instance_ptr;
        }
        else
        {
            // returns existing instance
            instance = boost::static_pointer_cast<BaseClass>(singleton_it->second);
        }
    }
    else
    {
        // create new instance
        instance = loader->createInstance<BaseClass>(derived_class_name);
    }
}

}