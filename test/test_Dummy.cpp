#include <boost/test/unit_test.hpp>
#include <plugin_manager/Dummy.hpp>

using namespace plugin_manager;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    plugin_manager::DummyClass dummy;
    dummy.welcome();
}
