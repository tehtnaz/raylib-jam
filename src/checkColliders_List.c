#include "checkColliders_Float.h"
#include "checkColliders_List.h"
#include "wallPattern.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "colliderType.h"

CollisionInfo CheckAllCollisions(CollisionInfo info, int colliderNum, Rectangle* walls, Rectangle self){
    CollisionInfo collision = info;
    int result = 0;
    for(int i = 0; i < colliderNum; i++){
        //printf("check\n");
        result = f_checkCollider(walls[i], self, false, true);
        switch(result){
            case 1:
                collision.left = true;
                break;
            case 2:
                collision.right = true;
                break;
            case 3:
                collision.up = true;
                break;
            case 4:
                collision.down = true;
                collision.floor = walls[i].y;
                break;
        }
        if(result != 0){
            printf("hit\n");
        }
    }
    return collision;
}
CollisionInfo CheckAllCollisionsNew(int colliderNum, Rectangle* walls, Rectangle self){
    CollisionInfo collision;
    collision.left = false;
    collision.right = false;
    collision.up = false;
    collision.down = false;
    collision.trigger = 0;
    return CheckAllCollisions(collision, colliderNum, walls, self);
}


CollisionInfo CheckColliderColInfo(CollisionInfo info, Rectangle box, Rectangle self, int trigger){
    CollisionInfo collision = info;
    int result = f_checkCollider(box, self, trigger, true);
    switch(result){
            case 1:
                collision.left = true;
                break;
            case 2:
                collision.right = true;
                break;
            case 3:
                collision.up = true;
                break;
            case 4:
                collision.down = true;
                collision.floor = box.y;
                break;
            case 5:
                collision.trigger = trigger;
                break;
        }
    return collision;
}


CollisionInfo CheckAllCollisionsList(CollisionInfo info, WallPattern* root, Rectangle self){
    CollisionInfo collision = info;
    int result = 0;
    WallPattern* select = root;
    while(select != NULL){
        result = f_checkCollider(select->rec, self, select->trigger, true);
        switch(result){
            case 1:
                collision.left = true;
                break;
            case 2:
                collision.right = true;
                break;
            case 3:
                collision.up = true;
                break;
            case 4:
                collision.down = true;
                collision.floor = select->rec.y;
                break;
            case 5:
                collision.triggerObjID = (uint64_t)select;
                collision.trigger = select->trigger;
                break;
        }
        /*if(result != 0){
            printf("hit\n");
        }*/
        select = select->next;
    }
    return collision;
}
CollisionInfo CheckAllCollisionsListNew(WallPattern* root, Rectangle self){
    CollisionInfo collision;
    collision.left = false;
    collision.right = false;
    collision.up = false;
    collision.down = false;
    collision.trigger = 0;
    return CheckAllCollisionsList(collision, root, self);
}