#pragma once

#include <dlfcn.h>

void *DlopenExt(const char *path, int flags);

void *DlopenMem(const char *memfdPath, int memfd, int flags);
