#include "key_device.h"
#include <stddef.h>

extern KeyDevice g_user_key;

static KeyDevice *__g_key_device_set[KEY_NUM_MAX] = {
    &g_user_key,
};

pKeyDevice getKeyDevice(KEY_TYPE key_type)
{
    int i = 0;
    for (i = 0; i < KEY_NUM_MAX; i++)
    {
        if (key_type == __g_key_device_set[i]->key_type)
            return __g_key_device_set[i];
    }

    return NULL;
}
