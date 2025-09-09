#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <cmath>
#include <fstream>
#include <Windows.h>
#include <cstring>

#include "RubikCube.h" // Rubik Cube Simulator
#include "Webcam.h" // Webcam for Scanning Colored Squares
#include "AStarSearch.h" // A* Algorithm for solving the cube
#include "PuzzleState.h" // This is for hashing the cube in a way the A* understands and solves
#include "SerialPort.h" // Required for communication between Host and Arduino Uno
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

void ResetSearch(AStarSearch astarsearch, PuzzleState startnode)
{
    astarsearch = AStarSearch();
    startnode = PuzzleState();
}

void RubikCubeSimulator(RubikCube RC)
{
    SerialPort Uno;
    WCHAR Port[] = L"\\\\.\\COM5";

    bool play = true, syncMovesWithRealRC = false;
    char input[256];

    char q[9][256] = {"B F D B F2 U' D2"};

    while (play)
    {
        cout << endl << "Choose a move!" << endl; // ('0' Stop | '-'  Sync with Real Rubik | '+' Solve Algorithmetically the Rubik)"<<endl;
        cin.getline(input, 256);
        int i = 0;

        switch (input[0]) // Settings beforehand
        {
        case '0': // Stop the Simulator
            cout << endl << "Ending the simulator... (BYE!!!)" << endl;
            play = false; 
            i++;
            break;

        case '-': // Repeat (or stop repeating) moves to Arduino Uno throught serial port
            syncMovesWithRealRC != syncMovesWithRealRC;

            if (syncMovesWithRealRC)
            {
                Uno.Initialize(Port, 9600);
                if (!Uno.isConnected())
                {
                    cout << endl << "Failed to connect... Retry the command again...?" << endl;
                    syncMovesWithRealRC = false;
                }
            }
            else Uno.disconnect();

            cout << endl << "Setting -> Repeat RC Moves to Uno: " << syncMovesWithRealRC<<endl;
            i++;
            break;

        case '+': // Solve current Rubik Cube

            i++;
            break;

        default:
            if (input[0] > 0 && input[0] <= 9)
            {
                cout << endl << "Scrambling the code in a predetermined pattern: " << q[input[0]] << endl;
                strcpy(input, q[input[0]]);
                i++;
            }
            break;
        }

        // Now the algorithm for interpreting the actions
        int specialMove = 0;
        for (; i < strlen(input); i++)
        {
            if (input[i] == ' ' || input[i] == '\0') continue;

            if (i + 1 < strlen(input))
            {
                if (input[i + 1] == 39)
                {
                    specialMove++;
                    RC.actionRubik(RC.dir, RC.rc, input[i], true, false);
                }
                
                if (input[i + 1] == '2')
                {
                    specialMove++;
                    RC.actionRubik(RC.dir, RC.rc, input[i], false, true);
                }
            }

            if (specialMove == 1) i++;
            else RC.actionRubik(RC.dir, RC.rc, input[i], false, false);
        }

        RC.ReadRubik(RC.dir, RC.rc); 

        if (syncMovesWithRealRC)
        {
            Uno.transmit((unsigned char*)input, 8);
        }
    }
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

    if (RubikGame)
    {
        RC.rotateFaceToMatchOrientation(RC.dir, RC.rc, 0);
        RC.ReadRubik(RC.dir, RC.rc);
        RubikCubeSimulator(RC); // Start simulating a Rubik Cube in console
    }

    return 0;
}