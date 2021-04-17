#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include "Spawn.h"

#define READ_BUFFER_SIZE 255

using namespace spawnchild;

bool Spawn::isChild(){
    return this->processPID == 0;
}

bool Spawn::isAlive(){
    bool alive = false;

    if (this->processPID > 0){
        alive = waitpid(this->processPID, &this->status, WNOHANG) == 0;
    }

    return alive;
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
    setupPipes();
    runProcess(getCArgs(processPath ,args));
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