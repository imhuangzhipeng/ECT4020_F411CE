#include "key_device.h"
#include <stddef.h>

extern struct Key_Device g_user_key;

static struct Key_Device *__key_device_set[KEY_TYPE_NUM] = {
    &g_user_key,
};

struct Key_Device *getKeyDevice(key_type_t type)
{
    int i = 0;
    for (i = 0; i < KEY_TYPE_NUM; i++)
    {
        if (type == __key_device_set[i]->type)
            return __key_device_set[i];
    }

    return NULL;
}
