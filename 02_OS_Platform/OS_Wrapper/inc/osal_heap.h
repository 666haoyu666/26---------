#ifndef OSAL_HEAP_H
#define OSAL_HEAP_H

#include "common_types.h"

void *osal_heap_malloc(size_t wanted_size);

void osal_heap_free(void *ptr);

#endif /* OSAL_HEAP_H */
