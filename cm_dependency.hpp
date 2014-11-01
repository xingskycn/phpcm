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
	return false;
    }
    const char header[] = CmDependency_header;
    if (memcmp(raw, &header, strlen(header)) != 0) {
	return false;
    }
    int* dependencyLenPosition = (int *)((long)raw+sizeof(CmDependency_header)+sizeof(uint_least32_t));
    int dependencyLen = *dependencyLenPosition;
    char* TerminateZeroPosition = (char*)(dependencyLenPosition+(long)dependencyLen);
    if ((long)TerminateZeroPosition > (long)raw+(long)raw_len) {
	return false;
    }
    char TerminateZero = *TerminateZeroPosition;
    if (TerminateZero != 0) {
	return false;
    }
    return true;
}

static CmDependency* makeCmDependencyFromName(char* name, uint_least32_t crc32, size_t *outLength)
{
    (*outLength) = sizeof(CmDependency) + strlen(name); //+1-1=0
    CmDependency *dep = (CmDependency*)malloc((size_t) (*outLength));
    strcpy(((char*)dep->prefix), CmDependency_header);
    dep->crc32 = crc32;
    dep->dependencyLen = strlen(name)+1;
    strcpy((dep->dependency), name);
    return dep;
}

#endif