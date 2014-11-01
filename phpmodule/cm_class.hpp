#ifndef CM_CM_H
#define CM_CM_H

#include <string>
#include <map>
#include <vector>

#include "cm_adapter.hpp"
#include "cm_ServerPair.hpp"

// A very simple car class
class Cm {
    public:
        Cm(std::vector<ServerPair> config);
        ~Cm();
        //set Value to Key with isDependency feature, with Dependency parent, with Expire time
        bool set(char *key, char *value, int value_len, char *dependency, long expire);
        //get Key
        char* get(char *key, size_t *return_value_length);
        //remove Key (only this key, no children)
        bool remove(char *key);
    private:
        std::map<unsigned char, std::vector<CmAdapter*> > backends;
        std::map<std::string, CmAdapter*> realBackends;
        std::map<std::string, std::vector<CmAdapter*> > realBackendsReplicas;
        char* processSetDependency(char *value, int value_len, char *dependency, int *newValue_len);
        char* processGetDependency(char *value, int value_len, int *newValue_len);
};

#endif /* CM_CM_H */
