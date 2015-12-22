
# cpp-subprocess

[![Build Status](https://travis-ci.org/tsaarni/cpp-subprocess.svg?branch=master)](https://travis-ci.org/tsaarni/cpp-subprocess)

*subprocess* is a small header-only library that provides convenient
C++ API to execute shell commands.  It is similar to `popen()`, as it
creates a new process and executes the command by invoking shell.  The
difference compared to `popen()` is that stdin, stdout and stderr of
the child process are associated to iostream objects which can be
accessed by the parent process.

The library uses GNU extension `__gnu_cxx::stdio_filebuf` to construct
iostream objects from file descriptors.  It works on gcc and clang.


### Tutorial

The first example executes `ls` and print the directory listing of
the current working directory into standard out.

```C++
#include "subprocess.hpp"

int
main(int argc, char *argv[])
{
    subprocess::popen cmd("ls", {});
    std::cout << cmd.stdout().rdbuf();
    
    return 0;
}
```

The second example executes `sort -r` and write the contents of
`inputfile.txt` to the standard input of the executed command.  It
will then read the standard output of the command and print it.  The
resulting output is the reverse sorted contents of the
`inputfile.txt`.


```C++
#include <fstream>
#include "subprocess.hpp"

int
main(int argc, char *argv[])
{  
    subprocess::popen cmd("sort", {"-r"});

    std::ifstream file("inputfile.txt");
    std::string line;
    
    while (std::getline(file, line))
    {
       cmd.stdin() << line << std::endl;
    }
    cmd.close();
   
    std::cout << cmd.stdout().rdbuf();
    
    return 0;
}
```



### Installation

The library consists of single file `include/subprocess.hpp`.  Copy
the file into your own project or e.g. into `/usr/local/include/`.
