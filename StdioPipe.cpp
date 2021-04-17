#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "Spawn.h"

using namespace spawnchild;

StdioPipe::StdioPipe(){
    bool isException = false;

    isException |= pipe(this->redirect_out) < 0;
    isException |= pipe(this->redirect_in) < 0;
    isException |= pipe(this->redirect_err) < 0;

    if (isException){
        std::__throw_runtime_error("Failed to create alternate pipes");
    }
}

void StdioPipe::replace_stdio(){
    dup2(this->redirect_out[1], STDOUT_FILENO);
    dup2(this->redirect_in[0], STDIN_FILENO);
    dup2(this->redirect_err[1], STDERR_FILENO);
}

StdioPipe::~StdioPipe(){
    close(this->redirect_out[0]);
    close(this->redirect_out[1]);

    close(this->redirect_in[0]);
    close(this->redirect_in[1]);

    close(this->redirect_err[0]);
    close(this->redirect_err[1]);
}

std::string readStream(int fd){
    char readBuffer[READ_BUFFER_SIZE];
    ssize_t bytesRead;
    std::string result;

    while ((bytesRead = read(fd, readBuffer, READ_BUFFER_SIZE - 1)) > 0){
        readBuffer[bytesRead] = 0;
        result.append(readBuffer);

        if (bytesRead < READ_BUFFER_SIZE - 1){
            break;
        }
    }

    return result;
}

std::string StdioPipe::readOut(){
    return readStream(this->redirect_out[0]);
}

std::string StdioPipe::readErr(){
    return readStream(this->redirect_err[0]);
}

void StdioPipe::writeIn(std::string data){
    write(this->redirect_in[1], data.c_str(), data.size());
    if (data.c_str()[data.size() - 1] != '\n'){
        write(this->redirect_in[1], "\n", 1);
    }
    write(this->redirect_in[1], 0, 1);
}