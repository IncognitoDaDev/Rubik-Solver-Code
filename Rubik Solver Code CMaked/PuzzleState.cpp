#include "PuzzleState.h"
#include "RubikCube.h"
#include "AStarSearch.h"
#include <string>
#include <iostream>

enum TypeOfPiece
{
	Center = 0,
	Edge = 1,
	Corner = 2
};

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

int PuzzleState::HashPiece(int typeOfPiece, int colors, int orientations)
{
	return typeOfPiece * 1000000 + orientations * 1000 + colors;
}

int PuzzleState::PieceID(RubikCube& nodeGoal, int typeOfPiece, int* coords)
{
	int ID = 0, p = 1;
	for (int i = 0; i < typeOfPiece + 1; i++)
	{
		ID += nodeGoal.rc[nodeGoal.dir[coords[i] / 100]][coords[i] / 10 % 10][coords[i] % 10] * p;
		p *= 10;
	}

	return ID;
}

// We are hashing the Rubik Cube into a properly 3D Matrix where edges gain two digits and corners three digits (Front -> Up/Bottom -> Right/Left)
void PuzzleState::HashRC(RubikCube& nodeGoal, int(&hash)[3][3][3])
{
	int colorID[3];
	hash[1][1][1] = -1; // Free Space!

	//Front ------
	colorID[0] = Front * 100 + 11;
	hash[2][1][1] = HashPiece(Center, PieceID(nodeGoal, Center, colorID), Front);

	// Corners
	colorID[0] = Front * 100 + 0; colorID[1] = Up * 100 + 20; colorID[2] = Left * 100 + 2;
	hash[2][0][0] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 310);
	colorID[0] = Front * 100 + 2; colorID[1] = Up * 100 + 22; colorID[2] = Right * 100 + 0;
	hash[2][0][2] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 410);
	colorID[0] = Front * 100 + 20; colorID[1] = Bottom * 100 + 0; colorID[2] = Left * 100 + 22;
	hash[2][2][0] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 350);
	colorID[0] = Front * 100 + 22; colorID[1] = Bottom * 100 + 2; colorID[2] = Right * 100 + 20;
	hash[2][2][2] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 450);

	// Edge   
	colorID[0] = Front * 100 + 1; colorID[1] = Up * 100 + 21;
	hash[2][0][1] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 10);
	colorID[0] = Front * 100 + 12; colorID[1] = Right * 100 + 10;
	hash[2][1][2] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 40);
	colorID[0] = Front * 100 + 21; colorID[1] = Bottom * 100 + 1;
	hash[2][2][1] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 50);
	colorID[0] = Front * 100 + 10; colorID[1] = Left * 100 + 12;
	hash[2][1][0] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 30);


	//Back ------
	colorID[0] = Back * 100 + 11;
	hash[0][1][1] = HashPiece(Center, PieceID(nodeGoal, Center, colorID), Back);

	// Corners
	colorID[0] = Back * 100 + 2; colorID[1] = Up * 100 + 0; colorID[2] = Left * 100 + 0;
	hash[0][0][0] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 312);
	colorID[0] = Back * 100 + 0; colorID[1] = Up * 100 + 2; colorID[2] = Right * 100 + 2;
	hash[0][0][2] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 412);
	colorID[0] = Back * 100 + 22; colorID[1] = Bottom * 100 + 20; colorID[2] = Left * 100 + 20;
	hash[0][2][0] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 352);
	colorID[0] = Back * 100 + 20; colorID[1] = Bottom * 100 + 22; colorID[2] = Right * 100 + 22;
	hash[0][2][2] = HashPiece(Corner, PieceID(nodeGoal, Corner, colorID), 452);

	// Edges
	colorID[0] = Back * 100 + 1; colorID[1] = Up * 100 + 1;
	hash[0][0][1] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 12);
	colorID[0] = Back * 100 + 10; colorID[1] = Right * 100 + 12;
	hash[0][1][2] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 42);
	colorID[0] = Back * 100 + 21; colorID[1] = Bottom * 100 + 21;
	hash[0][2][1] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 52);
	colorID[0] = Back * 100 + 12; colorID[1] = Left * 100 + 10;
	hash[0][1][0] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 32);


	// Right ------
	colorID[0] = Right * 100 + 11;
	hash[1][1][2] = HashPiece(Center, PieceID(nodeGoal, Center, colorID), Right);

	// Edges
	colorID[0] = Right * 100 + 1; colorID[1] = Up * 100 + 12;
	hash[1][0][2] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 14);
	colorID[0] = Right * 100 + 21; colorID[1] = Bottom * 100 + 12;
	hash[1][2][2] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 54);


	// Left ------
	colorID[0] = Left * 100 + 11;
	hash[1][1][0] = HashPiece(Center, PieceID(nodeGoal, Center, colorID), Left);

	// Edges
	colorID[0] = Left * 100 + 1; colorID[1] = Up * 100 + 10;
	hash[1][0][0] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 13);
	colorID[0] = Left * 100 + 21; colorID[1] = Bottom * 100 + 10;
	hash[1][2][0] = HashPiece(Edge, PieceID(nodeGoal, Edge, colorID), 53);

	// Up ------
	colorID[0] = Up * 100 + 11;
	hash[1][0][1] = HashPiece(Center, PieceID(nodeGoal, Center, colorID), Up);

	// Bottom ------
	colorID[0] = Bottom * 100 + 11;
	hash[1][2][1] = HashPiece(Center, PieceID(nodeGoal, Center, colorID), Bottom);
}

