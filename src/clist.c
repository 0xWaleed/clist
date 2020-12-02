#include <memory.h>
#include "clist.h"

allocator_t g_allocator = NULL;

clist_s* clist_create(size_t capacity)
{
    allocator_t allocator = clist_allocator_current();

    clist_s* list = allocator(sizeof(clist_s));

    if (list == NULL)
    {
        return NULL;
    }

    memset(list, 0, sizeof(clist_s));

    list->capacity = capacity * 2;
    size_t finalSize = list->capacity * sizeof(void*);
    list->values = allocator(finalSize);

    memset(list->values, 0, finalSize);

    return list;
}

void clist_allocator_restore()
{
    g_allocator = &clist_allocator_default;
}

void* clist_allocator_default(size_t size)
{
    return malloc(size);
}

void clist_allocator_register(allocator_t allocator)
{
    g_allocator = allocator;
}

allocator_t clist_allocator_current()
{
    if (!g_allocator)
    {
        return &clist_allocator_default;
    }
    return g_allocator;
}

void clist_add(clist_s* clist, void* element)
{
    if (clist->size >= clist->capacity)
    {
        size_t newSize = clist->capacity + BLOCK_SIZE;
        clist->values = realloc(clist->values, sizeof(NULL) * newSize);
        clist->capacity = newSize;
    }
    clist->values[clist->size++] = element;
}

int clist_find(clist_s* clist, predicate_t predicate)
{
    for (int i = 0; i < clist->size; ++i)
    {
        if (predicate(clist->values[i]))
        {
            return i;
        }
    }
    return -1;
}

void clist_clear(clist_s* clist)
{
    for (int i = 0; i < clist->size; ++i)
    {
        clist->values[i] = NULL;
    }
    clist->size = 0;
}

void clist_destroy(clist_s** clist)
{
    clist_s* list = *clist;

    if (list == NULL)
    {
        return;
    }

    list->size = 0;
    list->capacity = 0;

    if (list->values != NULL)
    {
        free(list->values);
        list->values = NULL;
    }

    free(list);
    *clist = NULL;
}


