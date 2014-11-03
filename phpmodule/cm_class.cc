#include <iostream>
#include <string>
#include <sstream>
#include <string.h>


#include "cm_class.hpp"
#include "crc32.h"
#include "cm_dependency.hpp"

//объявлено ниже
CmAdapter* newServerPairAdapter(ServerPair config);
std::vector<CmAdapter*> newServerPairReplica(std::vector<ServerPair>);

/*
* Object manage
*/

Cm::Cm(std::vector<ServerPair> config) {
    unsigned int j=0;
    for (unsigned int i=0; i<=255; i++) {
	if (!config[j].isReplica) { //single node
	    std::string hp(config[j].serverName);
	    std::stringstream ss;
	    ss << hp << ":" << config[j].port;
	    ss >> hp;
	    CmAdapter* backend;
	    if (realBackends.find(hp) == realBackends.end()) {
		backend = newServerPairAdapter(config[j]);
		realBackends[hp] = backend;
	    } else {
	        backend = realBackends[hp];
	    }
	    std::vector<CmAdapter*> one;
	    one.push_back(backend);
	    backends[(unsigned char)i] = one;
	} else {
	    std::stringstream ss;
	    for (unsigned int k=0; k<config[j].replica.size(); k++) {
		ss << config[j].replica[k].serverName << ":" << config[j].replica[k].port << "|";
	    }
	    std::string hp;
	    ss >> hp;
	    std::vector<CmAdapter*> repl;
	    if (realBackendsReplicas.find(hp) == realBackendsReplicas.end()) {
		repl = newServerPairReplica(config[j].replica);
	    } else {
		repl = realBackendsReplicas[hp];
	    }
	    backends[(unsigned char)i] = repl;
	}
	j++;
	if (j==config.size()) j=0;
    }
}

Cm::~Cm() {
    for (unsigned int i=0; i<=255; i++) {
	backends[(unsigned char)i].clear();
    }
    backends.clear();
    for (std::map<std::string, CmAdapter*>::iterator it = realBackends.begin(); it != realBackends.end(); ++it)
    {
	delete it->second;
    }
    realBackends.clear();
    for (std::map<std::string, std::vector<CmAdapter*> >::iterator it = realBackendsReplicas.begin(); it != realBackendsReplicas.end(); ++it)
    {
	for (unsigned int i=0; i<it->second.size(); i++) {
	    delete it->second[i];
	}
	it->second.clear();
    }
    realBackendsReplicas.clear();
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

CmAdapter* newServerPairAdapter(ServerPair config)
{
    CmAdapter* backend = new CmAdapter(config.serverName, config.port, config.stable);
    return backend;
}

std::vector<CmAdapter*> newServerPairReplica(std::vector<ServerPair> list)
{
    std::vector<CmAdapter*> replica;
    for (unsigned int i=0; i<list.size(); i++) {
	replica.push_back(newServerPairAdapter(list[i]));
    }
    return replica;
}
