#include "robotraconteurlite/nodeid.h"
#include <uuid/uuid.h>
#include <string.h>

int robotraconteurlite_nodeid_equal(const struct robotraconteurlite_nodeid* a,
                                    const struct robotraconteurlite_nodeid* b)
{
    return memcmp(a->data, b->data, sizeof(a->data)) == 0;
}

int robotraconteurlite_nodeid_isany(const struct robotraconteurlite_nodeid* a)
{
    uint8_t zero_uuid[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return memcmp(a->data, zero_uuid, sizeof(a->data)) == 0;
}

int robotraconteurlite_nodeid_newrandom(struct robotraconteurlite_nodeid* a)
{
    /* Use uuid.h to generate a random UUID */
    uuid_t uuid;
    uuid_generate_random(uuid);
    (void)memcpy(a->data, uuid, sizeof(a->data));
    return 0;
}

int robotraconteurlite_nodeid_reset(struct robotraconteurlite_nodeid* a)
{
    (void)memset(a->data, 0, sizeof(a->data));
    return 0;
}

int robotraconteurlite_nodeid_copy_to(const struct robotraconteurlite_nodeid* src,
                                      struct robotraconteurlite_nodeid* dst)
{
    (void)memcpy(dst->data, src->data, sizeof(dst->data));
    return 0;
}
