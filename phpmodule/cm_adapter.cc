#include <iostream>
#include <stdio.h>

#include <libmemcached/memcached.h>

#include "cm_debug.hpp"
#include "cm_adapter.hpp"
#include "cm_RVal.hpp"

CmAdapter::CmAdapter(const char* serverName, int serverPort, bool stable, bool debug)
{
    this->stable = stable;
    this->debug = debug;

    IFDEBUG_A
        INFO("PHPCM: Debug output %s", "started");
    IFDEBUG_E

    char *config_string= new char [strlen(serverName)+strlen("--SERVER=:")+1+6];
    sprintf(config_string, "--SERVER=%s:%d", serverName, serverPort);
    memc= memcached(config_string, strlen(config_string));
}

CmAdapter::~CmAdapter()
{
    IFDEBUG_A
        INFO("PHPCM: connection: %s", "closing");
    IFDEBUG_E

    memcached_free(memc);

    IFDEBUG_A
        INFO("PHPCM: connection: %s", "closed");
        INFO("PHPCM: Debug output %s", "done");
    IFDEBUG_E
}

bool CmAdapter::flush()
{
    IFDEBUG_A
        INFO("PHPCM: flush: %s", "no info");
    IFDEBUG_E
    memcached_return_t rc= memcached_flush(memc, 0);
    if (rc == MEMCACHED_SUCCESS) {
        IFDEBUG_A
            INFO("PHPCM: flush: %s", "success");
        IFDEBUG_E
        return true;
    } else {
        IFDEBUG_A
            INFO("PHPCM: flush: %s", "fail");
        IFDEBUG_E
        return false;
    }
}

bool CmAdapter::set(char* key, char* val, int val_len, long expire)
{
    IFDEBUG_A
        INFO("PHPCM: set: key=%s", key);
    IFDEBUG_E

    memcached_return_t rc= memcached_set(memc, key, strlen(key), val, val_len, (time_t)expire, (uint32_t)0);
    if (rc != MEMCACHED_SUCCESS) {
        IFDEBUG_A
            INFO("PHPCM: set: result=%s", "fail");
        IFDEBUG_E
        return false;
    } else {
        IFDEBUG_A
            INFO("PHPCM: set: result=%s", "success");
        IFDEBUG_E
        return true;
    }
}

bool CmAdapter::add(char* key, char* val, int val_len, long expire)
{
    IFDEBUG_A
        INFO("PHPCM: set: key=%s", key);
    IFDEBUG_E

    memcached_return_t rc= memcached_add(memc, key, strlen(key), val, val_len, (time_t)expire, (uint32_t)0);
    if (rc != MEMCACHED_SUCCESS) {
        IFDEBUG_A
            INFO("PHPCM: add: result=%s", "notset/fail");
        IFDEBUG_E
        return false;
    } else {
        IFDEBUG_A
            INFO("PHPCM: add: result=%s", "success");
        IFDEBUG_E
        return true;
    }
}


char* CmAdapter::get(char *key, size_t *return_value_length)
{
    IFDEBUG_A
        INFO("PHPCM: get: key=%s", key);
    IFDEBUG_E

    uint32_t flags=0;
    memcached_return_t error;

    IFDEBUG_A
        char* ret = memcached_get(memc, key, strlen (key), return_value_length, &flags, &error);
        if (ret == NULL) {
            INFO("PHPCM: get: result=%s", "fail/or-not-set");
        } else {
            INFO("PHPCM: get: result=%s", "success/isset");
        }
        return ret;
    IFDEBUG_ELSE
        return memcached_get(memc, key, strlen (key), return_value_length, &flags, &error);
    IFDEBUG_E
}

bool CmAdapter::remove(char *key)
{
    IFDEBUG_A
        INFO("PHPCM: delete: key=%s", key);
    IFDEBUG_E
    if (memcached_delete(memc, key, strlen(key), 0) == MEMCACHED_SUCCESS) {
        IFDEBUG_A
            INFO("PHPCM: delete: result=%s", "success");
        IFDEBUG_E
        return true;
    } else {
        IFDEBUG_A
            INFO("PHPCM: delete: result=%s", "fail/or-not-set");
        IFDEBUG_E
        return false;
    }
}

bool CmAdapter::isStable()
{
    return this->stable;
}
