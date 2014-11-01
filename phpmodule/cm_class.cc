#include <iostream>
#include <string.h>

#include "cm_class.hpp"
#include "crc32.h"
#include "cm_dependency.hpp"

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

bool Cm::set(char *key, char *value, int value_len, char *dependency, long expire)
{
    bool summary=true;
    int j = backends[key[0]].size();
    for (int i=0; i<j; i++) {
        int newValue_len=0;
        char* newValue = this->processSetDependency(value, value_len, dependency, &newValue_len);
        summary = summary && backends[key[0]][i]->set(key, newValue, newValue_len, expire);
    }
    return summary;
}

char* Cm::get(char *key, size_t *return_value_length)
{
    int j = backends[key[0]].size();
    for (int i=0; i<j; i++) {
        char* result = backends[key[0]][i]->get(key, return_value_length);
        if (result != NULL) {
            int newValue_len;
            char *newValue = this->processGetDependency(result, *return_value_length, &newValue_len);
            if (newValue_len == 0) {
                *return_value_length = 0;
                return NULL;
            } else {
                *return_value_length = newValue_len;
                return newValue;
            }
        }
    }
    *return_value_length = 0;
    return NULL;
}

bool Cm::remove(char *key)
{
    bool summary=true;
    int j = backends[key[0]].size();
    for (int i=0; i<j; i++) {
        summary = summary && backends[key[0]][i]->remove(key);
    }
    return summary;
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
    size_t outLength;
    CmDependency *depStruct = makeCmDependencyFromName(dependency, crc32, &outLength);
    if (!validateCmDependencyInRaw(depStruct, outLength)) {
	std::terminate();
    }
    *newValue_len = value_len + outLength;
    char *newValue = new char [*newValue_len];
    memcpy(newValue, depStruct, outLength);
    memcpy( (char*)((long)newValue + (int)outLength), value, value_len);
    return newValue;
}

char* Cm::processGetDependency(char *value, int value_len, int *newValue_len)
{
    if (validateCmDependencyInRaw(value, value_len)) {
        size_t depStructLen;
        CmDependency *depStruct = makeCmDependencyFromRaw(value, &depStructLen);
        
        uint_least32_t crc32 = 0;
        size_t depLen = 0;
        char *depValue = this->get(depStruct->dependency, &depLen);
        if (depLen == 0) {
            crc32 = Crc32Empty();
        } else {
            crc32 = Crc32((const unsigned char *)depValue, depLen);
        }
        if (crc32 != depStruct->crc32) {
            delete value;
            *newValue_len = 0;
            return NULL;
        } else {
            *newValue_len = value_len - depStructLen;
            if (*newValue_len <= 0) {
                return NULL;
            }
            char *newValue = new char [*newValue_len];
            memcpy(newValue, value+depStructLen, *newValue_len);
            delete value;
            return newValue;
        }
    } else {
        *newValue_len = value_len;
        return value;
    }
}
