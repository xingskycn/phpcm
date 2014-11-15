#ifndef CM_CM_WRAPPER_H
#define CM_CM_WRAPPER_H

#include <string>
#include <map>
#include <vector>

#include "cm_interface.hpp"
#include "cm_class.hpp"
#include "cm_RVal.hpp"

class CmWrapper : public CmInterface {
    public:
        CmWrapper(Cm* cmOld, Cm* cmNew, bool debug);
        ~CmWrapper();

        //proxyed methods
        virtual bool set(char *key, char *value, int value_len, char *dependency, long expire);
        virtual char* get(char *key, size_t *return_value_length);
        virtual std::map<std::string, RVal> mget(std::vector<std::string> keys);
        virtual bool remove(char *key);
        virtual bool add(char *key, char *value, int value_len, char *dependency, long expire);
        virtual bool flush();
        virtual bool flush(long shardId);
    private:
        Cm* cmOld;
        Cm* cmNew;
        std::map<unsigned char, bool> useBoth;
        bool debug;
};

#endif /* CM_CM_WRAPPER_H */