void PuzzleState::DehashRC(int(&hash)[3][3][3], int(&rc)[6][3][3])
{
	//Front ------
	rc[Front][1][1] = hash[2][1][1]%10;

	// Corners
	rc[Front][0][0] = hash[2][0][0] % 10; rc[Up][2][0] = hash[2][0][0] / 10 % 10; rc[Left][0][2] = hash[2][0][0] / 100 % 10;
	rc[Front][0][2] = hash[2][0][2] % 10; rc[Up][2][2] = hash[2][0][2] / 10 % 10; rc[Right][0][0] = hash[2][0][2] / 100 % 10;
	rc[Front][2][0] = hash[2][2][0] % 10; rc[Bottom][0][0] = hash[2][2][0] / 10 % 10; rc[Left][2][2] = hash[2][2][0] / 100 % 10;
	rc[Front][2][2] = hash[2][2][2] % 10; rc[Bottom][0][2] = hash[2][2][2] / 10 % 10; rc[Right][2][0] = hash[2][2][2] / 100 % 10;

	// Edge
	rc[Front][0][1] = hash[2][0][1] % 10; rc[Up][2][1] = hash[2][0][1] / 10 % 10;
	rc[Front][1][2] = hash[2][1][2] % 10; rc[Right][1][0] = hash[2][1][2] / 10 % 10;
	rc[Front][2][1] = hash[2][2][1] % 10; rc[Bottom][0][1] = hash[2][2][1] / 10 % 10;
	rc[Front][1][0] = hash[2][1][0] % 10; rc[Left][1][2] = hash[2][1][0] / 10 % 10;


	//Back ------
	rc[Back][1][1] = hash[0][1][1] % 10;

	// Corners
	rc[Back][0][2] = hash[0][0][0] % 10; rc[Up][0][0] = hash[0][0][0] / 10 % 10; rc[Left][0][0] = hash[0][0][0] / 100 % 10;
	rc[Back][0][0] = hash[0][0][2] % 10; rc[Up][0][2] = hash[0][0][2] / 10 % 10; rc[Right][0][2] = hash[0][0][2] / 100 % 10;
	rc[Back][2][2] = hash[0][2][0] % 10; rc[Bottom][2][0] = hash[0][2][0] / 10 % 10; rc[Left][2][0] = hash[0][2][0] / 100 % 10;
	rc[Back][2][0] = hash[0][2][2] % 10; rc[Bottom][2][2] = hash[0][2][2] / 10 % 10; rc[Right][2][2] = hash[0][2][2] / 100 % 10;

	// Edge
	rc[Back][0][1] = hash[0][0][1] % 10; rc[Up][0][1] = hash[0][0][1] / 10 % 10;
	rc[Back][1][0] = hash[0][1][2] % 10; rc[Right][1][2] = hash[0][1][2] / 10 % 10;
	rc[Back][2][1] = hash[0][2][1] % 10; rc[Bottom][2][1] = hash[0][2][1] / 10 % 10;
	rc[Back][1][2] = hash[0][1][0] % 10; rc[Left][1][0] = hash[0][1][0] / 10 % 10;


	// Right ------
	rc[Right][1][1] = hash[1][1][2] % 10;

	// Edges
	rc[Right][0][1] = hash[1][0][2] % 10; rc[Up][1][2] = hash[1][0][2] / 10 % 10;
	rc[Right][2][1] = hash[1][2][2] % 10; rc[Bottom][1][2] = hash[1][2][2] / 10 % 10;


	// Left ------
	rc[Left][1][1] = hash[1][1][0] % 10;

	// Edges
	rc[Left][0][1] = hash[1][0][0] % 10; rc[Up][1][0] = hash[1][0][0] / 10 % 10;
	rc[Left][2][1] = hash[1][2][0] % 10; rc[Bottom][1][0] = hash[1][2][0] / 10 % 10;

	// Up ------
	rc[Up][1][1] = hash[1][0][1] % 10;

	// Bottom ------
	rc[Bottom][1][1] = hash[1][2][1] % 10;
}

