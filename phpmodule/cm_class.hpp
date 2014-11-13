#ifndef CM_CM_H
#define CM_CM_H

#include <string>
#include <map>
#include <vector>

#include "cm_interface.hpp"
#include "cm_adapter.hpp"
#include "cm_ServerPair.hpp"
#include "cm_RVal.hpp"

class Cm : public CmInterface {
    public:
        //object manage
        Cm(std::vector<ServerPair> config, bool debug);
        ~Cm();
        std::string getBackendString(unsigned char i);

        //methods
        //set Value to Key with isDependency feature, with Dependency parent, with Expire time
        virtual bool set(char *key, char *value, int value_len, char *dependency, long expire);
        //get Key
        virtual char* get(char *key, size_t *return_value_length);
        //get Keys list
        virtual std::map<std::string, RVal> mget(std::vector<std::string> keys);
        //remove Key (only this key, no children)
        virtual bool remove(char *key);
        //add Value to Key if key not exists
        virtual bool add(char *key, char *value, int value_len, char *dependency, long expire);
        //flush all values in all backends
        virtual bool flush();
        //flush all values in one backend
        virtual bool flush(long shardId);
    private:
        std::vector<CmAdapter*> getShardVector(long shardId);

        std::vector<std::vector<CmAdapter*> > shards;
        std::map<unsigned char, std::vector<CmAdapter*> > backends;
        std::map<unsigned char, std::string> backendStrings;
        std::map<std::string, CmAdapter*> realBackends;
        std::map<std::string, std::vector<CmAdapter*> > realBackendsReplicas;
        char* processSetDependency(char *value, int value_len, char *dependency, int *newValue_len);
        char* processGetDependency(char *value, int value_len, int *newValue_len);
        bool debug;
};

#endif /* CM_CM_H */
