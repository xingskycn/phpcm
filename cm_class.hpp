#ifndef CM_CM_H
#define CM_CM_H

#include <map>
#include <vector>

#include "cm_adapter.hpp"

// A very simple car class
class Cm {
    public:
        Cm();
        //set Value to Key with isDependency feature, with Dependency parent, with Expire time
        bool set(char *key, char *value, int value_len, bool isDependency, char *dependency, long expire);
        //get Key
        char* get(char *key, size_t *return_value_length, bool byDependency);
        //remove Key (only this key, no children)
        bool remove(char *key);
        //remove all children Keys (this works only for isDependency keys)
        bool expire(char *key);
        //remove all children Keys and self Key (if Key is not isDependency key remove only Key)
        bool expireOnlyChildren(char *key);
    private:
        std::map<unsigned char, std::vector<CmAdapter*> > backends;
        char* processSetDependency(char *value, int value_len, char *dependency, int *newValue_len);
        char* processGetDependency(char *value, int value_len, int *newValue_len);
};

#endif /* CM_CM_H */
