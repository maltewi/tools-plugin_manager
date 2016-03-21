#include <boost/test/unit_test.hpp>
#include <plugin_manager/PluginLoader.hpp>
#include "plugin_loader_data/BaseClass.hpp"
#include "plugin_loader_data/FloatPlugin.hpp"
#include "plugin_loader_data/StringPlugin.hpp"
#include <plugin_manager/Exceptions.hpp>

using namespace plugin_manager;


BOOST_AUTO_TEST_CASE(plugin_loader_test)
{
    PluginLoader* loader = PluginLoader::getInstance();
    std::vector<std::string> xml_paths;
    const char* root_folder = std::getenv("AUTOPROJ_CURRENT_ROOT");
    BOOST_CHECK(root_folder != NULL);
    std::string root_folder_str(root_folder);
    root_folder_str += "/tools/plugin_manager/test/plugin_loader_data";
    xml_paths.push_back(root_folder_str);
    loader->clear();
    loader->overridePluginXmlPaths(xml_paths);
    loader->reloadXMLPluginFiles();

    // check if elements are available
    BOOST_CHECK(PluginLoader::getInstance()->hasClass("StringPlugin"));
    BOOST_CHECK(PluginLoader::getInstance()->hasClass("FloatPlugin"));
    BOOST_CHECK(PluginLoader::getInstance()->hasClass("SomeNotExistingPlugin") == false);
    BOOST_CHECK(PluginLoader::getInstance()->hasClassOfType("StringPlugin", "plugin_manager::BaseClass"));
    BOOST_CHECK(PluginLoader::getInstance()->hasClassOfType("StringPlugin", "plugin_manager::BaseClass2") == false);

    // create instances
    boost::shared_ptr<BaseClass> base_plugin;
    BOOST_CHECK(PluginLoader::getInstance()->createInstance("StringPlugin", base_plugin));
    boost::shared_ptr<StringPlugin> string_plugin_a = boost::dynamic_pointer_cast<StringPlugin>(base_plugin);

    boost::shared_ptr<StringPlugin> string_plugin_b;
    BOOST_CHECK((PluginLoader::getInstance()->createInstance<StringPlugin, BaseClass>("StringPlugin", string_plugin_b)));

    BOOST_CHECK(string_plugin_a.get() != string_plugin_b.get());
    BOOST_CHECK(string_plugin_a.use_count() == 2);
    BOOST_CHECK(string_plugin_b.use_count() == 1);

    // create singleton instance
    {
        boost::shared_ptr<FloatPlugin> float_plugin_a;
        BOOST_CHECK((PluginLoader::getInstance()->createInstance<FloatPlugin, BaseClass>("FloatPlugin", float_plugin_a)));
        float_plugin_a->data = 0;
        BOOST_CHECK(float_plugin_a.use_count() == 2);

        boost::shared_ptr<FloatPlugin> float_plugin_b;
        BOOST_CHECK((PluginLoader::getInstance()->createInstance<FloatPlugin, BaseClass>("FloatPlugin", float_plugin_b)));
        float_plugin_b->data = 42;

        BOOST_CHECK(float_plugin_a.get() == float_plugin_b.get());
        BOOST_CHECK(float_plugin_a->data == float_plugin_b->data);
        BOOST_CHECK(float_plugin_a.use_count() == 3);
    }

    boost::shared_ptr<FloatPlugin> float_plugin;
    BOOST_CHECK((PluginLoader::getInstance()->createInstance<FloatPlugin, BaseClass>("FloatPlugin", float_plugin)));
    BOOST_CHECK(float_plugin->data == 42);

    // test down case exception
    boost::shared_ptr<FloatPlugin> string_plugin;
    typedef plugin_manager::DownCastException<FloatPlugin, BaseClass> DownCastExceptionType;
    BOOST_CHECK_THROW((PluginLoader::getInstance()->createInstance<FloatPlugin, BaseClass>("StringPlugin", string_plugin)),
                      DownCastExceptionType);

}
