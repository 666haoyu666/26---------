/**
 * @file  osal_heap.c
 * @brief FreeRTOS heap wrapper.
 */

#include "osal_heap.h"

#include "osal_freertos_priv.h"

void *osal_heap_malloc(size_t wanted_size)
{
    if (wanted_size == 0U) {
        return NULL;
    }

    return pvPortMalloc(wanted_size);
}

void osal_heap_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    vPortFree(ptr);
}
