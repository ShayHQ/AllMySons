#include <sys/wait.h>
#include "SyncSpawn.h"

SyncSpawn::SyncSpawn(std::string& processPath, std::vector<std::string>& args, std::string& input)
: Spawn(processPath, args){
    this->input = input;
    sync();
}

void SyncSpawn::sync(){
    this->redirected_stdio->writeIn(this->input);
    waitpid(this->processPID, &this->status, 0);
    if (WIFEXITED(this->status)){
        int returnedStatus = WEXITSTATUS(this->status);
        if (returnedStatus){
            std::string msg = this->processPID + " Program was terminated with non zero code";
            std::__throw_runtime_error(msg.c_str());
        }
    }
}

std::string SyncSpawn::getResult(){
    return this->redirected_stdio->readOut();
}