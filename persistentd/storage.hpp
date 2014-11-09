#ifndef Storage_HPP
#define Storage_HPP

//#include <ext/hash_set> //deprecated to usorted_map in c++11, using for older compilators
#include <map>
#include <string>

class Storage {

    public:
        Storage();
        std::string get(std::string key);
        bool set(std::string key, std::string value);
        bool del(std::string key);
    private:
//        __gnu_cxx::hash_set<std::string, __gnu_cxx::hash<std::string> > bucket;
        //temporary:
        std::map<std::string, std::string> bucket;

};

#endif
