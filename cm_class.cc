#include <string.h>

#include "cm_class.hpp"
#include "crc32.h"

/*
* Object manage
*/

Cm::Cm() {
    CmAdapter* adapter = new CmAdapter();
    for (unsigned int i=0; i<=255; i++) {
	std::vector<CmAdapter*> one;
	one.push_back(adapter);
	backends[(unsigned char)i] = one;
    }
}

/*
* Public Interface
*/

bool Cm::set(char *key, char *value, int value_len, bool isDependency, char *dependency, long expire)
{
    bool summary=true;
    for (auto adapter : backends[key[0]]) {
        int newValue_len=0;
        char* newValue = this->processSetDependency(value, value_len, dependency, &newValue_len);
        summary = summary && adapter->set(key, newValue, newValue_len, expire);
    }
    return summary;
}

char* Cm::get(char *key, size_t *return_value_length, bool byDependency)
{
    for (auto adapter : backends[key[0]]) {
        char* result = adapter->get(key, return_value_length);
        if (result != NULL) {
            if (byDependency) {
                int newValue_len;
                char *newValue = this->processGetDependency(result, *return_value_length, &newValue_len);
                if (newValue_len == 0) {
                    return NULL;
                } else {
                    *return_value_length = newValue_len;
                    return newValue;
                }
            } else {
                return result;
            }
        }
    }
    return NULL;
}

bool Cm::remove(char *key)
{
    bool summary=true;
    for (auto adapter : backends[key[0]]) {
        summary = summary && adapter->remove(key);
    }
    return summary;
}

bool Cm::expire(char *key)
{
    return true;
}

bool Cm::expireOnlyChildren(char *key)
{
    return true;
}

/*
* Internal/private
*/

char* Cm::processSetDependency(char *value, int value_len, char *dependency, int *newValue_len)
{
    if (strlen(dependency) == 0) {
        (*newValue_len) = value_len;
        return value;
    }
    uint_least32_t crc32=0;
    size_t depLen = 0;
    char *depValue = this->get(dependency, &depLen);
    if (depLen == 0) {
        crc32 = Crc32Empty();
    } else {
        crc32 = Crc32((const unsigned char *)depValue, depLen);
    }
    *newValue_len = sizeof(char)*4 + sizeof(uint_least32_t) + value_len;
    char *newValue = new char [*newValue_len];
    char tmp[] = "DEP";
    memcpy(newValue, &tmp, sizeof(char)*4);
    memcpy(newValue+sizeof("DEP"), &crc32, sizeof(uint_least32_t));
    memcpy(newValue+sizeof("DEP")+sizeof(uint_least32_t), value, value_len);
    return newValue;
}

char* Cm::processGetDependency(char *value, int value_len, int *newValue_len)
{
    if (
        (value[0] == 'D') &&
        (value[1] == 'E') &&
        (value[2] == 'P') &&
        (value[3] == '\0')
    ) {
        uint_least32_t ocrc32=0;
        memcpy(&ocrc32, value+sizeof(char)*4, sizeof(uint_least32_t));
        
        uint_least32_t crc32=0;
        size_t depLen = 0;
        char *depValue = this->get(dependency, &depLen);
        if (depLen == 0) {
            crc32 = Crc32Empty();
        } else {
            crc32 = Crc32((const unsigned char *)depValue, depLen);
        }
        if (crc32 != ocrc32) {
            delete value;
            *newValue_len = 0;
            return NULL;
        } else {
            *newValue_len = value_len - sizeof(uint_least32_t) - sizeof(char)*4;
            if (*newValue_len <= 0) {
                return NULL;
            }
            char *newValue = new char [*newValue_len];
            memcpy(newValue, value+sizeof(uint_least32_t) + sizeof(char)*4);
            delete value;
            return newValue;
        }
    } else {
        *newValue_len = value_len;
        return value;
    }
}
