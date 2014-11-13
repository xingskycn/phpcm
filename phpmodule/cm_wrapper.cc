#include "php_cm.hpp"
#include "cm_wrapper.hpp"
#include "cm_debug.hpp"

//object manage
CmWrapper::CmWrapper(Cm* cmOld, Cm* cmNew, bool debug)
{
    this->debug = debug;
    this->cmOld = cmOld;
    this->cmNew = cmNew;
    for (unsigned int i=0; i<=255; i++) {
        if (cmOld->getBackendString(i).compare(cmNew->getBackendString(i)) == 0) {
            this->useBoth[(unsigned char)i] = false;
        } else {
            this->useBoth[(unsigned char)i] = true;
        }
    }
}

CmWrapper::~CmWrapper()
{
    delete this->cmOld;
    delete this->cmNew;
}

//proxyed methods
bool CmWrapper::set(char *key, char *value, int value_len, char *dependency, long expire)
{
    if (this->useBoth[this->getShard(key)]) {
        IFDEBUG_A
            INFO("PHPCM: ReSharding: `set` key %s to both configurations", key);
        IFDEBUG_E
        bool summary;
        summary = this->cmOld->set(key, value, value_len, dependency, expire);
        return summary && this->cmNew->set(key, value, value_len, dependency, expire);
    } else {
        return this->cmOld->set(key, value, value_len, dependency, expire);
    }
}

char* CmWrapper::get(char *key, size_t *return_value_length)
{
    return this->cmOld->get(key, return_value_length);
}

std::map<std::string, RVal> CmWrapper::mget(std::vector<std::string> keys)
{
    return this->cmOld->mget(keys);
}

bool CmWrapper::remove(char *key)
{
    if (this->useBoth[this->getShard(key)]) {
        IFDEBUG_A
            INFO("PHPCM: ReSharding: `remove` key %s for both configurations", key);
        IFDEBUG_E

        bool summary;
        summary = this->cmOld->remove(key);
        return summary && this->cmNew->remove(key);
    } else {
        return this->cmOld->remove(key);
    }
}

bool CmWrapper::add(char *key, char *value, int value_len, char *dependency, long expire)
{
    if (this->useBoth[this->getShard(key)]) {
        IFDEBUG_A
            INFO("PHPCM: ReSharding: `add` key %s to both configurations", key);
        IFDEBUG_E

        bool summary;
        summary = this->cmOld->add(key, value, value_len, dependency, expire);
        return summary && this->cmNew->add(key, value, value_len, dependency, expire);
    } else {
        return this->cmOld->add(key, value, value_len, dependency, expire);
    }
}

bool CmWrapper::flush()
{
    bool summary = this->cmOld->flush();
    return summary && this->cmNew->flush();
}

bool CmWrapper::flush(long shardId)
{
    zend_error(E_WARNING, "Cant flush on ReSharding");
    return false;
}
