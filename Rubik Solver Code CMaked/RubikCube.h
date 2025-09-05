#pragma once
#include <string>
#include <cstring>

class RubikCube
{
public:
    int rc[6][3][3];
    int dir[6] = { 0, 1, 2, 3, 4, 5 };

    enum Color
    {
        White = 0,
        Blue = 1,
        Yellow = 2,
        Red = 3,
        Green = 5,
        Orange = 4
    };

    enum Orientation
    {
        Front = 0,
        Up = 1,
        Back = 2,
        Left = 3,
        Right = 4,
        Bottom = 5
    };

    void copyRC(int(&nrc)[6][3][3], int rc[6][3][3]);
    std::string intTocharColorType(int n);

    void identifyFaces(int(&dir)[6]);
    void rotateFaceToMatchOrientation(int dir[6], int(&rc)[6][3][3], bool undo);
    void actionRubik(int(&dir)[6], int(&rc)[6][3][3], char action, bool prime);
    void queueMove(RubikCube& RC, char *move);
    void ReadRubik(int(&dir)[6], int rc[6][3][3]);

private:
    void rotateMatrix(int(&ma)[3][3], int countRotation, int anticlockwise);
    void mirrorMatrix(int(&ma)[3][3]);
    void faceRotate(int face, int(&rc)[6][3][3], int anticlockwise);
};

