#include "include/raylib.h"
#include "include/rlgl.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "recpp.h"
#include "animation.h"
#include "vector2pp.h"
#ifdef __EMSCRIPTEN__
    #include "include/emscripten.h"
#endif

#include "wallPattern.h"
#include "checkColliders_Float.h"
#include "checkColliders_List.h"

Rectangle patterns[4] = {
    {900,0,50,400},
    {900,100,50,200},
    {900,400,50,100},
    {900,50,50,100}
};



float velocity = 0.0f;
const float gravity = 9.81f * 2.5f;
const float jumpHeight = -15.0f;
Rectangle player = {100, 50, 50, 50};
Rectangle ground = {0, 164*3, 900, 100};
Texture2D glu;
Rectangle* walls;
int wallNum = 0;
float wallSpeed = -250.0f;
Animation playerAnim;
Animation playerAnim_flip;
float playerSpeed = 300.0f;
Vector2 startingPos = {100, 50};
Texture2D frame;
Animation background;
WallPattern* basic;
WallPattern* patternList[4];
WallPattern** wallsList;



void NewWall(){
    wallNum++;
    
    wallsList = realloc(wallsList, wallNum * sizeof(WallPattern*));
    wallsList[wallNum-1] = CopyWallPattern(patternList[GetRandomValue(0, 3)]);
    
    
    //walls = realloc(walls, wallNum * sizeof(Rectangle));
    //walls[wallNum-1] = patterns[GetRandomValue(0, 3)];
    

    //printf("Made New Wall, new length: %d\n", wallNum);
}
void KillWall(){
    FreeWallPattern(wallsList[0]);
    for(int i = 0; i < wallNum - 1; i++){
        wallsList[i] = wallsList[i+1];
        //walls[i] = walls[i+1];
    }
    wallNum--;
}



void CalculateCollisions(){
    for(int i = 0; i < wallNum; i++){
        //walls[i].x += wallSpeed * GetFrameTime();
        MoveWallPattern(wallsList[i], (Vector2){wallSpeed * GetFrameTime(), 0});
    }
    //if(walls[0].x < -100 && wallNum > 0){
    bool done = false;
    while(!done){
        if(wallsList[0]->rec.x < -100 && wallNum > 0){
            KillWall();
        }else{
            done = true;
        }
    }

    
    //CollisionInfo collision = CheckAllCollisionsNew(wallNum, walls, player);
    printf(" ");
    CollisionInfo collision = {0,0,0,0,0,0};
    for(int i = 0; i < wallNum; i++){
        collision = CheckAllCollisionsList(collision, wallsList[i], player);
    }
    collision = CheckColliderColInfo(collision, ground, player);

    if(collision.right){
        player.x += wallSpeed * GetFrameTime();
    }

    if(IsKeyDown(KEY_A)){
        if(!collision.left){
            player.x -= playerSpeed * GetFrameTime();
        }
    }
    if(IsKeyDown(KEY_D)){
        if(!collision.right){
            player.x += playerSpeed * GetFrameTime();
        }
    }
    if(collision.up && velocity < 0){
        velocity = 0;
    }
    if(collision.down && velocity >= 0){
        player.y = collision.floor - player.height;
        velocity = 0;
        if(IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W)){
            velocity += jumpHeight;
        }
    }else{
        velocity += gravity * GetFrameTime();
        player.y += velocity;
    }
    
    //printf("are you sure?\n");
    if(collision.trigger == 1){
        player.x = startingPos.x;
        player.y = startingPos.y;
    }
    //printf("no papa\n");
}

void DebugVisuals(){
    DrawRectangleRec(player, RED);
    //DrawTextureEx(playerAnim.texture, addVec2(recToVec(player), (Vector2){100,0}), 0, 3, WHITE);
    DrawTextureEx(playerAnim_flip.texture, addVec2(recToVec(player), (Vector2){100,0}), 0, 3, WHITE);
}

void DrawWallPattern(WallPattern* root){
    WallPattern* select = root;
    while(select != NULL){
        DrawRectangleRec(select->rec, (Color){130-(select->rec.x * 130 / 900),130-(select->rec.x * 130 / 900),130-(select->rec.x * 130 / 900),255});
        select = select->next;
    }
}

void GameVisuals(){
    DrawAnimationPro(&background, (Vector2){0,0}, 3, WHITE, CYCLE_FORWARD);
    DrawTextureEx(frame, (Vector2){0,0}, 0, 3, WHITE);
    //DrawTextureEx(glu, recToVec(player), 0, 3, WHITE);
    DrawAnimationPro(IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) ? &playerAnim_flip : &playerAnim, recToVec(player), 3, WHITE, CYCLE_FORWARD);
    for(int i = 0; i < wallNum; i++){
        DrawWallPattern(wallsList[i]);
        //DrawRectangleRec(walls[i], (Color){130-(walls[i].x * 130 / 900),130-(walls[i].x * 130 / 900),130-(walls[i].x * 130 / 900),255});
    }
}

void UpdateDrawFrame(){
    //printf("game is running?\n");
    if(IsKeyPressed(KEY_F)){
        //for(int i = 0; i < 1000; i ++){
        NewWall();
        //}
    }
    //printf("yes papa\n");
    CalculateCollisions();
    //printf("collision were calculated?\nyes papa\n");
    BeginDrawing();
        ClearBackground(RAYWHITE);
        //DebugVisuals();
        GameVisuals();
    EndDrawing();
}

int main(void){
    SetRandomSeed(1);
    InitWindow(900, 506, "Runner");
    SetTargetFPS(60);
    rlDisableBackfaceCulling();
    frame = LoadTexture("resources/frame.png");
    glu = LoadTexture("resources/glu.png");
    player.width = glu.width * 3;
    player.height = glu.height * 3;

    playerAnim = assignProperties(19, 0, 5, true, 4, true);
    playerAnim.texture = LoadTexture("resources/glu_anim.png");

    playerAnim_flip = assignProperties(19, 0, 5, true, 4, true);
    playerAnim_flip.texture = LoadTexture("resources/glu_anim.png");
    playerAnim_flip = flipAnimationHorizontal(playerAnim_flip);

    background = assignProperties(300, 0, 10, true, 4, true);
    background.texture = LoadTexture("resources/background.png");

    patternList[0] = NewRecPattern(newRec(900,0,50,400), 0);
    patternList[1] = NewRecPattern(newRec(900,100,50,200), 0);
    patternList[2] = NewRecPattern(newRec(900,400,50,100), 0);
    patternList[3] = NewRecPattern(newRec(900,50,50,100), 0);


    basic = NewRecPattern(newRec(10, 20, 30, 30), 0);
    basic->next = NewRecPattern(newRec(15, 25, 35, 35), 0);
    AddRecPattern(basic, newRec(22, 20, 30, 30), 0);
    PrintWallPattern(basic);
    MoveWallPattern(basic, (Vector2){10, 20});
    PrintWallPattern(basic);
    WallPattern* copy = CopyWallPattern(basic);
    puts("%%%%%%%%");
    PrintWallPattern(basic);
    PrintWallPattern(copy);
    puts("%%%%%%%%");
    FreeWallPattern(basic);
    printf("!\n");
    PrintWallPattern(basic);
    PrintWallPattern(copy);
    printf("Note: The game crashes if we don't print to console lol");

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