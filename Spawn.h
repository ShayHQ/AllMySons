#pragma once

#include <string>
#include <vector>
#include <functional>

#define READ_BUFFER_SIZE 255

#define PID_T unsigned int

typedef void* HANDLE;
typedef void* ProcStartInfo;

#ifdef WIN32
#define PipeHandle HANDLE
#else
#define PipeHandle int
#endif

namespace spawnchild{
    class StdioPipe{
        bool isBlocking = true;
        PipeHandle redirect_out[2];
        PipeHandle redirect_in[2];
        PipeHandle redirect_err[2];
    public:
        virtual std::string readOut();
        virtual void writeIn(std::string);
        virtual std::string readErr();
        inline void setReadBlocking(bool block) {this->isBlocking = block;}
    protected:
        StdioPipe();
        ~StdioPipe();
#ifdef WIN32
        void replace_stdio(ProcStartInfo startInfo);
#else
        void replace_stdio();
#endif
        friend class Spawn;
    };

    class Spawn
    {
    protected:
        StdioPipe* redirected_stdio = nullptr;
        PID_T processPID = 0;
        HANDLE processHandle = nullptr;
        std::string path;
        int status;
        
    public:
        Spawn(std::string&, std::vector<std::string>&);
        ~Spawn();
        bool isAlive();
    private:
        std::vector<char *> getCArgs(std::string&, std::vector<std::string>&);
        void runProcess(std::vector<char *>);
        bool isChild();
    protected:
        virtual void setupPipes();
        void destroyPipes();
    };
};