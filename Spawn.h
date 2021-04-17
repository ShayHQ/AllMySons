#pragma once

#include <string>
#include <vector>
#include <functional>

#define READ_BUFFER_SIZE 255

class StdioPipe{

    int redirect_out[2];
    int redirect_in[2];
    int redirect_err[2];
public:
    virtual std::string readOut();
    virtual void writeIn(std::string);
    virtual std::string readErr();
protected:
    StdioPipe();
    ~StdioPipe();
    void replace_stdio();

    friend class Spawn;
};

class Spawn
{
protected:
    StdioPipe* redirected_stdio = nullptr;
    pid_t processPID = 0;
    int status;
    
public:
    Spawn(std::string&, std::vector<std::string>&);
    ~Spawn();
    bool isAlive();
private:
    std::vector<char *> getCArgs(std::string&, std::vector<std::string>&);
    void runProcess(std::vector<char *>);
    bool isChild();
protected:
    virtual void setupPipes();
    void destroyPipes();
};