void PuzzleState::DesiredCoords(int hash[3][3][3], int pieceID, int& zyx)
{
	zyx = 0;

	int faces[6] = { hash[2][1][1] % 10, hash[1][0][1] % 10, hash[0][1][1] % 10, hash[1][1][0] % 10, hash[1][1][2] % 10, hash[1][2][1] % 10 };
	int color[3];

	switch (pieceID / 1000000)
	{
	case Edge:
		for (int i = 0; i < 6; i++)
		{
			if (faces[i] == pieceID % 10) color[0] = i;
			if (faces[i] == pieceID / 10 % 10) color[1] = i;
		}
		switch (color[1])
		{
		case Front:
			switch (color[0])
			{
			case Up: zyx = 201; break;
			case Bottom: zyx = 221; break;
			case Left: zyx = 210; break;
			case Right: zyx = 212; break;
			}
			break;

		case Back:
			switch (color[0])
			{
			case Up: zyx = 1; break;
			case Bottom: zyx = 21; break;
			case Left: zyx = 10; break;
			case Right: zyx = 12; break;
			}
			break;

		case Up:
			switch (color[0])
			{
			case Front: zyx = 201; break;
			case Back: zyx = 1; break;
			case Left: zyx = 100; break;
			case Right: zyx = 102; break;
			}
			break;

		case Bottom:
			switch (color[0])
			{
			case Front: zyx = 221; break;
			case Back: zyx = 21; break;
			case Left: zyx = 120; break;
			case Right: zyx = 122; break;
			}
			break;

		case Right:
			switch (color[0])
			{
			case Up: zyx = 102; break;
			case Bottom: zyx = 122; break;
			case Front: zyx = 212; break;
			case Back: zyx = 12; break;
			}
			break;

		case Left:
			switch (color[0])
			{
			case Up: zyx = 100; break;
			case Bottom: zyx = 120; break;
			case Front: zyx = 210; break;
			case Back: zyx = 10; break;
			}
			break;
		}
		break;

	case Corner:
		for (int i = 0; i < 6; i++)
		{
			if (faces[i] == pieceID % 10) color[0] = i;
			if (faces[i] == pieceID / 10 % 10) color[1] = i;
			if (faces[i] == pieceID / 100 % 10) color[2] = i;
		}
		switch (color[2])
		{
		case Front:
			switch (color[1])
			{
			case Bottom:
				switch (color[0])
				{
				case Left: zyx = 222; break;
				case Right: zyx = 220; break;
				}
				break;

			case Up:
				switch (color[0])
				{
				case Left: zyx = 200; break;
				case Right: zyx = 202; break;
				}
				break;

			case Left:
				switch (color[0])
				{
				case Up: zyx = 200; break;
				case Bottom: zyx = 220; break;
				}
				break;

			case Right:
				switch (color[0])
				{
				case Up: zyx = 202; break;
				case Bottom: zyx = 222; break;
				}
				break;
			}
			break;

		case Back:
			switch (color[1])
			{
			case Bottom:
				switch (color[0])
				{
				case Left: zyx = 20; break;
				case Right: zyx = 22; break;
				}
				break;

			case Up:
				switch (color[0])
				{
				case Left: zyx = 0; break;
				case Right: zyx = 2; break;
				}
				break;

			case Left:
				switch (color[0])
				{
				case Up: zyx = 0; break;
				case Bottom: zyx = 20; break;
				}
				break;

			case Right:
				switch (color[0])
				{
				case Up: zyx = 2; break;
				case Bottom: zyx = 22; break;
				}
				break;
			}
			break;

		case Up:
			switch (color[1])
			{
			case Front:
				switch (color[0])
				{
				case Left: zyx = 202; break;
				case Right: zyx = 200; break;
				}
				break;

			case Back:
				switch (color[0])
				{
				case Left: zyx = 2; break;
				case Right: zyx = 0; break;
				}
				break;

			case Left:
				switch (color[0])
				{
				case Front: zyx = 200; break;
				case Back: zyx = 0; break;
				}
				break;

			case Right:
				switch (color[0])
				{
				case Front: zyx = 202; break;
				case Back: zyx = 2; break;
				}
				break;
			}
			break;

		case Bottom:
			switch (color[1])
			{
			case Front:
				switch (color[0])
				{
				case Left: zyx = 220; break;
				case Right: zyx = 222; break;
				}
				break;

			case Back:
				switch (color[0])
				{
				case Left: zyx = 20; break;
				case Right: zyx = 22; break;
				}
				break;

			case Left:
				switch (color[0])
				{
				case Front: zyx = 220; break;
				case Back: zyx = 20; break;
				}
				break;

			case Right:
				switch (color[0])
				{
				case Front: zyx = 222; break;
				case Back: zyx = 22; break;
				}
				break;
			}
			break;

		case Right:
			switch (color[1])
			{
			case Front:
				switch (color[0])
				{
				case Up: zyx = 202; break;
				case Bottom: zyx = 222; break;
				}
				break;

			case Back:
				switch (color[0])
				{
				case Up: zyx = 2; break;
				case Bottom: zyx = 22; break;
				}
				break;

			case Up:
				switch (color[0])
				{
				case Front: zyx = 202; break;
				case Back: zyx = 2; break;
				}
				break;

			case Bottom:
				switch (color[0])
				{
				case Front: zyx = 222; break;
				case Back: zyx = 22; break;
				}
				break;
			}
			break;

		case Left:
			switch (color[1])
			{
			case Front:
				switch (color[0])
				{
				case Up: zyx = 200; break;
				case Bottom: zyx = 220; break;
				}
				break;

			case Back:
				switch (color[0])
				{
				case Up: zyx = 0; break;
				case Bottom: zyx = 20; break;
				}
				break;

			case Up:
				switch (color[0])
				{
				case Front: zyx = 200; break;
				case Back: zyx = 0; break;
				}
				break;

			case Bottom:
				switch (color[0])
				{
				case Front: zyx = 220; break;
				case Back: zyx = 20; break;
				}
				break;
			}
			break;
		}
		break;

	default: zyx = -1; break;
	}
}

