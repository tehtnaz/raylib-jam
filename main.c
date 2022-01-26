#include "include/raylib.h"
#include "include/rlgl.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#ifdef __EMSCRIPTEN__
    #include "include/emscripten.h"
#endif

#include "recpp.h"
#include "animation.h"
#include "vector2pp.h"

#include "wallPattern.h"
#include "checkColliders_Float.h"
#include "checkColliders_List.h"

typedef enum GameState{
    STATE_ACTIVE,
    STATE_DEAD,
    STATE_MENU
}GameState;


float velocity = 0.0f;
const float gravity = 9.81f * 2.5f;
const float jumpHeight = -15.0f;
const float playerSpeed = 250.0f;

const Rectangle ground = {0, 164*3, 900, 100};
const Rectangle spikes = {0,0,21,507};

Rectangle player = {100, 50, 50, 50};
Texture2D stick;
Vector2 startingPos = {100, 50};

int wallNum = 0;
float wallSpeed = -250.0f;
Animation playerAnim;
Animation playerAnim_flip;

Texture2D frame;
Animation background;
WallPattern* basic;
WallPattern* patternList[4];
WallPattern** wallsList;

float timer = 0.0f;
float resetTime = 3.0f;
const float speedUpMultiplier = 0.95f;
float speedUpTimer = 0.0f;
const float speedUpResetTime = 4.5f;

bool wallJump = false;
bool jumpDebug = false;
bool debug = false;
GameState gameState = STATE_ACTIVE;


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
        MoveWallPattern(wallsList[i], (Vector2){wallSpeed * GetFrameTime(), 0});
    }
    bool done = false;
    while(!done){
        if(wallsList[0]->rec.x < -100 && wallNum > 0){
            KillWall();
        }else{
            done = true;
        }
    }


        printf(" ");
    CollisionInfo collision = {0,0,0,0,0,0};
    for(int i = 0; i < wallNum; i++){
        collision = CheckAllCollisionsList(collision, wallsList[i], player);
    }
    collision = CheckColliderColInfo(collision, ground, player, 0);
    collision = CheckColliderColInfo(collision, spikes, player, 1);

    if(collision.right){
        player.x += wallSpeed * GetFrameTime();
    }
    if(!collision.left && !collision.right){
        wallJump = true;
    }
    if(IsKeyDown(KEY_A)){
        if(!collision.left){
            player.x -= playerSpeed * GetFrameTime();
        }else if(IsKeyPressed(KEY_W) && wallJump && jumpDebug){
            velocity += jumpHeight;
            wallJump = false;
        }
    }
    if(IsKeyDown(KEY_D)){
        if(!collision.right){
            player.x += playerSpeed * GetFrameTime();
        }else if(IsKeyPressed(KEY_W) && wallJump && jumpDebug){
            velocity += jumpHeight;
            wallJump = false;
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
        wallJump = true;
    }else{
        velocity += gravity * GetFrameTime();
        player.y += velocity;
    }
    if(collision.trigger == 1 || player.x < 0 || player.y > 900){
        gameState = STATE_DEAD;
    }
}

void DebugVisuals(){
    DrawText(TextFormat("%f", resetTime), 0,20,15, WHITE);
    //DrawRectangleRec(player, RED);
    //DrawTextureEx(playerAnim.texture, addVec2(recToVec(player), (Vector2){100,0}), 0, 3, WHITE);
    //DrawTextureEx(playerAnim_flip.texture, addVec2(recToVec(player), (Vector2){100,0}), 0, 3, WHITE);
}

void DrawWallPattern(WallPattern* root){
    WallPattern* select = root;
    while(select != NULL){
        if(select-> trigger == 1) DrawRectangleRec(select->rec, (Color){255,84-(select->rec.x * 84 / 900),98-(select->rec.x * 98 / 900),255});
        else DrawRectangleRec(select->rec, (Color){74-(select->rec.x * 74 / 900),84-(select->rec.x * 84 / 900),98-(select->rec.x * 98 / 900),255});
        select = select->next;
    }
}

