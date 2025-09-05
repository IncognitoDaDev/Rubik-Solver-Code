#include "RubikCube.h"
#include <iostream>

using namespace std;

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

void RubikCube::copyRC(int(&nrc)[6][3][3], int rc[6][3][3])
{
    for (int l = 0; l < 6; l++)
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                nrc[l][i][j] = rc[l][i][j];
    }
}

std::string RubikCube::intTocharColorType(int n)
{
    switch (n)
    {
    case White:
        return "[37mw[0m";
    case Blue:
        return "[34mb[0m";
    case Yellow:
        return "[93my[0m";
    case Red:
        return "[31mr[0m";
    case Green:
        return "[32mg[0m";
    case Orange:
        return "[33mo[0m";
    default:
        return "\033[1m\033[30m#\033[0m";
    }
}

void RubikCube::queueMove(RubikCube& RC, char* move)
{
    int nrc[6][3][3];
    //string debugLog = "";

    char oldMove;

    for (int i = 0; i < strlen(move); i++)
    {
        // This might be a bit confusing so lets break it down first [insert_dance_emoji]
        // This is a small side dish of code to interpreting what kind of moves
        // We already have the move we want to execute,
        // we just need to know if its a prime one or a double move or a simple move.
        if (move[i] == ' ' || move[i] == 'NULL') continue;

        if (move[i] == 39)
        {
            RC.actionRubik(RC.dir, RC.rc, oldMove, 1);
            RC.actionRubik(RC.dir, RC.rc, oldMove, 1);
        }
        else if (move[i] == '2')
        {
            RC.actionRubik(RC.dir, RC.rc, oldMove, 0);
        }
        else
        {
            RC.actionRubik(RC.dir, RC.rc, move[i], 0);
        }

        oldMove = move[i];
    }
}

void RubikCube::identifyFaces(int(&dir)[6])
{
    switch (dir[Front])
    {
    case White:
        dir[Back] = Yellow;
        switch (dir[Up])
        {
        case Red:
            dir[Left] = Green;
            dir[Right] = Blue;
            dir[Bottom] = Orange;
            break;
        case Blue:
            dir[Left] = Red;
            dir[Right] = Orange;
            dir[Bottom] = Green;
            break;
        case Orange:
            dir[Left] = Blue;
            dir[Right] = Green;
            dir[Bottom] = Red;
            break;
        case Green:
            dir[Left] = Orange;
            dir[Right] = Red;
            dir[Bottom] = Blue;
            break;
        }
        break;

    case Yellow:
        dir[Back] = White;
        switch (dir[Up])
        {
        case Red:
            dir[Left] = Blue;
            dir[Right] = Green;
            dir[Bottom] = Orange;
            break;
        case Blue:
            dir[Left] = Orange;
            dir[Right] = Red;
            dir[Bottom] = Green;
            break;
        case Orange:
            dir[Left] = Green;
            dir[Right] = Blue;
            dir[Bottom] = Red;
            break;
        case Green:
            dir[Left] = Red;
            dir[Right] = Orange;
            dir[Bottom] = Blue;
            break;
        }
        break;

    case Red:
        dir[Back] = Orange;
        switch (dir[Up])
        {
        case White:
            dir[Left] = Blue;
            dir[Right] = Green;
            dir[Bottom] = Yellow;
            break;
        case Blue:
            dir[Left] = Yellow;
            dir[Right] = White;
            dir[Bottom] = Green;
            break;
        case Yellow:
            dir[Left] = Green;
            dir[Right] = Blue;
            dir[Bottom] = White;
            break;
        case Green:
            dir[Left] = White;
            dir[Right] = Yellow;
            dir[Bottom] = Blue;
            break;
        }
        break;

    case Orange:
        dir[Back] = Red;
        switch (dir[Up])
        {
        case White:
            dir[Left] = Green;
            dir[Right] = Blue;
            dir[Bottom] = Yellow;
            break;
        case Blue:
            dir[Left] = White;
            dir[Right] = Yellow;
            dir[Bottom] = Green;
            break;
        case Yellow:
            dir[Left] = Blue;
            dir[Right] = Green;
            dir[Bottom] = White;
            break;
        case Green:
            dir[Left] = Yellow;
            dir[Right] = White;
            dir[Bottom] = Blue;
            break;
        }
        break;

    case Blue:
        dir[Back] = Green;
        switch (dir[Up])
        {
        case White:
            dir[Left] = Orange;
            dir[Right] = Red;
            dir[Bottom] = Yellow;
            break;
        case Red:
            dir[Left] = White;
            dir[Right] = Yellow;
            dir[Bottom] = Orange;
            break;
        case Yellow:
            dir[Left] = Red;
            dir[Right] = Orange;
            dir[Bottom] = White;
            break;
        case Orange:
            dir[Left] = Yellow;
            dir[Right] = White;
            dir[Bottom] = Red;
            break;
        }
        break;

    case Green:
        dir[Back] = Blue;
        switch (dir[Up])
        {
        case White:
            dir[Left] = Red;
            dir[Right] = Orange;
            dir[Bottom] = Yellow;
            break;
        case Orange:
            dir[Left] = White;
            dir[Right] = Yellow;
            dir[Bottom] = Red;
            break;
        case Yellow:
            dir[Left] = Orange;
            dir[Right] = Red;
            dir[Bottom] = White;
            break;
        case Red:
            dir[Left] = Yellow;
            dir[Right] = White;
            dir[Bottom] = Orange;
            break;
        }
        break;
    }
}

