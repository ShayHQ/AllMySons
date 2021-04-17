#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include "SyncSpawn.h"

#define READ_BUFFER_SIZE 255

bool SyncSpawn::isChild(){
    return this->processPID == 0;
}

void SyncSpawn::closePipes(){
    close(this->pipesReplacedSTDOUT[0]);
    close(this->pipesReplacedSTDOUT[1]);
}

SyncSpawn::~SyncSpawn(){
    closePipes();
}

std::string SyncSpawn::getResult(){
    syncReadResult();
    return this->result;
}

std::vector<char*> SyncSpawn::getCArgs(std::string& processPath, std::vector<std::string>& args){
    std::vector<char *> c_args;
    c_args.reserve(args.size() + 2);

    c_args.push_back(const_cast<char*>(processPath.c_str()));
    for (const auto& arg : args){
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    return c_args;
}

SyncSpawn::SyncSpawn(std::string& processPath, std::vector<std::string>& args){
    setupPipes();
    runProcess(getCArgs(processPath ,args));
}

void SyncSpawn::setupPipes(){
    if(pipe2(this->pipesReplacedSTDOUT, O_NONBLOCK) < 0){
        std::__throw_runtime_error("Failed to create pipes");
    }
}

void SyncSpawn::runProcess(std::vector<char *> args){
    this->processPID = fork();
    if (isChild()){
        dup2(this->pipesReplacedSTDOUT[1], STDOUT_FILENO);
        closePipes();
        execvp(args[0], args.data());
        exit(-1);
    }
}

void SyncSpawn::syncReadResult(){
    waitpid(this->processPID, &this->status, 0);
    if (WIFEXITED(this->status)){
        char buffer[READ_BUFFER_SIZE];
        size_t readBytes;
        FILE* alternateSTDOUT;

        alternateSTDOUT = fdopen(this->pipesReplacedSTDOUT[0], "r");
        while(alternateSTDOUT && (readBytes = fread(buffer, sizeof(char), READ_BUFFER_SIZE - 1, alternateSTDOUT)) > 0){
            buffer[readBytes] = 0;
            this->result.append(buffer);
        }
        if (alternateSTDOUT){
            fclose(alternateSTDOUT);
        }
    }
}