
#include "SyncSpawn.h"

using namespace spawnchild;

#ifdef WIN32
#include <windows.h>

void SyncSpawn::sync(){
    this->redirected_stdio->writeIn(this->input);
    while (this->isAlive()){
        this->output += this->redirected_stdio->readOut();
    }
    this->output += redirected_stdio->readOut();

    if (this->status != STILL_ACTIVE){
        int returnedStatus = this->status;
        if (returnedStatus){
            std::string msg = this->processPID + " Program was terminated with non zero code";
            std::_Xruntime_error(msg.c_str());
        }
    }else{
        std::string msg = "Process: " + this->path + ": " + this->redirected_stdio->readErr();
        std::_Xruntime_error(msg.c_str());
    }
}
#else
#include <sys/wait.h>

void SyncSpawn::sync(){
    this->redirected_stdio->writeIn(this->input);
    std::string err = "";
    while(!waitpid(this->processPID, &this->status, WNOHANG)){
        this->output += redirected_stdio->readOut();
        err += redirected_stdio->readErr();
    }
    this->output += redirected_stdio->readOut();
    
    if (WIFEXITED(this->status)){
        int returnedStatus = WEXITSTATUS(this->status);
        if (returnedStatus){
            std::string msg = std::to_string(this->processPID) + " " + this->path + " Program was terminated with non zero code";
            std::__throw_runtime_error(msg.c_str());
        }
    }else{
        throw std::runtime_error("Process: "+ this->path +": " + err);
    }
}

#endif

SyncSpawn::SyncSpawn(std::string& processPath, std::vector<std::string>& args, std::string& input)
: Spawn(processPath, args){
    this->input = input;
    this->output = "";
    this->redirected_stdio->setReadBlocking(false);
    sync();
}

std::string SyncSpawn::getResult(){
    return this->output;
}