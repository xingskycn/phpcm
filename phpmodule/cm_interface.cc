#include <string.h>

#include "cm_interface.hpp"

unsigned char CmInterface::getShard(char *key)
{
    return key[strlen(key)-1];
}
