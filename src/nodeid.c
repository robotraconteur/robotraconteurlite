#include "robotraconteurlite/nodeid.h"
#include <uuid/uuid.h>
#include <string.h>

int robotraconteurlite_nodeid_equal(const struct robotraconteurlite_nodeid *a, const struct robotraconteurlite_nodeid *b)
{
    return memcmp(a->data, b->data, sizeof(a->data)) == 0;
}

int robotraconteurlite_nodeid_isany(const struct robotraconteurlite_nodeid *a)
{
    return memcmp(a->data, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01", sizeof(a->data)) == 0;
}

int robotraconteurlite_nodeid_newrandom(struct robotraconteurlite_nodeid *a)
{
    /* Use uuid.h to generate a random UUID */
    uuid_t uuid;
    uuid_generate_random(uuid);
    memcpy(a->data, uuid, sizeof(a->data));
    return 0;
}

int robotraconteurlite_nodeid_reset(struct robotraconteurlite_nodeid *a)
{
    memset(a->data, 0, sizeof(a->data));
    return 0;
}

int robotraconteurlite_nodeid_copy_to(const struct robotraconteurlite_nodeid *src, struct robotraconteurlite_nodeid *dst)
{
    memcpy(dst->data, src->data, sizeof(dst->data));
    return 0;
}