void GameVisuals(){
    DrawAnimationPro(&background, (Vector2){0,0}, 3, WHITE, gameState == STATE_ACTIVE ? CYCLE_FORWARD : CYCLE_NONE);
    DrawTextureEx(frame, (Vector2){0,0}, 0, 3, WHITE);
    //DrawTextureEx(spikeTexture, (Vector2){0,0}, 0, 3, WHITE);
    if((IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && gameState == STATE_ACTIVE) DrawAnimationPro(IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) ? &playerAnim_flip : &playerAnim, recToVec(player), 3, WHITE, CYCLE_FORWARD);
    else DrawTextureEx(stick, recToVec(player), 0, 3, WHITE);
    for(int i = 0; i < wallNum; i++){
        DrawWallPattern(wallsList[i]);
        //DrawRectangleRec(walls[i], (Color){130-(walls[i].x * 130 / 900),130-(walls[i].x * 130 / 900),130-(walls[i].x * 130 / 900),255});
    }
}

void WallTimer(){
    timer -= GetFrameTime();
    speedUpTimer -= GetFrameTime();
    if(IsKeyPressed(KEY_F) || timer < 0){
        //for(int i = 0; i < 1000; i ++){
        NewWall();
        timer = resetTime;
        //}
    }
    if(IsKeyPressed(KEY_G) || speedUpTimer < 0){
        resetTime *= speedUpMultiplier;
        speedUpTimer = speedUpResetTime;
        wallSpeed = -250 * 3 / resetTime;
    }
}

void UpdateDrawFrame(){
    if(IsKeyPressed(KEY_F1)){
        jumpDebug = !jumpDebug;
    }
    if(IsKeyPressed(KEY_F2)){
        debug = !debug;
    }
    if(gameState == STATE_DEAD && IsKeyPressed(KEY_R)){
        gameState = STATE_ACTIVE;
        player.x = startingPos.x;
        player.y = startingPos.y;
        resetTime = 3.0f;
        speedUpTimer = speedUpResetTime;
        wallSpeed = -250.0f;
        while(wallNum > 0){
            KillWall();
        }
    }
    if(gameState == STATE_ACTIVE){
        if(IsKeyPressed(KEY_R) && jumpDebug) gameState = STATE_DEAD;
        WallTimer();
        CalculateCollisions();
    }
    BeginDrawing();
        ClearBackground(RAYWHITE);
        if(gameState == STATE_ACTIVE  || gameState == STATE_DEAD){
            GameVisuals();
            if(debug) DrawText(TextFormat("timer: %f\nresetTimer: %f\nspeedUpTimer: %f\n wallSpeed: %f", timer, resetTime, speedUpTimer, wallSpeed), 0, 0, 15, WHITE);
            if(jumpDebug) DrawText("Debug Mode Enabled.\nWall-Jump Enabled\nReset with R enabled", 0,0,15, WHITE);
            if(gameState == STATE_DEAD){
                DrawText("You died.", player.x + 50 < 700 ? player.x + 50 : 700, player.y > 0 ? (player.y > 467 ? 467 : player.y) : 0, 40, RED);
            }
        }
    EndDrawing();
}

int main(void){
    SetRandomSeed(1);
    InitWindow(900, 507, "Runner");
    SetTargetFPS(60);
    rlDisableBackfaceCulling();
    frame = LoadTexture("resources/frame.png");
    stick = LoadTexture("resources/stick.png");
    //spikeTexture = LoadTexture("resources/spikes.png");
    player.width = stick.width * 3;
    player.height = stick.height * 3;

    playerAnim = assignProperties(15, 0, 3, true, 2, true);
    playerAnim.texture = LoadTexture("resources/glustickanimation.png");

    playerAnim_flip = assignProperties(15, 0, 3, true, 2, true);
    playerAnim_flip.texture = LoadTexture("resources/glustickanimation.png");
    playerAnim_flip = flipAnimationHorizontal(playerAnim_flip);

    background = assignProperties(300, 0, 10, true, 4, true);
    background.texture = LoadTexture("resources/background.png");

    patternList[0] = NewRecPattern(newRec(900,0,50,400), 0);
    AddRecPattern(patternList[0], newRec(890, 0, 10, 100), 1);
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