#include <iostream>
#include <stdio.h>

#include <libmemcached/memcached.h>

#include "cm_adapter.hpp"

CmAdapter::CmAdapter(const char* serverName, int serverPort, bool stable)
{
    std::cout << "Construct Adapter" << std::endl;
    this->stable = stable;
    char *config_string= new char [strlen(serverName)+strlen("--SERVER=:")+1+6];
    sprintf(config_string, "--SERVER=%s:%d", serverName, serverPort);
    memc= memcached(config_string, strlen(config_string));
}

CmAdapter::~CmAdapter()
{
    std::cout << "Destroy Adapter" << std::endl;
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

bool CmAdapter::isStable()
{
    return this->stable;
}
