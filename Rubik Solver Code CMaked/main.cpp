#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <cmath>
#include <fstream>

#include "RubikCube.h" // Rubik Cube Simulator
#include "Webcam.h" // Webcam for Scanning Colored Squares
#include "AStarSearch.h" // A* Algorithm for solving the cube
#include "PuzzleState.h" // This is for hashing the cube in a way the A* understands and solves
#include "main.h"


using namespace std;
using namespace cv;

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

void ResetSearch(AStarSearch astarsearch, PuzzleState startnode)
{
    astarsearch = AStarSearch();
    startnode = PuzzleState();
}

int main()
{
    int SquareCheck = 1, ColorCheck = 1, RubikGame = 1, FileInputOverride = 1;
    VideoCapture cam;
    Mat frame; bool ok = 1;

    RubikCube RC;
    Webcam Cam;


    if (!FileInputOverride)
    {
        cam = VideoCapture(1);

        if (!cam.isOpened())
        {
            cout << "No cam detected...";
            return 0;
        }
    }

    int nr = 0;
    while (ok && !FileInputOverride)
    {
        cam >> frame;
        if (frame.empty()) // Check if the frame is empty
        {
            cout << "Error: Captured frame is empty!" << endl;
            break;
        }
        
        vector<vector<Point>> sq;
        vector<int> colorType;


        if (RubikGame)
        {
            // translate the squares to data as input (while also filtering the extras)
            Cam.findSquaresAlt(frame, sq, colorType);
            Cam.filterExtraContours(sq, colorType);

            // debug -> mark individually the squares gathered as data
            if (SquareCheck)
                Cam.drawSquares(frame, sq);

            // debug -> render individually the color gathered as data (minus white)
            if (ColorCheck)
            {
                Mat color = Cam.CombineColorFilterResults(frame, vector<int>({ Blue, Yellow, Red, Green, Orange }));
                addWeighted(frame, 0.5, color, 1.0, 0.0, frame);
            }

            // translate and position the squares into a 3x3 matrix (and repeat it to the console)
            int a[3][3];
            Cam.translateFaceColor(frame, sq, colorType, a);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                    cout << RC.intTocharColorType(a[i][j]) << " ";

                cout << endl;
            }

            // user will position the rubik cube and press 'r' when sure to save input
            if (waitKey(500) == 'r')
            {
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        RC.rc[nr][i][j] = a[i][j];

                cout << "Input saved (" << nr+1 << "/6)" << endl;
                cout << endl;

                nr++;
            }

            switch (nr)
            {
            case 0: // frontFace
                cout << "f: White / u: Blue";
                break;
            case 1: // upFace
                cout << "f: Blue / u: Yellow";
                break;
            case 2: // backFace
                cout << "f: Yellow / u: Blue";
                break;
            case 3: // leftFace
                cout << "f: Red / u: Blue";
                break;
            case 4: // rightFace
                cout << "f: Orange / u: Blue";
                break;
            case 5: // bottomFace
                cout << "f: Green / u: White";
                break;
            case 6:
                ok = 0;
                cout << "Creating an image of rubik cube 2D..." << endl;
                break;
            }

            cout << endl;
        }
        
        imshow("Webcam", frame);
        
        if (waitKey(300) == 'q') ok = 0; // Stop the program 
    }

    if(!FileInputOverride)
    {
        cam.release();
        destroyAllWindows();
    }
    
    if (FileInputOverride)
    {
        ifstream fin("RubikCube.txt");
        int z;
        for (int l = 0; l < 6; l++)
            for (int y = 0; y < 3; y++)
                for (int x = 0; x < 3; x++)
                {
                    fin >> z;
                    RC.rc[l][y][x] = z;
                }

        fin.close();
    }

    string NameOutput = (string)"RubikSession" + (string)".txt";
    ofstream fout(NameOutput);

    AStarSearch aStarSearch;
    PuzzleState startNode;
    RubikCube nRC;
    string debugLog = "";
    if (RubikGame)
    {
        RC.copyRC(nRC.rc, RC.rc);
        RC.rotateFaceToMatchOrientation(RC.dir, RC.rc, 0);
        RC.ReadRubik(RC.dir, RC.rc);
    }

    char oldMove;

    while (RubikGame)
    {
        char q[] = "B F D B F2 U' D2 F2 L' U2";
        int tempDir[6] = { 0, 1, 2 , 3, 4, 5 };
        char move;
        cout << endl << "Your move... ";
        cin >> move;

            switch (move)
            {
            case '0': // Stop the Game
                RubikGame = 0;
                break;

            case '-': // Reset the Game
                RC.copyRC(RC.rc, nRC.rc);
                for (int i = 0; i < 6; i++) RC.dir[i] = i;
                RC.rotateFaceToMatchOrientation(RC.dir, RC.rc, 0);
                debugLog = "";
                cout << "The game has been reset!" << endl;

                cout << endl;
                RC.ReadRubik(RC.dir, RC.rc);
                break;

            case '+': // Save moves onto a file
                fout << debugLog;
                break;

            case '1': // Solve the Rubik Cube algorithmically (and spit out a line of RC notations as a solution)
                cout << "Scrambling the cube in a predetermined format..." << endl;
                RC.queueMove(RC, q);
                RC.ReadRubik(RC.dir, RC.rc);

                ResetSearch(aStarSearch, startNode);

                cout << "Solving the cube..." << endl;
                startNode.RC.copyRC(startNode.RC.rc, RC.rc);
                for(int i = 0; i < 6; i++) startNode.RC.dir[i] = RC.dir[i];

                aStarSearch.Algorithm(startNode);
                
                cout << endl;
                break;

            case '3':
                ResetSearch(aStarSearch, startNode);

                cout << "Solving the cube..." << endl;
                startNode.RC.copyRC(startNode.RC.rc, RC.rc);
                for (int i = 0; i < 6; i++) startNode.RC.dir[i] = RC.dir[i];

                aStarSearch.Algorithm(startNode);

                cout << endl;
                break;

            default:
                // This might be a bit confusing so lets break it down first [insert_dance_emoji]
                // This is a small side dish of code to interpreting what kind of moves
                // We already have the move we want to execute,
                // we just need to know if its a prime one or a double move or a simple move.
                if (move == ' ' || move == 'NULL') continue;

                if (move == 39)
                {
                    RC.actionRubik(RC.dir, RC.rc, oldMove, 1);
                    RC.actionRubik(RC.dir, RC.rc, oldMove, 1);
                }
                else if (move == '2')
                {
                    RC.actionRubik(RC.dir, RC.rc, oldMove, 0);
                }
                else
                {
                    RC.actionRubik(RC.dir, RC.rc, move, 0);
                }

                oldMove = move;

                cout << endl;
                RC.ReadRubik(RC.dir, RC.rc);
                break;
            }
    }

    fout.close();
    return 0;
}