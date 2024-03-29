#include <iostream>
#include <string>
#include <sstream>
#include <string.h>


#include "cm_class.hpp"
#include "crc32.h"
#include "cm_dependency.hpp"
#include "cm_debug.hpp"

//объявлено ниже
CmAdapter* newServerPairAdapter(ServerPair config, bool debug);
std::vector<CmAdapter*> newServerPairReplica(std::vector<ServerPair>, bool debug);

/*
* Object manage
*/

Cm::Cm(std::vector<ServerPair> config, bool debug) {
    this->debug = debug;
    unsigned int j=0;
    for (unsigned int i=0; i<=255; i++) {
	if (!config[j].isReplica) { //single node
	    std::string hp(config[j].serverName);
	    std::stringstream ss;
	    ss << hp << ":" << config[j].port;
	    ss >> hp;
	    CmAdapter* backend;
	    if (realBackends.find(hp) == realBackends.end()) {
		backend = newServerPairAdapter(config[j], debug);
		std::vector<CmAdapter*> single;
		single.push_back(backend);
		shards.push_back(single);
		realBackends[hp] = backend;
	    } else {
	        backend = realBackends[hp];
	    }
	    std::vector<CmAdapter*> one;
	    one.push_back(backend);
	    backends[(unsigned char)i] = one;
	    backendStrings[(unsigned char)i] = hp;
	} else {
	    std::stringstream ss;
	    for (unsigned int k=0; k<config[j].replica.size(); k++) {
		ss << config[j].replica[k].serverName << ":" << config[j].replica[k].port << "|";
	    }
	    std::string hp;
	    ss >> hp;
	    std::vector<CmAdapter*> repl;
	    if (realBackendsReplicas.find(hp) == realBackendsReplicas.end()) {
		repl = newServerPairReplica(config[j].replica, debug);
		shards.push_back(repl);
		realBackendsReplicas[hp] = repl;
	    } else {
		repl = realBackendsReplicas[hp];
	    }
	    backends[(unsigned char)i] = repl;
	    backendStrings[(unsigned char)i] = hp;
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

std::string Cm::getBackendString(unsigned char i)
{
    return backendStrings[i];
}

/*
* Public Interface
*/

bool Cm::set(char *key, char *value, int value_len, char *dependency, long expire)
{
    bool summary=true;
    int j = backends[this->getShard(key)].size();
    for (int i=0; i<j; i++) {
        int newValue_len=0;
        char* newValue = this->processSetDependency(value, value_len, dependency, &newValue_len);
        summary = summary && backends[this->getShard(key)][i]->set(key, newValue, newValue_len, expire);
    }
    return summary;
}

bool Cm::add(char *key, char *value, int value_len, char *dependency, long expire)
{
    bool summary=true;
    int j = backends[this->getShard(key)].size();
    for (int i=0; i<j; i++) {
        int newValue_len=0;
        char* newValue = this->processSetDependency(value, value_len, dependency, &newValue_len);
        summary = summary && backends[this->getShard(key)][i]->add(key, newValue, newValue_len, expire);
    }
    return summary;
}

std::map<std::string, RVal> Cm::mget(std::vector<std::string> keys)
{
    std::map<std::string, RVal> ret;
    for (unsigned int i=0; i<keys.size(); i++) {
        size_t value_length;
        char *value = get((char *)keys[i].c_str(), &value_length);
        if (value != NULL) {
            RVal rval = { .value = value, .length = value_length };
            ret[keys[i]] = rval;
        }
    }
    return ret;
}


char* Cm::get(char *key, size_t *return_value_length)
{
    int j = backends[this->getShard(key)].size();
    for (int i=0; i<j; i++) {
        char* result = backends[this->getShard(key)][i]->get(key, return_value_length);
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
    int j = backends[this->getShard(key)].size();
    for (int i=0; i<j; i++) {
        summary = summary && backends[this->getShard(key)][i]->remove(key);
    }
    return summary;
}

bool Cm::flush(long shardId)
{
    std::vector<CmAdapter*> adapters = this->getShardVector(shardId);
    bool summary=true;
    bool been=false;
    for (int i=0; i<adapters.size(); i++) {
        summary = summary && adapters[i]->flush();
        been=true;
    }
    return summary && been;
}

//flush all backends
bool Cm::flush()
{
    bool summary = true;
    for (std::map<std::string, CmAdapter*>::iterator it = realBackends.begin(); it != realBackends.end(); ++it)
    {
	summary = summary && it->second->flush();
    }
    for (std::map<std::string, std::vector<CmAdapter*> >::iterator it = realBackendsReplicas.begin(); it != realBackendsReplicas.end(); ++it)
    {
	for (unsigned int i=0; i<it->second.size(); i++) {
	    summary = summary && it->second[i]->flush();
	}
    }
    return summary;
}

/*
* Internal/private
*/

char* Cm::processSetDependency(char *value, int value_len, char *dependency, int *newValue_len)
{
    if (strlen(dependency) == 0) {
        IFDEBUG_A
            INFO("PHPCM: processing setDependency: key=%s", "no dependency key, skip");
        IFDEBUG_E
        (*newValue_len) = value_len;
        return value;
    }
    IFDEBUG_A
        INFO("PHPCM: processing setDependency: key=%s", dependency);
    IFDEBUG_E
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
        IFDEBUG_A
            INFO("PHPCM: processing getDependency: DEP-header=%s", "valid, processing");
        IFDEBUG_E

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
        IFDEBUG_A
            INFO("PHPCM: processing getDependency: DEP-header=%s", "INvalid, returning raw");
        IFDEBUG_E
        *newValue_len = value_len;
        return value;
    }
}

std::vector<CmAdapter*> Cm::getShardVector(long shardId)
{
    if (shardId >= shards.size()) {
        IFDEBUG_A
            std::string hp;
            std::stringstream ss;
            ss << "Id=" << shardId << ":not_found";
            ss >> hp;
            WARNING("PHPCM: flush by shardId: %s", hp.c_str());
        IFDEBUG_E

        std::vector<CmAdapter*> empty;
        return empty;
    }
    return shards[shardId];
}

CmAdapter* newServerPairAdapter(ServerPair config, bool debug)
{
    CmAdapter* backend = new CmAdapter(config.serverName, config.port, config.stable, debug);
    return backend;
}

std::vector<CmAdapter*> newServerPairReplica(std::vector<ServerPair> list, bool debug)
{
    std::vector<CmAdapter*> replica;
    for (unsigned int i=0; i<list.size(); i++) {
	replica.push_back(newServerPairAdapter(list[i], debug));
    }
    return replica;
}