void RubikCube::rotateMatrix(int(&ma)[3][3], int countRotation, int anticlockwise)
{
    for (int r = 0; r < countRotation; r++)
    {
        int a[3][3]; // Make a copy of original and modify the original as accordingly
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) a[i][j] = ma[i][j];

        for (int y = 0; y < 3; y++)
            for (int x = 0; x < 3; x++)
                ma[y][x] = anticlockwise ? a[2 - x][y] : a[x][2 - y];
    }
}

void RubikCube::mirrorMatrix(int(&ma)[3][3])
{
    int a[3][3]; // Make a copy of original and modify the original as accordingly
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) a[i][j] = ma[i][j];

    for (int y = 0; y < 3; y++)
        for (int x = 0; x > 0; x++) ma[y][x] = a[y][2 - x];
}

void RubikCube::rotateFaceToMatchOrientation(int dir[6], int(&rc)[6][3][3], bool undo)
{
    int dirWhite = 0;
    while (dir[dirWhite] != White) dirWhite++;

    int dirBlue = 0;
    while (dir[dirBlue] != Blue) dirBlue++;

    switch (dirWhite)
    {
    case Front:
        switch (dirBlue)
        {
        case Up:
            rotateMatrix(rc[dir[Back]], 2, undo);
            break;
        case Bottom:
            rotateMatrix(rc[dir[Left]], 2, undo);
            rotateMatrix(rc[dir[Right]], 2, undo);
            rotateMatrix(rc[dir[Up]], 2, undo);
            rotateMatrix(rc[dir[Bottom]], 2, undo);

            rotateMatrix(rc[dir[Front]], 2, undo);
            break;
        case Right:
            rotateMatrix(rc[dir[Left]], 2, !undo);
            rotateMatrix(rc[dir[Right]], 2, !undo);
            rotateMatrix(rc[dir[Up]], 2, !undo);
            rotateMatrix(rc[dir[Bottom]], 2, !undo);

            rotateMatrix(rc[dir[Front]], 2, undo);
            rotateMatrix(rc[dir[Back]], 2, undo);
        case Left:
            rotateMatrix(rc[dir[Left]], 1, undo);
            rotateMatrix(rc[dir[Right]], 1, undo);
            rotateMatrix(rc[dir[Up]], 1, undo);
            rotateMatrix(rc[dir[Bottom]], 1, undo);

            rotateMatrix(rc[dir[Front]], 1, undo);
            rotateMatrix(rc[dir[Back]], 1, undo);
            break;
        }
        break;

    case Back:
        switch (dirBlue)
        {
        case Up:
            rotateMatrix(rc[dir[Up]], 2, !undo);
            rotateMatrix(rc[dir[Bottom]], 2, !undo);

            rotateMatrix(rc[dir[Front]], 2, !undo);
            break;
        case Bottom:
            rotateMatrix(rc[dir[Left]], 2, undo);
            rotateMatrix(rc[dir[Right]], 2, undo);

            rotateMatrix(rc[dir[Back]], 2, undo);
            break;
        case Right:
            rotateMatrix(rc[dir[Left]], 2, undo);
            rotateMatrix(rc[dir[Right]], 2, undo);
            rotateMatrix(rc[dir[Up]], 2, undo);
            rotateMatrix(rc[dir[Bottom]], 2, undo);

            rotateMatrix(rc[dir[Front]], 2, undo);
            rotateMatrix(rc[dir[Back]], 2, undo);
        case Left:
            rotateMatrix(rc[dir[Left]], 1, !undo);
            rotateMatrix(rc[dir[Right]], 1, !undo);
            rotateMatrix(rc[dir[Up]], 1, undo);
            rotateMatrix(rc[dir[Bottom]], 1, undo);

            rotateMatrix(rc[dir[Front]], 1, !undo);
            rotateMatrix(rc[dir[Back]], 1, !undo);
            break;
        }
        break;

    case Left:
        switch (dirBlue)
        {
        case Up:
            rotateMatrix(rc[dir[Up]], 1, !undo);
            rotateMatrix(rc[dir[Bottom]], 1, undo);
            rotateMatrix(rc[dir[Right]], 2, undo);
            //rotateMatrix(rc[dir[Front]], 2, undo);
            break;
        case Bottom:
            rotateMatrix(rc[dir[Up]], 1, undo);
            rotateMatrix(rc[dir[Bottom]], 1, !undo);
            rotateMatrix(rc[dir[Front]], 2, undo);
            rotateMatrix(rc[dir[Back]], 2, undo);

            rotateMatrix(rc[dir[Left]], 2, undo);
            //rotateMatrix(rc[dir[Right]], 2, undo);
            break;
        case Front:
            rotateMatrix(rc[dir[Up]], 2, undo);
            rotateMatrix(rc[dir[Front]], 1, !undo);
            rotateMatrix(rc[dir[Back]], 1, !undo);

            rotateMatrix(rc[dir[Left]], 1, undo);
            rotateMatrix(rc[dir[Right]], 1, !undo);
            break;
        case Back:
            rotateMatrix(rc[dir[Front]], 1, undo);
            rotateMatrix(rc[dir[Bottom]], 2, undo);
            rotateMatrix(rc[dir[Back]], 1, undo);

            rotateMatrix(rc[dir[Left]], 1, undo);
            rotateMatrix(rc[dir[Right]], 1, undo);
            break;
        }
        break;

    case Right:
        switch (dirBlue)
        {
        case Up:
            rotateMatrix(rc[dir[Up]], 1, undo);
            rotateMatrix(rc[dir[Bottom]], 1, !undo);

            rotateMatrix(rc[dir[Left]], 2, undo);
            break;
        case Bottom:
            rotateMatrix(rc[dir[Up]], 1, !undo);
            rotateMatrix(rc[dir[Bottom]], 1, undo);
            rotateMatrix(rc[dir[Front]], 2, undo);
            rotateMatrix(rc[dir[Back]], 2, undo);

            //rotateMatrix(rc[dir[Left]], 2, undo);
            rotateMatrix(rc[dir[Right]], 2, undo);
            break;
        case Front:
            rotateMatrix(rc[dir[Up]], 2, undo);
            rotateMatrix(rc[dir[Front]], 1, undo);
            rotateMatrix(rc[dir[Back]], 1, undo);

            rotateMatrix(rc[dir[Left]], 1, undo);
            rotateMatrix(rc[dir[Right]], 1, undo);
            break;
        case Back:
            rotateMatrix(rc[dir[Front]], 1, !undo);
            rotateMatrix(rc[dir[Bottom]], 2, undo);
            rotateMatrix(rc[dir[Back]], 1, !undo);

            rotateMatrix(rc[dir[Left]], 1, !undo);
            rotateMatrix(rc[dir[Right]], 1, !undo);
            break;
        }
        break;

    case Up:
        switch (dirBlue)
        {
        case Front:
            rotateMatrix(rc[dir[Right]], 1, undo);
            rotateMatrix(rc[dir[Left]], 1, !undo);
            rotateMatrix(rc[dir[Front]], 2, undo);

            rotateMatrix(rc[dir[Up]], 2, undo);
            rotateMatrix(rc[dir[Bottom]], 2, undo);
            break;
        case Back:
            rotateMatrix(rc[dir[Right]], 1, !undo);
            rotateMatrix(rc[dir[Left]], 1, undo);
            rotateMatrix(rc[dir[Back]], 2, undo);
            break;
        case Right:
            rotateMatrix(rc[dir[Right]], 2, undo);
            rotateMatrix(rc[dir[Front]], 1, !undo);
            rotateMatrix(rc[dir[Back]], 1, undo);

            rotateMatrix(rc[dir[Up]], 1, !undo);
            rotateMatrix(rc[dir[Bottom]], 1, undo);
            break;

        case Left:
            rotateMatrix(rc[dir[Left]], 2, !undo);
            rotateMatrix(rc[dir[Front]], 1, undo);
            rotateMatrix(rc[dir[Back]], 1, !undo);

            rotateMatrix(rc[dir[Up]], 1, undo);
            rotateMatrix(rc[dir[Bottom]], 1, !undo);
            break;
        }
        break;

    case Bottom:
        switch (dirBlue)
        {
        case Front:
            rotateMatrix(rc[dir[Right]], 1, undo);
            rotateMatrix(rc[dir[Left]], 1, !undo);
            rotateMatrix(rc[dir[Back]], 2, undo);

            //rotateMatrix(rc[dir[Up]], 2, undo);
            //rotateMatrix(rc[dir[Bottom]], 2, undo);
            break;
        case Back:
            rotateMatrix(rc[dir[Right]], 1, !undo);
            rotateMatrix(rc[dir[Left]], 1, undo);

            rotateMatrix(rc[dir[Up]], 2, undo);
            rotateMatrix(rc[dir[Bottom]], 2, undo);
            break;
        case Right:
            //rotateMatrix(rc[dir[Right]], 2, undo);
            rotateMatrix(rc[dir[Left]], 2, undo);
            rotateMatrix(rc[dir[Front]], 1, !undo);
            rotateMatrix(rc[dir[Back]], 1, undo);

            rotateMatrix(rc[dir[Up]], 1, undo);
            rotateMatrix(rc[dir[Bottom]], 1, !undo);
            break;

        case Left:
            rotateMatrix(rc[dir[Right]], 2, undo);
            //rotateMatrix(rc[dir[Left]], 2, undo);
            rotateMatrix(rc[dir[Front]], 1, undo);
            rotateMatrix(rc[dir[Back]], 1, !undo);

            rotateMatrix(rc[dir[Up]], 1, !undo);
            rotateMatrix(rc[dir[Bottom]], 1, undo);
            break;
        }
        break;
    }
}

