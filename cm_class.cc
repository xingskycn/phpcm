#include "cm_class.hpp"

#include <string.h>

Cm::Cm() {
    CmAdapter* adapter = new CmAdapter();
    for (unsigned int i=0; i<=255; i++) {
	std::vector<CmAdapter*> one;
	one.push_back(adapter);
	backends[(unsigned char)i] = one;
    }
}


bool Cm::set(char *key, char *value, int value_len, bool isDependency, char *dependency, long expire)
{
    return true;
}

char* get(char *key)
{
    char* r = new char [2];
    strcpy(r, "r");
    return r;
}

bool remove(char *key)
{
    return true;
}

bool expire(char *key)
{
    return true;
}

bool expireOnlyChildren(char *key)
{
    return true;
}

