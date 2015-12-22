
#define BOOST_TEST_MODULE test_subprocess

#include <boost/test/unit_test.hpp>

#include "subprocess.hpp"


BOOST_AUTO_TEST_CASE(arguments)
{
    subprocess::popen cmd("echo", {"foo", "bar", "baz"});

    std::string buf;
    std::getline(cmd.stdout(), buf);

    BOOST_CHECK_EQUAL(buf, "foo bar baz");
}

BOOST_AUTO_TEST_CASE(stdio_forwarding)
{
    subprocess::popen cmd("cat", {});

    cmd.stdin() << "Hello world!" << std::endl;
    cmd.close();
    
    std::string buf;
    std::getline(cmd.stdout(), buf);
    
    BOOST_CHECK_EQUAL(buf, "Hello world!");
    BOOST_CHECK_EQUAL(cmd.wait(), 0);
}

BOOST_AUTO_TEST_CASE(return_values)
{
    subprocess::popen true_cmd("true", {});
    BOOST_CHECK_EQUAL(true_cmd.wait(), 0);
    
    subprocess::popen false_cmd("false", {});
    BOOST_CHECK_EQUAL(false_cmd.wait(), 1);
}

BOOST_AUTO_TEST_CASE(bad_command)
{
    subprocess::popen cmd("/non-existing-comand", {});

    BOOST_CHECK(cmd.wait() != 0);
}

