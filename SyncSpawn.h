#include "Spawn.h"

class SyncSpawn : public Spawn
{
protected:
    std::string input;
public:
    SyncSpawn(std::string&, std::vector<std::string>&, std::string&);
    std::string getResult();
private:
    void sync();
};
