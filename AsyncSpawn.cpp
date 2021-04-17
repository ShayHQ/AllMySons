#include <sys/wait.h>
#include <sys/types.h>
#include <future>
#include <mutex>
#include <signal.h>
#include "AsyncSpawn.h"

AsyncSpawn::AsyncSpawn(std::string& processPath, std::vector<std::string>& args, AsyncSpawnEvents events)
: Spawn(processPath, args){
    this->events = events;
    connectEvents();
}

AsyncSpawn::~AsyncSpawn(){
    killProcess();
    waitEvents();
}

void AsyncSpawn::send(std::string data){
    this->redirected_stdio->writeIn(data);
}

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

void AsyncSpawn::killProcess(){
    if (isAlive()){
        kill(this->processPID, SIGKILL);
    }
    destroyPipes();
    this->stop = true;
}