#include <stdio.h>
#include <unistd.h>
#include "Spawn.h"

std::string readStream(int fd, bool isBlocking);

using namespace spawnchild;
#ifdef WIN32
#include  <windows.h>

StdioPipe::StdioPipe(){
    bool isException = false;

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = true;
    saAttr.lpSecurityDescriptor = nullptr;

    isException |= !CreatePipe(this->redirect_out, this->redirect_out + 1, &saAttr, 0);
    isException |= !CreatePipe(this->redirect_in, this->redirect_in + 1, &saAttr, 0);
    isException |= !CreatePipe(this->redirect_err, this->redirect_err + 1, &saAttr, 0);

    if (isException){
        std::__throw_runtime_error("Failed to create alternate pipes");
    }

    if ( ! SetHandleInformation(this->redirect_in[1], HANDLE_FLAG_INHERIT, 0) ){
        std::__throw_runtime_error("write handle to the pipe for STDIN is inherited.");
    }
}

void StdioPipe::replace_stdio(ProcStartInfo startInfo){
    STARTUPINFO* implStartInfo = (STARTUPINFO*)startInfo;
    ZeroMemory(implStartInfo, sizeof(STARTUPINFO));
    implStartInfo->cb = sizeof(STARTUPINFO);
    implStartInfo->hStdError = this->redirect_err + 1;
    implStartInfo->hStdOutput = this->redirect_out + 1;
    implStartInfo->hStdInput = this->redirect_in;
    implStartInfo->dwFlags |= STARTF_USESTDHANDLES;
}

StdioPipe::~StdioPipe(){
    CloseHandle(this->redirect_out[0]);
    CloseHandle(this->redirect_out[1]);

    CloseHandle(this->redirect_in[0]);
    CloseHandle(this->redirect_in[1]);

    CloseHandle(this->redirect_err[0]);
    CloseHandle(this->redirect_err[1]);
}

std::string readStream(int fd, bool isBlocking){
    char readBuffer[READ_BUFFER_SIZE];
    DWORD bytesRead = 0;
    std::string result;

    while (ReadFile(HANDLE(fd), readBuffer, READ_BUFFER_SIZE - 1, &bytesRead, nullptr) && bytesRead > 0){
        result += readBuffer;
    }

    return result;
}

void StdioPipe::writeIn(std::string data){
    WriteFile(this->redirect_in[1], data.c_str(), data.size(), nullptr, nullptr);
    if (data.c_str()[data.size() - 1] != '\n'){
        WriteFile(this->redirect_in[1], "\n", 1, nullptr, nullptr);
    }
    WriteFile(this->redirect_in[1], 0, 1, nullptr, nullptr);
}

#else
#include <poll.h>

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

bool isDataAvailable(int fd, bool lie){
    struct pollfd queryRead;

    queryRead.fd = fd;
    queryRead.events = POLLIN;
    queryRead.revents = 0;
    int success = poll(&queryRead, 1, 100);
    return lie || (success > 0 && queryRead.revents & POLLIN);
}

std::string readStream(int fd, bool isBlocking){
    char readBuffer[READ_BUFFER_SIZE];
    ssize_t bytesRead;
    std::string result;

    while (isDataAvailable(fd, isBlocking) && (bytesRead = read(fd, readBuffer, READ_BUFFER_SIZE - 1)) > 0){
        readBuffer[bytesRead] = 0;
        result.append(readBuffer);

        if (bytesRead < READ_BUFFER_SIZE - 1){
            break;
        }
    }

    return result;
}

void StdioPipe::writeIn(std::string data){
    write(this->redirect_in[1], data.c_str(), data.size());
    if (data.c_str()[data.size() - 1] != '\n'){
        write(this->redirect_in[1], "\n", 1);
    }
    write(this->redirect_in[1], 0, 1);
}
#endif
std::string StdioPipe::readOut(){
    return readStream((int)this->redirect_out[0], this->isBlocking);
}

std::string StdioPipe::readErr(){
    return readStream((int)this->redirect_err[0], this->isBlocking);
}
