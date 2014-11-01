#include <libmemcached/memcached.h>

#include "cm_adapter.hpp"

CmAdapter::CmAdapter()
{
    const char *config_string= "--SERVER=localhost";
    memc= memcached(config_string, strlen(config_string));
}

CmAdapter::~CmAdapter()
{
    memcached_free(memc);
}

bool CmAdapter::set(char* key, char* val, int val_len, long expire)
{
    memcached_return_t rc= memcached_set(memc, key, strlen(key), val, val_len, (time_t)expire, (uint32_t)0);
    if (rc != MEMCACHED_SUCCESS) {
        return false;
    } else {
        return true;
    }
}

char* CmAdapter::get(char *key, size_t *return_value_length)
{
    uint32_t flags=0;
    memcached_return_t error;
    return memcached_get(memc, key, strlen (key), return_value_length, &flags, &error);
}

bool CmAdapter::remove(char *key)
{
    if (memcached_delete(memc, key, strlen(key), 0) == MEMCACHED_SUCCESS) {
        return true;
    } else {
        return false;
    }
}
