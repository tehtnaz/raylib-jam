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

#include "gui2.h"

typedef enum GameState{
    STATE_ACTIVE,
    STATE_DEAD,
    STATE_MENU
}GameState;

Texture2D stick;
Texture2D frame;
Texture2D spikeTexture;
Texture2D trapz;
Animation background;
Animation playerAnim;
Animation playerAnim_flip;

Music main_music;
Music menu;
Sound death_sound;

bool wallJump = false;
float velocity = 0.0f;
const float gravity = 9.81f * 3.0f;
const float jumpHeight = -16.0f;
const float playerSpeed = 325.0f;

Rectangle player = {100, 50, 0, 0};
Vector2 startingPos = {100, 50};
const Rectangle ground = {0, 492, 900, 100};
const Rectangle spikes = {0,0,21,507};
const Rectangle ceiling = {0,-100,900,100};


int wallNum = 0;
WallPattern* patternList[6];
WallPattern** wallsList;

float wallSpeed = -150.0f;
float speedUpTimer = 0.0f;
const float speedUpMultiplier = 1.05f;
const float speedUpResetTime = 4.5f;
const float wallSpacing = 200.0f;


bool jumpDebug = false;
bool debug = false;

GameState gameState = STATE_MENU;

CollisionInfo collision = {0,0,0,0,0,0};

GuiText playText;
GuiBox playBox;

int score = 0;
int highscore = 0;

//Animation pistonAnim;
//WallPattern* piston;
//WallPattern* basic;
//bool wallHoldDebug = false;
//int trapNum = 0;
//WallPattern** trapParents;

void NewWall(){
    wallNum++;
    int rnd = GetRandomValue(0, 5);
    wallsList = realloc(wallsList, wallNum * sizeof(WallPattern*));
    wallsList[wallNum-1] = CopyWallPattern(patternList[rnd]);

    if(rnd == 1){
        wallsList[wallNum-1]->next->next->anim = malloc(sizeof(Animation));
        *(wallsList[wallNum-1]->next->next->anim) = assignProperties(93, 0, 3, false, 4, false);
        wallsList[wallNum-1]->next->next->anim->texture = trapz;
        printf("Made new animation\n");
    }
    if(rnd == 5){
        wallsList[wallNum-1]->next->next->next->anim = malloc(sizeof(Animation));
        *(wallsList[wallNum-1]->next->next->next->anim) = assignProperties(93, 0, 3, false, 4, false);
        wallsList[wallNum-1]->next->next->next->anim->texture = trapz;
        printf("Made new animation\n");
    }
}
void KillWall(){
    FreeWallPattern(wallsList[0]);
    for(int i = 0; i < wallNum - 1; i++){
        wallsList[i] = wallsList[i+1];
    }
    wallNum--;
}

void MoveWalls(){
    for(int i = 0; i < wallNum; i++){
        MoveWallPattern(wallsList[i], (Vector2){wallSpeed * GetFrameTime(), 0});
        score += wallSpeed * GetFrameTime() * -0.5;
    }
    bool done = false;
    while(!done){
        if(wallsList[0]->rec.x < -1000 && wallNum > 0){
            KillWall();
        }else{
            done = true;
        }
    }
}

void CalculateCollisions(){
    collision = (CollisionInfo){0,0,0,0,0,0};
    for(int i = 0; i < wallNum; i++){
        collision = CheckAllCollisionsList(collision, wallsList[i], player);
    }
    collision = CheckColliderColInfo(collision, ground, player, 0);
    collision = CheckColliderColInfo(collision, ceiling, player, 0);
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
        }else{
            player.x += wallSpeed * GetFrameTime();
            if(IsKeyPressed(KEY_W) && wallJump && velocity > 0){
                velocity = jumpHeight;
                wallJump = false;
            }
        }
    }
    if(IsKeyDown(KEY_D)){
        if(!collision.right){
            player.x += playerSpeed * GetFrameTime();
        }else if(IsKeyPressed(KEY_W) && wallJump && velocity > 0){
            velocity = jumpHeight;
            wallJump = false;
        }
    }
    if(collision.up && velocity < 0){
        velocity = 0;
    }
    if(collision.down && velocity >= 0){
        player.y = collision.floor - player.height + 0.01f;
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
        player = RotateRecCW(player);
        gameState = STATE_DEAD;
        PlaySound(death_sound);
    }
    if(collision.trigger == 2){
        ((WallPattern*)collision.triggerObjID)->next->anim->isAnimating = true;
    }
}

