#include <future>
#include <signal.h>
#include "AsyncSpawn.h"

using namespace spawnchild;

#ifdef WIN32
#include <windows.h>
void AsyncSpawn::onExit(){
    HANDLE mutex = CreateMutex(nullptr, false, nullptr);
    while (!this->stop){
        WaitForSingleObject(this->processHandle, 0);
        if (GetExitCodeProcess(this->processHandle, (LPDWORD)&this->status) && this->status != STILL_ACTIVE){
            if (!this->status){
                events.onExit(this->status, false);
                this->stop = true;
            }else{
                events.onExit(this->status, true);
                this->stop = true;
            }
        }else{
            CloseHandle(mutex);
            throw GetLastError();
        }
        CloseHandle(mutex);
    }
}

void AsyncSpawn::killProcess(){
    if (isAlive()){
        TerminateProcess(this->processHandle, 0);
    }
    destroyPipes();
    this->stop = true;
}
#else
#include <sys/wait.h>
#include <sys/types.h>
#include <mutex>

void AsyncSpawn::onExit(){
    while (!this->stop){
        waitpid(this->processPID, &this->status, 0);
        std::mutex mtx;
        mtx.lock();
        if (WIFEXITED(this->status)){
            events.onExit(WEXITSTATUS(this->status), false);
            this->stop = true;
        }
        if (WIFSIGNALED(this->status)){
            events.onExit(WTERMSIG(this->status), true);
            this->stop = true;
        }
        mtx.unlock();
    }
}

void AsyncSpawn::killProcess(){
    if (isAlive()){
        kill(this->processPID, SIGKILL);
    }
    destroyPipes();
    this->stop = true;
}
#endif

void AsyncSpawn::send(std::string data){
    this->redirected_stdio->writeIn(data);
}

AsyncSpawn::AsyncSpawn(std::string& processPath, std::vector<std::string>& args, AsyncSpawnEvents events)
: Spawn(processPath, args){
    this->events = events;
    connectEvents();
}

AsyncSpawn::~AsyncSpawn(){
    killProcess();
    waitEvents();
}

void AsyncSpawn::onMessage(){
    while (!this->stop){
        auto output = this->redirected_stdio->readOut();
        if (output.size() > 0){
            events.onMessage(output);
        }
    }
}

void AsyncSpawn::onError(){
    while (!this->stop){
        auto output = this->redirected_stdio->readErr();
        if (output.size()){
            events.onError(output);
        }
    }
}

void AsyncSpawn::connectEvents(){
    eventListeners.push_back(std::async(std::bind(&AsyncSpawn::onExit, this)));
    eventListeners.push_back(std::async(std::bind(&AsyncSpawn::onMessage, this)));
    eventListeners.push_back(std::async(std::bind(&AsyncSpawn::onError, this)));
}

void AsyncSpawn::waitEvents(){
    for (const auto& listener : eventListeners){
        listener.wait();
    }
}