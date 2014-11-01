#ifndef CM_CM_DEPENDENCY_H
#define CM_CM_DEPENDENCY_H

#define CmDependency_header "DEP"

typedef struct {
    const char prefix[4];
    uint_least32_t crc32;
    unsigned int dependencyLen;
    char dependency[1];
} CmDependency;

static CmDependency* makeCmDependencyFromRaw(void* raw, size_t *outLength)
{
    int* dependencyLenPosition = (int*)((long)raw+sizeof(CmDependency_header)+sizeof(uint_least32_t));
    int dependencyLen = *dependencyLenPosition;
    *outLength = sizeof(CmDependency) + dependencyLen - 1;
    return (CmDependency*)raw;
}

static bool validateCmDependencyInRaw(void* raw, int raw_len)
{
    if (raw_len < sizeof(CmDependency)) {
//	std::cout << "not valid-1" << std::endl;
	return false;
    }
    const char header[] = CmDependency_header;
    if (memcmp(raw, &header, strlen(header)) != 0) {
//	std::cout << "not valid-2" << std::endl;
	return false;
    }
//    int* dependencyLenPosition = (int *)((long)raw+4+sizeof(uint_least32_t));
//    int dependencyLen = *dependencyLenPosition;
//    char* TerminateZeroPosition = (char*)(dependencyLenPosition+sizeof(unsigned int)+(long)dependencyLen);
//    std::cout << "Positions: " << (long)TerminateZeroPosition << "	" << (long)raw+(long)raw_len << std::endl;
//    if ((long)TerminateZeroPosition > (long)raw+(long)raw_len) {
//	std::cout << "not valid-3" << std::endl;
//	return false;
//    }
//    char TerminateZero = *TerminateZeroPosition;
//    if (TerminateZero != 0) {
//	std::cout << "not valid-4" << std::endl;
//	std::cout << "dependency= " << (char*)((long)raw+4) << std::endl;
//	return false;
//    }
    return true;
}

static CmDependency* makeCmDependencyFromName(char* name, uint_least32_t crc32, size_t *outLength)
{
//    std::cout << "sizeof(CmDependency) = " << sizeof(CmDependency) << " + strlen(name) = " << strlen(name) << std::endl;
    (*outLength) = sizeof(CmDependency) + strlen(name); //+1-1=0
    CmDependency *dep = (CmDependency*)malloc((size_t) (*outLength));
    strcpy(((char*)dep->prefix), CmDependency_header);
    dep->crc32 = crc32;
    dep->dependencyLen = strlen(name) + 1;
    strcpy((char*)dep->dependency, name);
//    std::cout << "maked from name: " << (char*)dep->dependency << std::endl;
    return dep;
}

#endif
