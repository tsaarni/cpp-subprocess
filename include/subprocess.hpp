//
// subprocess C++ library - https://github.com/tsaarni/cpp-subprocess
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Tero Saarni
//

#include <string>
#include <vector>
#include <iostream>
#include <ext/stdio_filebuf.h>
#include <cstdio>
#include <system_error>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


namespace subprocess
{

class popen
{   
public:
   
    popen(std::string cmd, std::vector<std::string> argv)
    {
        argv.insert(argv.begin(), cmd);

        if (pipe(in_pipe)  == -1 ||
            pipe(out_pipe) == -1 ||
            pipe(err_pipe) == -1 )
        {
            throw std::system_error(errno, std::system_category());
        }
      
        pid = fork();
      
        if (pid == 0) child(argv);

        ::close(in_pipe[READ]);
        ::close(out_pipe[WRITE]);
        ::close(err_pipe[WRITE]);
        
        in_filebuf  = new __gnu_cxx::stdio_filebuf<char>(in_pipe[WRITE], std::ios_base::out, 1);
        out_filebuf = new __gnu_cxx::stdio_filebuf<char>(out_pipe[READ], std::ios_base::in, 1);
        err_filebuf = new __gnu_cxx::stdio_filebuf<char>(err_pipe[READ], std::ios_base::in, 1);

        in_stream  = new std::ostream(in_filebuf);
        out_stream = new std::istream(out_filebuf);
        err_stream = new std::istream(err_filebuf);
    }

    ~popen()
    {
        delete out_filebuf;
        delete in_filebuf;
        delete err_filebuf;
        delete out_stream;
        delete in_stream;
        delete err_stream;
    }

    std::ostream& stdin()  { return *in_stream;  };
    std::istream& stdout() { return *out_stream; };
    std::istream& stderr() { return *err_stream; };
   
    int wait()
    {
        int status = 0;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    };

    void close()
    {
        in_filebuf->close();
    }
    
    
private:
    
    enum ends_of_pipe { READ = 0, WRITE = 1 };

    struct raii_char_str
    {
        raii_char_str(std::string s) : buf(s.c_str(), s.c_str() + s.size() + 1) { };
        operator char*() const { return &buf[0]; };
        mutable std::vector<char> buf;
    };
   
    void child(std::vector<std::string> argv)
    {
        if (::close(in_pipe[WRITE]) == -1 ||
            ::close(out_pipe[READ]) == -1 ||
            ::close(err_pipe[READ]) == -1 )
        {
            std::perror("subprocess: close() failed");
            return;
        }
        
        if (dup2(in_pipe[READ], STDIN_FILENO)    == -1 ||
            dup2(out_pipe[WRITE], STDOUT_FILENO) == -1 ||
            dup2(err_pipe[WRITE], STDERR_FILENO) == -1 )
        {
            std::perror("subprocess: dup2() failed");
            return;
        }
      
        std::vector<raii_char_str> real_args(argv.begin(), argv.end());
        std::vector<char*> cargs(real_args.begin(), real_args.end());
        cargs.push_back(nullptr);
        
        if (execvp(cargs[0], &cargs[0]) == -1)
        {
            std::perror("subprocess: execvp() failed");
            return;
        }
    }

    pid_t pid;

    int in_pipe[2];
    int out_pipe[2];
    int err_pipe[2];
   
    __gnu_cxx::stdio_filebuf<char>* in_filebuf;
    __gnu_cxx::stdio_filebuf<char>* out_filebuf;
    __gnu_cxx::stdio_filebuf<char>* err_filebuf;

    std::ostream* in_stream;
    std::istream* out_stream;
    std::istream* err_stream;
};

} // namespace: subprocess
