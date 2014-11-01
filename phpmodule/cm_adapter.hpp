#ifndef CM_CMADAPTER_H
#define CM_CMADAPTER_H

#include <libmemcached/memcached.h>

class CmAdapter {
    public:
        CmAdapter();
        ~CmAdapter();
        bool set(char* key, char* val, int val_len, long expire);
        char* get(char *key, size_t *return_value_length);
        bool remove(char* key);
    private:
        memcached_st *memc;
};

#endif
