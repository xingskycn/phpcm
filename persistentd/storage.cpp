#include "storage.hpp"

/*
* dump format: "key\0", "value\0" "\n" next...
*/
Storage::Storage()
{
    //skip
}

std::string Storage::get(std::string key)
{
    if (bucket.find(key) != bucket.end()) {
        return bucket[key];
    } else {
        return std::string("");
    }
}

bool Storage::set(std::string key, std::string value)
{
    bucket[key] = value;
    return true;
}

bool Storage::del(std::string key)
{
    if (bucket.find(key) != bucket.end()) {
        bucket.erase(key);
        return true;
    } else {
        return false;
    }
}
