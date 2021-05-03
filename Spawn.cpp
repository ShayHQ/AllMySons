#include <stdio.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "Spawn.h"

#define READ_BUFFER_SIZE 255

using namespace spawnchild;

#ifdef WIN32
#include <windows.h>

bool Spawn::isAlive(){
    bool alive = false;
    if (this->processPID > 0 && this->processHandle){
        GetExitCodeProcess(this->processHandle, (LPDWORD)&this->status);
        alive = this->status == STILL_ACTIVE;
    }

    return alive;
}

std::string getCmdLine(std::vector<char *> args){
    std::string cmdline;
    for (const char* arg : args){
        cmdline += arg;
        cmdline += " ";
    }

    return cmdline;
}
void Spawn::runProcess(std::vector<char *> args){
    STARTUPINFO startInfo;
    PROCESS_INFORMATION piProcInfo;
    this->redirected_stdio->replace_stdio(&startInfo);
    if (!CreateProcess(this->path.c_str(), getCmdLine(args).c_str(), nullptr, nullptr, true, 0, nullptr, nullptr, &startInfo, &piProcInfo)){
        throw std::runtime_error("Process was not created " + this->path);
    }

    this->processPID = piProcInfo.dwProcessId;
    this->processHandle = piProcInfo.hProcess;
}
#else
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

bool Spawn::isAlive(){
    bool alive = false;

    if (this->processPID > 0){
        alive = waitpid(this->processPID, &this->status, WNOHANG) == 0;
    }

    return alive;
}

void Spawn::runProcess(std::vector<char *> args){
    this->processPID = fork();
    if (isChild()){
        this->redirected_stdio->replace_stdio();
        destroyPipes();
        execvp(args[0], args.data());
        exit(-1);
    }
}

#endif

bool Spawn::isChild(){
    return this->processPID == 0;
}

Spawn::~Spawn(){
    destroyPipes();
}

std::vector<char*> Spawn::getCArgs(std::string& processPath, std::vector<std::string>& args){
    std::vector<char *> c_args;
    c_args.reserve(args.size() + 2);

    c_args.push_back(const_cast<char*>(processPath.c_str()));
    for (const auto& arg : args){
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    return c_args;
}

Spawn::Spawn(std::string& processPath, std::vector<std::string>& args){
    this->path = processPath;
    setupPipes();
    runProcess(getCArgs(processPath ,args));
}


void Spawn::setupPipes(){
    if (!this->redirected_stdio){
        this->redirected_stdio = new StdioPipe();
    }
}

void Spawn::destroyPipes(){
    if (this->redirected_stdio){
        delete this->redirected_stdio;
    }
    this->redirected_stdio = nullptr;
}