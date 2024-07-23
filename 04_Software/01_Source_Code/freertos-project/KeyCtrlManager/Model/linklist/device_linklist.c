#include "device_linklist.h"

void addDevLinkList(pDeviceNode *list, DeviceNode *item)
{
    if (!list)
    {
        return;
    }

    item->pnext = *list;
    *list = item;
}
