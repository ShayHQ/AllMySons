#pragma once

#include <string>
#include <vector>

class SyncSpawn
{
private:
    int pipesReplacedSTDOUT[2];
    std::string result;
    pid_t processPID;
    int status;
    
public:
    SyncSpawn(std::string&, std::vector<std::string>&);
    std::string getResult();
    ~SyncSpawn();
private:
    std::vector<char *> getCArgs(std::string&, std::vector<std::string>&);
    void setupPipes();
    void runProcess(std::vector<char *>);
    void syncReadResult();
    void closePipes();
    bool isChild();
};