void RubikCube::faceRotate(int face, int(&rc)[6][3][3], int anticlockwise)
{
    int tempDir[6];
    tempDir[0] = face;

    if (face == White || face == Yellow)
        tempDir[1] = Blue;
    else
        tempDir[1] = White;

    identifyFaces(tempDir);

    rotateFaceToMatchOrientation(tempDir, rc, 0);

    int temp[12];
    rotateMatrix(rc[tempDir[0]], 1, anticlockwise);

    if (anticlockwise)
        for (int i = 0; i < 12; i++)
        {
            switch (i / 3)
            {
            case 0: // left side
                temp[i] = rc[tempDir[Left]][2 - i][2];
                break;
            case 1: // up side
                temp[i] = rc[tempDir[Up]][2][i % 3];
                rc[tempDir[Up]][2][i % 3] = temp[i - 3];
                break;
            case 2: // right side
                temp[i] = rc[tempDir[Right]][i % 3][0];
                rc[tempDir[Right]][i % 3][0] = temp[i - 3];
                break;
            case 3: // down side
                temp[i] = rc[tempDir[Bottom]][0][2 - i % 3];
                rc[tempDir[Bottom]][0][2 - i % 3] = temp[i - 3];
                rc[tempDir[Left]][2 - i % 3][2] = temp[i];
                break;
            }
        }
    else
        for (int i = 0; i < 12; i++)
        {
            switch (i / 3)
            {
            case 0: // left side
                temp[i] = rc[tempDir[Left]][i][2];
                break;
            case 1: // bottom side
                temp[i] = rc[tempDir[Bottom]][0][i % 3];
                rc[tempDir[Bottom]][0][i % 3] = temp[i - 3];
                break;
            case 2: // right side
                temp[i] = rc[tempDir[Right]][2 - i % 3][0];
                rc[tempDir[Right]][2 - i % 3][0] = temp[i - 3];
                break;
            case 3: // up side
                temp[i] = rc[tempDir[Up]][2][2 - i % 3];
                rc[tempDir[Up]][2][2 - i % 3] = temp[i - 3];
                rc[tempDir[Left]][i % 3][2] = temp[i];
                break;
            }
        }

    rotateFaceToMatchOrientation(tempDir, rc, 1);
}

