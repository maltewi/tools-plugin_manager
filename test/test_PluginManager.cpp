#include <boost/test/unit_test.hpp>
#include <plugin_manager/PluginManager.hpp>

using namespace plugin_manager;

BOOST_AUTO_TEST_CASE(plugin_manager_test)
{
    // load xml files
    std::vector<std::string> xml_paths;
    const char* root_folder = std::getenv("AUTOPROJ_CURRENT_ROOT");
    BOOST_CHECK(root_folder != NULL);
    std::string root_folder_str(root_folder);
    root_folder_str += "/tools/plugin_manager/test/plugin_manager_data";
    xml_paths.push_back(root_folder_str);
    PluginManager plugin_manager(xml_paths, false);

    // check available classes
    std::vector<std::string> available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 3);
    BOOST_CHECK(std::find(available_classes.begin(), available_classes.end(), "envire::VectorPlugin") != available_classes.end());
    BOOST_CHECK(std::find(available_classes.begin(), available_classes.end(), "envire::FakePlugin") != available_classes.end());
    BOOST_CHECK(std::find(available_classes.begin(), available_classes.end(), "envire::StringPlugin") != available_classes.end());

    // check if they are both of the same type
    available_classes = plugin_manager.getAvailableClasses("envire::core::ItemBase");
    BOOST_CHECK(available_classes.size() == 3);

    // check if class info is available
    BOOST_CHECK(plugin_manager.isClassInfoAvailable("VectorPlugin"));
    BOOST_CHECK(plugin_manager.isClassInfoAvailable("envire::FakePlugin"));
    BOOST_CHECK(plugin_manager.isClassInfoAvailable("envire::StringPlugin"));
    BOOST_CHECK(plugin_manager.isClassInfoAvailable("UnknownPlugin") == false);

    // get base class with full type name
    std::string base_class;
    BOOST_CHECK(plugin_manager.getBaseClass("envire::VectorPlugin", base_class));
    BOOST_CHECK(base_class == "envire::core::ItemBase");

    // get base class with class name only
    base_class.clear();
    BOOST_CHECK(plugin_manager.getBaseClass("VectorPlugin", base_class));
    BOOST_CHECK(base_class == "envire::core::ItemBase");

    // get base class of not existing class
    base_class.clear();
    BOOST_CHECK(plugin_manager.getBaseClass("UnknownPlugin", base_class) == false);

    // get associated classes
    std::vector<std::string> associated_classes;
    BOOST_CHECK(plugin_manager.getAssociatedClasses("VectorPlugin", associated_classes));
    BOOST_CHECK(associated_classes.size() == 1);
    BOOST_CHECK(associated_classes.front() == "Eigen::Vector3d");
    BOOST_CHECK(plugin_manager.getAssociatedClasses("FakePlugin", associated_classes) == false);

    // get library path
    std::string library_path;
    BOOST_CHECK(plugin_manager.getClassLibraryPath("envire::VectorPlugin", library_path));
    BOOST_CHECK(library_path == "envire_vector_plugin");

    // check singleton flag
    bool is_singleton = false;
    BOOST_CHECK(plugin_manager.getSingletonFlag("envire::VectorPlugin", is_singleton));
    BOOST_CHECK(is_singleton == false);
    BOOST_CHECK(plugin_manager.getSingletonFlag("envire::StringPlugin", is_singleton));
    BOOST_CHECK(is_singleton == true);

    // get all registered libraries
    std::set<std::string> libs = plugin_manager.getRegisteredLibraries();
    BOOST_CHECK(libs.size() == 2);
    BOOST_CHECK(std::find(libs.begin(), libs.end(), "envire_vector_plugin") != libs.end());
    BOOST_CHECK(std::find(libs.begin(), libs.end(), "envire_string_plugin") != libs.end());

    // remove one of the classes
    BOOST_CHECK(plugin_manager.removeClassInfo("envire::FakePlugin"));

    available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 2);

    // remove all classes
    plugin_manager.clear();
    available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 0);

    // reload class info
    plugin_manager.reloadXMLPluginFiles();
    available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 3);
}
