#ifndef CM_CM_INTERFACE_H
#define CM_CM_INTERFACE_H

#include <string>
#include <map>
#include <vector>

#include "cm_RVal.hpp"

class CmInterface {
    public:
        virtual bool set(char *key, char *value, int value_len, char *dependency, long expire) =0;
        virtual char* get(char *key, size_t *return_value_length) =0;
        virtual std::map<std::string, RVal> mget(std::vector<std::string> keys) =0;
        virtual bool remove(char *key) =0;
        virtual bool add(char *key, char *value, int value_len, char *dependency, long expire) =0;
        virtual bool flush() =0;
        virtual bool flush(long shardId) =0;

        unsigned char getShard(char *key);
};

#endif /* CM_CM_INTERFACE_H */
