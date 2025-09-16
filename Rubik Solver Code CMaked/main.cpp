#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cmath>
#include <fstream>
#include <Windows.h>
#include <cstring>

#include "RubikCube.h" // Rubik Cube Simulator
#include "AStarSearch.h" // A* Algorithm for solving the cube
#include "PuzzleState.h" // This is for hashing the cube in a way the A* understands and solves
#include "main.h"

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

void ResetSearch(AStarSearch astarsearch, PuzzleState startnode)
{
	astarsearch = AStarSearch();
	startnode = PuzzleState();
}

void RubikCubeSimulator(RubikCube RC)
{
	AStarSearch algo;
	PuzzleState startnode;

	bool play = true, syncMovesWithRealRC = false;
	char input[256];

	char q[9][256] = { "B F D B F2 U' D2", "L U2 B' L' D' B' L2 B U2 R U' B2 L' R F2 B2 U D2 B R' B2 R' U2 R' L' " };

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

		case '+': // Solve current Rubik Cube
			cout << endl << "Initializing the solver..." << endl;
			ResetSearch(algo, startnode);
			startnode.RC.copyRC(startnode.RC.rc, RC.rc);

			RC.copyRC(RC.rc, algo.SearchSolution(startnode).RC.rc);

			i++;
			break;

		default:
			if (input[0] > '0' && input[0] <= '9')
			{
				cout << endl << "Scrambling the code in a predetermined pattern: " << q[input[0] - '0' - 1] << endl;
				strcpy(input, q[input[0] - '0' - 1]);
				i++;
			}
			break;
		}

		// Now the algorithm for interpreting the actions
		RC.queueMove(RC, input, i);
		RC.ReadRubik(RC.dir, RC.rc);
	}
}

int main()
{
	RubikCube RC;

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

	RC.rotateFaceToMatchOrientation(RC.dir, RC.rc, 0);
	RC.ReadRubik(RC.dir, RC.rc);
	RubikCubeSimulator(RC); // Start simulating a Rubik Cube in console


	return 0;
}