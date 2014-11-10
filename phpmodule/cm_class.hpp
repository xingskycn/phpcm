#ifndef CM_CM_H
#define CM_CM_H

#include <string>
#include <map>
#include <vector>

#include "cm_adapter.hpp"
#include "cm_ServerPair.hpp"
#include "cm_RVal.hpp"

// A very simple car class
class Cm {
    public:
        Cm(std::vector<ServerPair> config, bool debug);
        ~Cm();
        //set Value to Key with isDependency feature, with Dependency parent, with Expire time
        bool set(char *key, char *value, int value_len, char *dependency, long expire);
        //get Key
        char* get(char *key, size_t *return_value_length);
        //get Keys list
        std::map<std::string, RVal> mget(std::vector<std::string> keys);
        //remove Key (only this key, no children)
        bool remove(char *key);
        //add Value to Key if key not exists
        bool add(char *key, char *value, int value_len, char *dependency, long expire);
        //flush all values in all backends
        bool flush();
        //flush all values in one backend
        bool flush(long shardId);
    private:
        std::vector<CmAdapter*> getShardVector(long shardId);

        std::vector<std::vector<CmAdapter*> > shards;
        std::map<unsigned char, std::vector<CmAdapter*> > backends;
        std::map<std::string, CmAdapter*> realBackends;
        std::map<std::string, std::vector<CmAdapter*> > realBackendsReplicas;
        char* processSetDependency(char *value, int value_len, char *dependency, int *newValue_len);
        char* processGetDependency(char *value, int value_len, int *newValue_len);
        bool debug;
};

#endif /* CM_CM_H */
