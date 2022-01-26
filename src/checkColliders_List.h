#ifndef CHECKCOLLIDERS_LIST_H
    #define CHECKCOLLIDERS_LIST_H
    #include <stdbool.h>
    #include "wallPattern.h"

    typedef struct CollisionInfo{
        bool left;
        bool right;
        bool up;
        bool down;
        int trigger;

        float floor;

        
    }CollisionInfo;

    CollisionInfo CheckAllCollisions(CollisionInfo info, int colliderNum, Rectangle* walls, Rectangle self);
    CollisionInfo CheckAllCollisionsNew(int colliderNum, Rectangle* walls, Rectangle self);
    CollisionInfo CheckColliderColInfo(CollisionInfo info, Rectangle box, Rectangle self);

    CollisionInfo CheckAllCollisionsList(CollisionInfo info, WallPattern* root, Rectangle self);
    CollisionInfo CheckAllCollisionsListNew(WallPattern* root, Rectangle self);

#endif //CHECKCOLLIDERS_LIST_H
