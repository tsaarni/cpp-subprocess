
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS  = test-subprocess

test_subprocess_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS) -I include
test_subprocess_LDADD    = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS)
test_subprocess_SOURCES  = tests/test-subprocess.cpp

test: test-subprocess
	./test-subprocess --catch_system_errors=no
