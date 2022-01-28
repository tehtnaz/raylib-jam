#ifndef WALLPATTERN_H
    #define WALLPATTERN_H
    #include "raylib.h"


    typedef struct WallPattern{
        Rectangle rec;
        int trigger;
        struct WallPattern* next;
    }WallPattern;


    void PrintWallPattern(WallPattern* root);
    void MoveWallPattern(WallPattern* root, Vector2 move);

    WallPattern* NewRecPattern(Rectangle rec, int trigger);
    void AddRecPattern(WallPattern* root, Rectangle rec, int trigger);

    WallPattern* CopyWallPattern(WallPattern* root);
    void FreeWallPattern(WallPattern* root);

    // This function has not been tested
    WallPattern* WallPatternAtIndex(WallPattern* root, int index);

    float GreatestWallX(WallPattern* root);

#endif //WALLPATTERN_H