void DebugVisuals(){
    if(debug){
        DrawText(TextFormat("speedUpTimer: %f\n wallSpeed: %f\nvelocity: %f\nplayer.y: %f\ncollision.floor: %f", speedUpTimer, wallSpeed, velocity, player.y, collision.floor), 700, 0, 15, WHITE);
        DrawRectangle(870, 391, 20, 9, collision.up ? RED : WHITE);
        DrawRectangle(861, 400, 9, 20, collision.left ? RED : WHITE);
        DrawRectangle(890, 400, 9, 20, collision.right ? RED : WHITE);
        DrawRectangle(870, 420, 20, 9, collision.down ? RED : WHITE);

        DrawRectangle(870, 400, 20, 20, wallJump ? YELLOW : WHITE);

        DrawRectangleRec(player, (Color){255,0,0,100});

        DrawRectangle(861, 429, 38, 6, 
            collision.trigger == 0 ? WHITE :
            collision.trigger == 1 ? RED : 
            collision.trigger == 2 ? BLUE : 
            YELLOW
        );
        //if(collision.trigger == 2) PlaySound(death_sound);
    }
    if(jumpDebug) DrawText("Debug Mode Enabled.\nReset with R enabled", 0,0,15, WHITE);
    
}

void DrawWallPattern(WallPattern* root){
    WallPattern* select = root;
    while(select != NULL){
        //if(select->trigger == 2) DrawAnimationPro();else 
        if(select-> trigger == 1) DrawRectangleRec(select->rec, (Color){255,84-(select->rec.x * 84 / 900),98-(select->rec.x * 98 / 900),255});
        else if(select->trigger == 0)DrawRectangleRec(select->rec, (Color){74-(select->rec.x * 74 / 900),84-(select->rec.x * 84 / 900),98-(select->rec.x * 98 / 900),255});
        if(select->anim != NULL){
            DrawAnimationPro(select->anim, recToVec(select->rec), 3, WHITE, select->anim->isAnimating ? CYCLE_FORWARD : CYCLE_NONE);
            if(select->anim->isAnimating == false && select->anim->currentFrame == 3) select->trigger = 1;
        }
        select = select->next;
    }
}

void GameVisuals(){
    background.fps = -wallSpeed / 60;
    DrawAnimationPro(&background, (Vector2){0,0}, 3, WHITE, gameState == STATE_ACTIVE ? CYCLE_FORWARD : CYCLE_NONE);
    DrawTextureEx(frame, (Vector2){0,0}, 0, 3, WHITE);
    for(int i = 0; i < wallNum; i++){
        DrawWallPattern(wallsList[i]);
    }
    DrawTextureEx(spikeTexture, (Vector2){0,0}, 0, 3, WHITE);
    
    if((IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) && gameState == STATE_ACTIVE) DrawAnimationPro(IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) ? &playerAnim_flip : &playerAnim, recToVec(player), 3, WHITE, CYCLE_FORWARD);
    else DrawTextureEx(stick, recToVec(player), gameState == STATE_ACTIVE ? 0 : 90, 3, WHITE);

    if(score > highscore) highscore = score;
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
    DrawText(TextFormat("High Score: %d", highscore), 890 - MeasureText(TextFormat("High Score: %d", highscore), 20), 10, 20, WHITE);
}

void WallTimer(){
    speedUpTimer -= GetFrameTime();
    if(IsKeyPressed(KEY_F)){
        speedUpTimer = speedUpResetTime;
        wallSpeed /= speedUpMultiplier;
    }
    if(IsKeyPressed(KEY_G) || speedUpTimer < 0){
        speedUpTimer = speedUpResetTime;
        wallSpeed *= speedUpMultiplier;
    }
    if(wallNum == 0 || (900 - GreatestWallX(wallsList[wallNum-1])) > wallSpacing){
        NewWall();
    }
}

void BodyFall(){
    collision = (CollisionInfo){0,0,0,0,0,0};
    for(int i = 0; i < wallNum; i++){
        collision = CheckAllCollisionsList(collision, wallsList[i], player);
    }
    collision = CheckColliderColInfo(collision, ground, player, 0);
    collision = CheckColliderColInfo(collision, ceiling, player, 0);

    if(collision.up && velocity < 0){
        velocity = 0;
    }

    if(collision.down && velocity >= 0){
        player.y = collision.floor - player.height + 0.01f;
        velocity = 0;
        wallJump = true;
    }else{
        velocity += gravity * GetFrameTime();
        player.y += velocity;
    }
}

