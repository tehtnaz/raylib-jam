#include "include/raylib.h"
#include "include/rlgl.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "recpp.h"
#ifdef __EMSCRIPTEN__
    #include "include/emscripten.h"
#endif

#include "checkColliders_Float.h"

typedef struct CollisionInfo{
    bool left;
    bool right;
    bool up;
    bool down;
    int trigger;

    float floor;
}CollisionInfo;

float velocity = 0.0f;
const float gravity = 9.81f;
const float jumpHeight = -10.0f;
Rectangle player = (Rectangle){50, 0, 50, 50};
Rectangle ground = (Rectangle){0, 500, 900, 100};
Texture2D glu;
Rectangle* walls;
int wallNum;
float wallSpeed = -250.0f;

void NewWall(){
    wallNum++;
    walls = realloc(walls, wallNum * sizeof(Rectangle));
    
    walls[wallNum-1] = (Rectangle){400, 0, 100, GetRandomValue(10, 400)};
    walls[wallNum-1].y += GetRandomValue(0, 500 - walls[wallNum-1].height);
    printf("Made New Wall, new length: %d\n", wallNum);
}
void KillWall(){
    for(int i = 0; i < wallNum - 1; i++){
        walls[i] = walls[i+1];
    }
    wallNum--;
}

CollisionInfo CheckAllCollisions(int colliderNum, Rectangle* walls, Rectangle self){
    CollisionInfo collision;
    collision.left = false;
    collision.right = false;
    collision.up = false;
    collision.down = false;
    collision.trigger = 0;
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
CollisionInfo CheckColliderColInfo(CollisionInfo info, Rectangle box, Rectangle self){
    CollisionInfo collision = info;
    int result = f_checkCollider(box, self, false, true);
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
        }
    return collision;
}

void CalculateCollisions(){
    for(int i = 0; i < wallNum; i++){
        walls[i].x += wallSpeed * GetFrameTime();
        //printf("wallpos: %f\n", walls[i].x);
    }
    if(walls[0].x < 0 && wallNum > 0){
        KillWall();
    }

    
    CollisionInfo collision = CheckAllCollisions(wallNum, walls, player);
    collision = CheckColliderColInfo(collision, ground, player);
    for(int i = 0; i < wallNum; i++){
        //if(CheckCollisionRecs(walls[i], player)) collision.right = true;
        //collision = CheckColliderColInfo(collision, walls[i], player);
    }

    if(collision.down && velocity >= 0){
        player.y = collision.floor - player.height;
        velocity = 0;
        if(IsKeyDown(KEY_SPACE)){
            velocity += jumpHeight;
        }
    }else{
        velocity += gravity * GetFrameTime();
        player.y += velocity;
    }
    /*if(collision.right){
        printf("oisulbkdefndlksajdnfsa;ldnjfalk;jndf\n\n\n\n");
    }*/
}

void UpdateDrawFrame(){
    if(IsKeyPressed(KEY_F)){
        NewWall();
    }
    CalculateCollisions();
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(player, RED);
        DrawTextureEx(glu, recToVec(player), 0, 3, WHITE);
        DrawRectangleRec(ground, BLACK);
        for(int i = 0; i < wallNum; i++){
            DrawRectangleRec(walls[i], GRAY);
        }
    EndDrawing();
}

int main(void){
    SetRandomSeed(1);
    InitWindow(900, 506, "NewWindow");
    SetTargetFPS(60);
    rlDisableBackfaceCulling();
    glu = LoadTexture("resources/glu.png");
    player.width = glu.width * 3;
    player.height = glu.height * 3;
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        while(!WindowShouldClose()){
            UpdateDrawFrame();
        }
    #endif
    CloseWindow();
    return 0;
}