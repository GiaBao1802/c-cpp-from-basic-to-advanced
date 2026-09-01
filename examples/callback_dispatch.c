#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef int (*Handler)(void *context, const uint8_t *data, size_t length);

typedef struct {
    uint8_t id;
    Handler handler;
    void *context;
} Route;

static int count_bytes(void *context, const uint8_t *data, size_t length)
{
    size_t *count = context;
    if ((count == NULL) || ((data == NULL) && (length != 0u))) return -1;
    *count += length;
    return 0;
}

static int dispatch(const Route *routes, size_t count, uint8_t id,
                    const uint8_t *data, size_t length)
{
    if (routes == NULL) return -1;
    for (size_t i = 0u; i < count; ++i) {
        if (routes[i].id == id && routes[i].handler != NULL)
            return routes[i].handler(routes[i].context, data, length);
    }
    return -2;
}

int main(void)
{
    size_t received = 0u;
    const uint8_t payload[3] = {1u, 2u, 3u};
    Route routes[] = {{0x22u, count_bytes, &received}};
    assert(dispatch(routes, 1u, 0x22u, payload, 3u) == 0);
    assert(received == 3u);
    assert(dispatch(routes, 1u, 0x2Eu, payload, 3u) == -2);
    return 0;
}