void UpdateDrawFrame(){
    if(gameState == STATE_MENU) UpdateMusicStream(menu);
    UpdateMusicStream(main_music);
    if(gameState != STATE_MENU){
        if(IsMusicStreamPlaying(menu)){
            StopMusicStream(menu);
            PlayMusicStream(main_music);
        }
        if(gameState == STATE_DEAD && IsMusicStreamPlaying(main_music)){
            StopMusicStream(main_music);
        }
        if(gameState == STATE_ACTIVE && !IsMusicStreamPlaying(main_music)){
            PlayMusicStream(main_music);
        }
    }
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
        player.width = stick.width * 3;
        player.height = stick.height * 3;
        speedUpTimer = speedUpResetTime;
        score = 0;
        wallSpeed = -150.0f;
        while(wallNum > 0){
            KillWall();
        }
    }
    if(gameState == STATE_ACTIVE){
        if(IsKeyPressed(KEY_R) && jumpDebug){
            player = RotateRecCW(player);
            gameState = STATE_DEAD;
            PlaySound(death_sound);
        }
        WallTimer();
        MoveWalls();
        CalculateCollisions();
    }else if(gameState == STATE_DEAD){
        BodyFall();
    }
    BeginDrawing();
        ClearBackground(RAYWHITE);
        if(gameState == STATE_ACTIVE  || gameState == STATE_DEAD){
            GameVisuals();
            DebugVisuals();
            if(gameState == STATE_DEAD){
                DrawText("You died.", player.x + 50 < 700 ? player.x + 50 : 700, player.y > 0 ? (player.y > 467 ? 467 : player.y) : 0, 40, RED);
            }
        }
        if(gameState == STATE_MENU){
            DrawAnimationPro(&background, (Vector2){0,0}, 3, WHITE, CYCLE_FORWARD);
            DrawAnimationPro(&playerAnim, (Vector2){450 - stick.width * 3, 507 / 2 - stick.height * 3}, 5, WHITE, CYCLE_FORWARD);
            DrawTextEx(GetFontDefault(), "Industrial\nRun", addNewVec2(negVec2(resizeVec2(MeasureTextEx(GetFontDefault(), "Industrial\nRun", 40, 3), 0.5)), 200, 200), 40, 3, WHITE);
            DrawTextEx(GetFontDefault(), "By: tehtnaz\nand LostBrand", addNewVec2(negVec2(resizeVec2(MeasureTextEx(GetFontDefault(), "By: tehtnaz\nand LostBrand", 40, 3), 0.5)), 700, 200), 40, 3, WHITE);
            renderGuiBox(playBox, true);
            if(isMouseInGuiBox(playBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                gameState = STATE_ACTIVE;
            }
        }
    EndDrawing();
}

