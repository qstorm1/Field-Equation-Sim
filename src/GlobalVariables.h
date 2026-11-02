#ifndef GLOABLVARIBLES_H
#define GLOABLVARIBLES_H

#endif //GLOABLVARIBLES_H

struct GlobalVariables{
    //TODO: make settings menu to change the variables that may need changing
    static constexpr int initWidth = 800;
    static constexpr int initHeight = 600;

    inline static float cameraFOV=90.0f*std::numbers::pi/180.0f;//radiant's
    inline static float zNear=0.1;
    inline static float zFar=100;
    inline static float sensX=10;
    inline static float sensY=10;
    inline static float forwardMoveExcel=10;
    inline static float backwardMoveExcel=10;
    inline static float sideMoveExcel=10;
    inline static float yMoveExcel=10;
    inline static float aspectRatio=static_cast<float>(initWidth)/static_cast<float>(initHeight);

    static void resetGlobalVariables()
    {
        cameraFOV=90;
        zNear=0.1;
        zFar=100;
        sensX=10;
        sensY=10;
        forwardMoveExcel=10;
        backwardMoveExcel=10;
        sideMoveExcel=10;
    }
};