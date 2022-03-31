#include "collision.h"

bool collision(AABB r1, AABB r2) {
    return
        (r1.pos.x <= r2.pos.x + r2.size.x
        &&
         r2.pos.x <= r1.pos.x + r1.size.x)
        &&
        (r1.pos.y <= r2.pos.y + r2.size.y
        &&
         r2.pos.y <= r1.pos.y + r1.size.y);

}