int main(void){
    SetRandomSeed(1);
    InitWindow(900, 507, "Industrial Run");
    InitAudioDevice();
    SetTargetFPS(60);
    rlDisableBackfaceCulling();

    main_music = LoadMusicStream("resources/main.mp3");
    menu = LoadMusicStream("resources/menu.mp3");

    death_sound = LoadSound("resources/die.wav");

    SetSoundVolume(death_sound, 0.4f);
    PlayMusicStream(menu);
    SetMasterVolume(0.5f);
    //SetMusicVolume(main_music, 1);
    //SetMusicVolume(menu, 1);

    frame = LoadTexture("resources/frame.png");
    stick = LoadTexture("resources/stick.png");
    trapz = LoadTexture("resources/trapz.png");
    player.width = stick.width * 3;
    player.height = stick.height * 3;
    spikeTexture = LoadTexture("resources/spikes.png");

    playerAnim = assignProperties(15, 0, 3, true, 2, true);
    playerAnim.texture = LoadTexture("resources/glustickanimation.png");

    playerAnim_flip = assignProperties(15, 0, 3, true, 2, true);
    playerAnim_flip.texture = LoadTexture("resources/glustickanimation.png");
    playerAnim_flip = flipAnimationHorizontal(playerAnim_flip);

    background = assignProperties(300, 0, 10, true, 4, true);
    background.texture = LoadTexture("resources/background.png");

    //pistonAnim = assignProperties(56, 0, 10000, false, 2, false);
    //pistonAnim.texture = LoadTexture("resources/background.png");

    //piston = NewRecPattern(newRec(0,0,pistonAnim.spriteSize * 3, pistonAnim.texture.height * 3), 0);

    Font default_font = GetFontDefault();

    playText = assignGuiText(&default_font, (Vector2){450,100}, (Vector2){0,0}, "Play", 40, BLUE, 3);
    playBox = assignGuiBox((Rectangle){0,0,0,0}, &playText, NULL, 1, WHITE, 3, BLACK);
    playBox = offsetGuiBox(playBox, (Vector2){0,200}, (Vector2){150,60}, true, 900, 507);
    playText = setGuiTextOrigin(playBox, playText, true);

    //wall climb
    patternList[0] = NewRecPattern(newRec(900, 0, 20 * 3, 126 * 3), 0);
    AddRecPattern(patternList[0], newRec(900 + 43 * 3, 36 * 3, 20 * 3, 128 * 3), 0);

    //pit
    patternList[1] = NewRecPattern(newRec(900, 122 * 3, 23 * 3, 42 * 3), 0);
    AddRecPattern(patternList[1], newRec(900 + 23 * 3, 152 * 3, 90 * 3, 5 * 3), 2); // animation trigger
    AddRecPattern(patternList[1], newRec(900 + 23 * 3, 154 * 3, 93 * 3, 15 * 3), 0); // actual death pit
    AddRecPattern(patternList[1], newRec(900 + 116 * 3, 122 * 3, 24 * 3, 42 * 3), 0);

    //l blocks
    patternList[2] = NewRecPattern(newRec(900,         55 * 3,    79 * 3, 21 * 3), 0);
    AddRecPattern(patternList[2], newRec(900,          105 * 3,   79 * 3, 21 * 3), 0);
    AddRecPattern(patternList[2], newRec(900 + 59 * 3, 126 * 3,   20 * 3, 38 * 3), 0);
    AddRecPattern(patternList[2], newRec(900 + 59 * 3, 0,         20 * 3, 55 * 3), 0);

    //[3] = jumps
    //[3] left side
    patternList[3] = NewRecPattern(newRec(900, 0, 24 * 3, 129 * 3), 0);
    AddRecPattern(patternList[3], newRec(900 + 24 * 3, 35 * 3, 10 * 3, 15 * 3), 0);
    AddRecPattern(patternList[3], newRec(900 + 24 * 3, 118 * 3, 10 * 3, 15 * 3), 0);

    //[3] right side
    AddRecPattern(patternList[3], newRec(900 + 55 * 3, 68 * 3, 23 * 3, 18 * 3), 0);
    //AddRecPattern(patternList[3], newRec(900 + 78 * 3, 0, 24 * 3, 12 * 3), 0);
    AddRecPattern(patternList[3], newRec(900 + 78 * 3, 38 * 3, 24 * 3, 126 * 3), 0);

    patternList[4] = NewRecPattern(newRec(900, 51 * 3, 14 * 3, 113* 3), 0);
    AddRecPattern(patternList[4], newRec(900 + 47 * 3, 28 * 3, 14 * 3, 56 * 3), 0);
    AddRecPattern(patternList[4], newRec(900 + 47 * 3, 132 * 3, 14 * 3, 32 * 3), 0);
    AddRecPattern(patternList[4], newRec(900 + 92 * 3, 51 * 3, 14 * 3, 113* 3), 0);

    patternList[5] = NewRecPattern(newRec(900, 112 * 3, 20 * 3, 52 * 3), 0);
    AddRecPattern(patternList[5], newRec(900 + 50 * 3, 46 * 3, 18 * 3, 118* 3), 0);
    AddRecPattern(patternList[5], newRec(900 + 68 * 3, 149 * 3, 93 * 3, 5 * 3), 2);
    AddRecPattern(patternList[5], newRec(900 + 68 * 3, 154 * 3, 93 * 3, 15 * 3), 0);
    AddRecPattern(patternList[5], newRec(900 + 161 * 3, 132 * 3, 17 * 3, 32 * 3), 0);
    AddRecPattern(patternList[5], newRec(900 + 161 * 3, 0, 17 * 3, 99* 3), 0);
    AddRecPattern(patternList[5], newRec(900 + 107 *3, 0, 54 * 3, 99  * 3), 0);

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