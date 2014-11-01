#ifndef CM_CMADAPTER_H
#define CM_CMADAPTER_H

#include <libmemcached/memcached.h>

class CmAdapter {
    public:
        CmAdapter(const char* serverName, int serverPort, bool stable);
        ~CmAdapter();
        bool set(char* key, char* val, int val_len, long expire);
        char* get(char *key, size_t *return_value_length);
        bool remove(char* key);
        bool isStable();
    private:
        memcached_st *memc;
        bool stable;
};

#endif
