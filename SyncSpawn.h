#include "Spawn.h"


namespace spawnchild{
    class SyncSpawn : public Spawn
    {
    protected:
        std::string input;
        std::string output;
    public:
        SyncSpawn(std::string&, std::vector<std::string>&, std::string&);
        std::string getResult();
    private:
        void sync();
    };
}