void PuzzleState::DebugHash(int(&hash)[3][3][3])
{
	for (int i = 0; i < 3; i++)
	{
		std::cout << "\nZ: " << i << std::endl;
		for (int j = 0; j < 9; j++)
			std::cout << hash[i][j / 3][j % 3] << ((j + 1) % 3 == 0 ? "\n" : " ");
	}
}

int PuzzleState::distCenter(int face, int pieceFace)
{
	if (pieceFace == face) return 0;

	switch (pieceFace)
	{
	case Front:
		if (face == Back) return 2;
		break;
	case Back:
		if (face == Front) return 2;
		break;
	case Up:
		if (face == Bottom) return 2;
		break;
	case Bottom:
		if (face == Up) return 2;
		break;
	case Right:
		if (face == Left) return 2;
		break;
	case Left:
		if (face == Right) return 2;
		break;
	}

	return 1;
}

bool PuzzleState::IsSameState(PuzzleState& rhs)
{
	int hash[3][3][3], rhs_hash[3][3][3];
	HashRC(RC, hash);
	HashRC(rhs.RC, rhs_hash);

	for (int z = 0; z < 3; z++)
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 3; x++)
				if (hash[z][y][x] != rhs_hash[z][y][x]) return false;

	return true;
}

void PuzzleState::copyMoves(PuzzleState& rhs)
{
	for (int i = 0; i < rhs.moves.size(); i++)
		moves.push_back(rhs.moves[i]);
}

