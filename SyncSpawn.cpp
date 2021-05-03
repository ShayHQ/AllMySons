
#include "SyncSpawn.h"

using namespace spawnchild;

#ifdef WIN32
#include <windows.h>

void SyncSpawn::sync(){
    this->redirected_stdio->writeIn(this->input);
    WaitForSingleObject(this->processHandle, 0);
    if (this->status != STILL_ACTIVE){
        int returnedStatus = this->status;
        if (returnedStatus){
            std::string msg = this->processPID + " Program was terminated with non zero code";
            std::__throw_runtime_error(msg.c_str());
        }
    }else{
        throw std::runtime_error("Process: "+ this->path +": " + this->redirected_stdio->readErr());
    }
}
#else
#include <sys/wait.h>

void SyncSpawn::sync(){
    this->redirected_stdio->writeIn(this->input);
    waitpid(this->processPID, &this->status, 0);
    if (WIFEXITED(this->status)){
        int returnedStatus = WEXITSTATUS(this->status);
        if (returnedStatus){
            std::string msg = this->processPID + " Program was terminated with non zero code";
            std::__throw_runtime_error(msg.c_str());
        }
    }else{
        throw std::runtime_error("Process: "+ this->path +": " + this->redirected_stdio->readErr());
    }
}

#endif

SyncSpawn::SyncSpawn(std::string& processPath, std::vector<std::string>& args, std::string& input)
: Spawn(processPath, args){
    this->input = input;
    sync();
}

std::string SyncSpawn::getResult(){
    return this->redirected_stdio->readOut();
}