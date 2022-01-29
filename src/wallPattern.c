#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include "wallPattern.h"
#include "animation.h"

void PrintWallPattern(WallPattern* root){
    if(root == NULL){
        puts("WARNING: PrintWallPattern - List does not exist (Value is null). Skipping...");
    }else{
        WallPattern* select = root;
        while(select != NULL){
            printf("!%f,%p;", select->rec.x, select->next);
            select = select->next;
        }
        puts("");
    }
}

void MoveWallPattern(WallPattern* root, Vector2 move){
    WallPattern* select = root;
    while(select != NULL){
        select->rec = (Rectangle){select->rec.x + move.x, select->rec.y + move.y, select->rec.width, select->rec.height};
        select = select->next;
    }
}

WallPattern* NewRecPattern(Rectangle rec, int trigger){
    WallPattern* block = (WallPattern*)malloc(sizeof(WallPattern));
    block->rec = rec;
    block->trigger = trigger;
    block->anim = NULL;
    block->next = NULL;
    return block;
}

void AddRecPattern(WallPattern* root, Rectangle rec, int trigger){
    WallPattern* select = root;
    while(select->next != NULL){
        //printf("!%f,%p;", select->rec.x, select->next);
        select = select->next;
    }
    //printf("!%f,%p;", select->rec.x, select->next);
    select->next = NewRecPattern(rec, trigger);
    //puts("\n");
}

//Must already have the root allocated
WallPattern* CopyWallPattern(WallPattern* root){
    WallPattern* copy = NewRecPattern(root->rec, root->trigger);
    WallPattern* select = root->next;
    while(select != NULL){
        AddRecPattern(copy, select->rec, select->trigger);
        select = select->next;
    }
    return copy;
}

// Frees all items in a wall pattern
void FreeWallPattern(WallPattern* root){
    WallPattern* select = root;
    WallPattern* next = NULL;
    while(select != NULL){
        next = select->next;
        select->rec = (Rectangle){0,0,0,0};
        select->next = NULL;
        if(select->anim != NULL){
            free(select->anim);
            select->anim = NULL;
            printf("Freed anim\n");
        }
        //printf("!!freed %p; ", select);
        //printf("!!child %p\n", select->next);
        free(select);
        //printf("freed %p; ", select);
        //printf("child %p\n", select->next);
        select = next;
    }
}

//WallPattern* ArrayToWallPattern();
//Rectangle* WallPatternToArray();


// This function has not been tested
WallPattern* WallPatternAtIndex(WallPattern* root, int index){
    WallPattern* select = root;
    for(int i = 0; i < index && select != NULL; i++){
        select = select->next;
    }
    if(select == NULL){
        printf("ERROR: WallPatternAtIndex - Could not find item of index %d. (List ended before value)\n", index);
    }
    return select;
}

float GreatestWallX(WallPattern* root){
    WallPattern* select = root;
    float maxX = 0.0f;
    //printf("this worked\n");
    while(select != NULL){
        if(select->rec.x + select->rec.width > maxX) maxX = select->rec.x + select->rec.width;
        select = select->next;
    }
    return maxX;
}