std::string PuzzleState::returnSolution()
{
	std::string a = "";
	for (int i = moves.size() - 1; i < moves.size(); i--)
		switch (moves[i])
		{
		case 0: a += "U "; break;
		case 1: a += "D "; break;
		case 2: a += "F "; break;
		case 3: a += "B "; break;
		case 4: a += "R "; break;
		case 5: a += "L "; break;
		case 6: a += "U' "; break;
		case 7: a += "D' "; break;
		case 8: a += "F' "; break;
		case 9: a += "B' "; break;
		case 10: a += "R' "; break;
		case 11: a += "L' "; break;
		}

	return a;
}

float PuzzleState::GoalDistanceEstimate(PuzzleState& nodeGoal)
{
	for (int z = 0; z < 3; z++)
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 3; x++)
					hash[z][y][x] = rhs[z][y][x];
}

float PuzzleState::GoalDistanceEstimate(PuzzleState& nodeGoal)
{
	// Returns the cost that is the sum of moves for pieces to be positioned properly plus the orientation

	// Return the estimated cost to goal from this node
	float cost = 0;

	int hash[3][3][3], goal_hash[3][3][3];
	RubikCube newGoal;

	// Creating the corect result from a persective point (accounting for color placement)
	for (int f = 0; f < 6; f++)
	{
		for (int i = 0; i < 9; i++)
		{
			newGoal.rc[f][i / 3][i % 3] = nodeGoal.RC.rc[f][1][1];
		}
	}
	HashRC(newGoal, goal_hash);
	HashRC(nodeGoal.RC, hash);

	//					F    U    B   L    D    R
	int centers[6] = { 211, 101, 11, 110, 112, 121 };

	int corners[8] = { 0, 2, 20, 22, 200, 202, 220, 222 };
	int edges[12] = { 1, 10, 12, 21, 100, 102, 120, 122, 201, 210, 212, 221 };
	int zyx = 0;

	/*The obvious heuristic for Rubik's Cube is a three dimensional version of the Manhattan distance.
		For each cubie, compute the minimum number of moves required to correctly position and orient it,
		and sum these values over all cubies.Unfortunately, to be admissible, this value has to be divided by 8,
		since every twist moves 8 cubies. A better heuristic is to take the maximum of the sum of Manhattan distances
		of the corner cubies, divided by four, and the maximum of the sum of edge cubies divided by 4.
		The expected value of the Manhattan distance of the edge cubies is 22/4=5.5, while the corresponding
		values for the corner cubies is 12.333/4 that's approximately equal to 3.08 partly
		because there are 12 edge cubies, but only eight corner cubes.*/

	for (int corner : corners)
	{
		DesiredCoords(hash, hash[corner / 100][corner / 10 % 10][corner % 10], zyx);

		int id = hash[corner / 100][corner / 10 % 10][corner % 10];
		int	desiredid = goal_hash[zyx / 100][zyx / 10 % 10][zyx % 10];

		float calc = abs(float(corner / 100) - float(zyx / 100)) + abs(float(corner / 10 % 10) - float(zyx / 10 % 10)) + abs(float(corner % 10) - float(zyx % 10));

		int p = 1; float distOrient = 0;
		// Check for orientation on every color
		for (int i = 0; i < 3; i++)
		{
			int f = 0;
			for (int i = 0; i < 6; i++)
			{
				if (id / p % 10 == hash[centers[i] / 100][centers[i] / 10 % 10][centers[i] % 10] % 10)
				{
					f = i;
					break;
				}
			}

			switch (distCenter(f, id / 1000 / p % 10))
			{
			case 0: break; // The color is positioned properly
			case 1:
				if (calc == 0 || calc == 4) distOrient += 2;
				else distOrient += 3;
				break;
			case 2:
				distOrient += 3;
				break;
			}

			p *= 10;
		}
		cost += distOrient / 8.0f;
	}

	float maxDistEdge = 0;
	for (int edge : edges)
	{
		DesiredCoords(hash, hash[edge / 100][edge / 10 % 10][edge % 10], zyx);
		int id = hash[edge / 100][edge / 10 % 10][edge % 10];
		int desiredid = goal_hash[zyx / 100][zyx / 10 % 10][zyx % 10];

		float calc = abs(float(edge / 100) - float(zyx / 100)) + abs(float(edge / 10 % 10) - float(zyx / 10 % 10)) + abs(float(edge % 10) - float(zyx % 10));

		int p = 1; float distOrient = 0;
		// Check for orientation on every color
		for (int i = 0; i < 2; i++)
		{
			int f = 0;
			for (int i = 0; i < 6; i++)
			{
				if (id / p % 10 == hash[centers[i] / 100][centers[i] / 10 % 10][centers[i] % 10] % 10)
				{
					f = i;
					break;
				}
			}

			switch (distCenter(f, id / 1000 / p % 10))
			{
			case 0: break; // The color is positioned properly
			case 1:
				switch ((int)calc)
				{
				case 0:
					distOrient += 3;
					break;
				case 2: // It means that our correct position is just across the face
					if (zyx / 100 == edge / 100 && zyx / 10 % 10 == edge / 10 % 10 || zyx / 10 % 10 == edge / 10 % 10 && zyx % 10 == edge % 10)
						distOrient += 3; //Our correct position is at the opposite side of the face
					else distOrient += 2; // Our correct position is adjancent
					break;
				case 4:
					distOrient += 2;
					break;
				}
				break;
			case 2:
				if (zyx / 100 == edge / 100 && zyx / 10 % 10 == edge / 10 % 10 || zyx / 10 % 10 == edge / 10 % 10 && zyx % 10 == edge % 10)
					distOrient += 2;
				else distOrient += 3; // Our correct position is adjancent on the opposite side of the cube
				break;
			}

			p *= 10;
		}
		cost += distOrient / 8.0f;

	}

	return cost;
}

