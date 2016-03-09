#include <iostream>
#include <plugin_manager/Dummy.hpp>

int main(int argc, char** argv)
{
    plugin_manager::DummyClass dummyClass;
    dummyClass.welcome();

    return 0;
}