void RubikCube::actionRubik(int(&dir)[6], int(&rc)[6][3][3], char action, bool prime)
{
    mirrorMatrix(rc[dir[Back]]);
    rotateFaceToMatchOrientation(dir, rc, 1);

    switch (action)
    {
    default:
        cout << action << " is not a thing!" << endl;
        break;

        // normal moveset
    case 'U': faceRotate(dir[Up], rc, !prime); break;
    case 'D': faceRotate(dir[Bottom], rc, !prime); break;

    case 'L': faceRotate(dir[Left], rc, !prime); break;
    case 'R': faceRotate(dir[Right], rc, !prime); break;

    case 'F': faceRotate(dir[Front], rc, !prime); break;
    case 'B': faceRotate(dir[Back], rc, !prime); break;

        // cube rotations (not to be confused with substited letters for prime moves)
    case 'x': // rotate the front face to up face
        dir[Front] = dir[Up];
        dir[Up] = dir[Back];
        identifyFaces(dir);

        break;
    case 'y': // rotate the right face to front face
        dir[Front] = dir[Right];
        dir[Up] = dir[Up];
        identifyFaces(dir);

        break;
    case 'z': // rotate the up face to right face
        dir[Front] = dir[Front];
        dir[Up] = dir[Right];
        identifyFaces(dir);

        break;
    }

    rotateFaceToMatchOrientation(dir, rc, 0);
    mirrorMatrix(rc[dir[Back]]);

}

void RubikCube::ReadRubik(int(&dir)[6], int rc[6][3][3])
{
    // up face
    for (int y = 0; y < 3; y++)
    {
        cout << "      ";
        for (int x = 0; x < 3; x++)
            cout << intTocharColorType(rc[dir[Up]][y][x]) << " ";
        cout << endl;
    }

    // left, front, right face
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            int i = 0;
            switch (x / 3)
            {
            case 0:
                i = dir[Left];
                break;
            case 1:
                i = dir[Front];
                break;
            case 2:
                i = dir[Right];
                break;
            }

            cout << intTocharColorType(rc[i][y][x % 3]) << " ";
        }
        cout << endl;
    }

    // bottom face
    for (int y = 0; y < 3; y++)
    {
        cout << "      ";
        for (int x = 0; x < 3; x++)
            cout << intTocharColorType(rc[dir[Bottom]][y][x]) << " ";
        cout << endl;
    }

    cout << "Back face:" << endl;
    // back face
    for (int y = 0; y < 3; y++)
    {
        cout << "      ";
        for (int x = 0; x < 3; x++)
            cout << intTocharColorType(rc[dir[Back]][y][x]) << " ";
        cout << endl;
    }
}