bool PuzzleState::IsGoal(PuzzleState& nodeGoal)
{
	int hash[3][3][3], goal_hash[3][3][3];
	RubikCube newGoal;

	// Creating the corect result from a persective point (accounting for color placement)
	for (int f = 0; f < 6; f++)
	{
		for (int i = 0; i < 9; i++)
		{
			newGoal.rc[f][i / 3][i % 3] = nodeGoal.RC.rc[f][1][1];
		}
	}

	HashRC(nodeGoal.RC, hash);
	HashRC(newGoal, goal_hash);

	for (int z = 0; z < 3; z++)
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 3; x++)
				if (hash[z][y][x] != goal_hash[z][y][x]) return false;

	return true;
}

void PuzzleState::SumCost(PuzzleState& nodeGoal, PuzzleState nodeParent)
{
	nodeGoal.g = nodeParent.g + 1.0f;
	nodeGoal.h = GoalDistanceEstimate(nodeGoal);
	nodeGoal.f = g + h;
}

float PuzzleState::CA_GoalDistEst(PuzzleState& nodeGoal)
{
	// Return the estimated cost to goal from this node
	float cost = 0;

	int hash[3][3][3], goal_hash[3][3][3];
	RubikCube newGoal;

	// Creating the corect result from a persective point (accounting for color placement)
	for (int f = 0; f < 6; f++)
	{
		for (int i = 0; i < 9; i++)
		{
			newGoal.rc[f][i / 3][i % 3] = nodeGoal.RC.rc[f][1][1];
		}
	}
	HashRC(newGoal, goal_hash);
	HashRC(nodeGoal.RC, hash);

void PuzzleState::SumCost(PuzzleState& nodeGoal, PuzzleState nodeParent)
{
	float nodeincrease, erosion;

	nodeincrease = 2.4f - 0.4f * nodeGoal.moves.size();
	if (nodeincrease < 1.2f) nodeincrease = 1.2f;


	nodeGoal.g = nodeParent.g + nodeincrease;
	nodeGoal.h = GoalDistanceEstimate(nodeGoal);
	nodeGoal.f = g + h;
}



