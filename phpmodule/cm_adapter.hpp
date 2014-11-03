#ifndef CM_CMADAPTER_H
#define CM_CMADAPTER_H

#include <map>
#include <vector>
#include <string>

#include <libmemcached/memcached.h>

#include "cm_RVal.hpp"

class CmAdapter {
    public:
        CmAdapter(const char* serverName, int serverPort, bool stable, bool debug);
        ~CmAdapter();
        bool set(char* key, char* val, int val_len, long expire);
        char* get(char *key, size_t *return_value_length);
        bool remove(char* key);
        bool add(char* key, char* val, int val_len, long expire);
        bool isStable();
    private:
        memcached_st *memc;
        bool stable;
        bool debug;
};

#endif
