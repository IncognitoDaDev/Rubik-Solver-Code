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

int PuzzleState::HashPiece(int typeOfPiece, int colors)
{
	return typeOfPiece * 1000 + colors;
}

// We are hashing the Rubik Cube into a properly 3D Matrix where edges gain two digits and corners three digits (Front/Back -> Up/Bottom -> Right/Left)
void PuzzleState::HashRC(RubikCube& RC, int(&hash)[3][3][3])
{
	/*//for (int z = 0; z < 3; z++)
	//	for (int y = 0; y < 3; y++)
	//		for (int x = 0; x < 3; x++)
	//		{
	//			int type = 0;

	//			if (z != 1)
	//			{
	//				hash[z][y][x] = nodeGoal.rc[(z > 1 ? Front : Back)][y][(z > 1 ? x : 2 - x)];

	//				if (y == x && x == 1) continue; // Center cubie only needs one colour
	//				type++;
	//				// The cubie is an edge, so we add one more colour to it
	//				if (y == 1)
	//				{
	//					if (x > 1) hash[z][y][x] += nodeGoal.rc[Right][y][2] * 10; // If its a Right
	//					else hash[z][y][x] += nodeGoal.rc[Left][y][0] * 10; // Or Left
	//					continue;
	//				}
	//				else if (x == 1)
	//				{
	//					if (y > 1) hash[z][y][x] += nodeGoal.rc[Up][0][x] * 10; // If its Up
	//					else hash[z][y][x] += nodeGoal.rc[Bottom][2][x] * 10; // Or Bottom 
	//					continue;
	//				}

	//				type++;
	//				// The cubie is a corner, so we add two more colours to it
	//				if (y > 1) hash[z][y][x] += nodeGoal.rc[Up][0][x] * 10; // If its Up
	//				else hash[z][y][x] += nodeGoal.rc[Bottom][2][x] * 10; // Or Bottom 

	//				if (x > 1) hash[z][y][x] += nodeGoal.rc[Right][y][2] * 100; // If its a Right
	//				else hash[z][y][x] += nodeGoal.rc[Left][y][0] * 100; // Or Left
	//			}
	//			else
	//			{
	//				if (y == x && x == 1) continue;

	//				if (y == 1 && x != 1) // Center Piece
	//				{
	//					if (x > 1) hash[z][y][x] += nodeGoal.rc[Right][y][2]; // If its a Right
	//					else if (x < 1) hash[z][y][x] += nodeGoal.rc[Left][y][0]; // Or Left
	//					continue;
	//				}
	//				else if (x == 1 && y != 1)
	//				{
	//					if (y > 1) hash[z][y][x] += nodeGoal.rc[Up][0][x]; // If its Up
	//					else if (y < 1) hash[z][y][x] += nodeGoal.rc[Bottom][2][x]; // Or Bottom 
	//					continue;
	//				}

	//				type++;
	//				if (y > 1) hash[z][y][x] += nodeGoal.rc[Up][0][x]; // If its Up
	//				else if (y < 1) hash[z][y][x] += nodeGoal.rc[Bottom][2][x]; // Or Bottom 

	//				if (x > 1) hash[z][y][x] += nodeGoal.rc[Right][y][2] * 10; // If its a Right
	//				else if (x < 1) hash[z][y][x] += nodeGoal.rc[Left][y][0] * 10; // Or Left
	//			}

	//			hash[z][y][x] = type * 1000; // Declaring what type of cubie it is (center, edge or corner?)
	//		} */

	int colorID[3];
	hash[1][1][1] = 777; // Free Space!

	//Front ------
	//colorID[0] = Front * 100 + 11;
	hash[2][1][1] = HashPiece(Center, RC.rc[Front][1][1]);

	// Corners
	hash[2][0][0] = HashPiece(Corner, RC.rc[Front][0][0] + RC.rc[Up][2][0] * 10 + RC.rc[Left][0][2] * 100);
	hash[2][0][2] = HashPiece(Corner, RC.rc[Front][0][2] + RC.rc[Up][2][2] * 10 + RC.rc[Right][0][0] * 100);

	hash[2][2][0] = HashPiece(Corner, RC.rc[Front][2][0] + RC.rc[Bottom][0][0] * 10 + RC.rc[Left][2][2] * 100);
	hash[2][2][2] = HashPiece(Corner, RC.rc[Front][2][2] + RC.rc[Bottom][0][2] * 10 + RC.rc[Right][2][0] * 100);

	// Edge
	hash[2][0][1] = HashPiece(Edge, RC.rc[Front][0][1] + RC.rc[Up][2][1] * 10);
	hash[2][2][1] = HashPiece(Edge, RC.rc[Front][2][1] + RC.rc[Bottom][0][1] * 10);

	hash[2][1][2] = HashPiece(Edge, RC.rc[Front][1][2] + RC.rc[Right][1][0] * 10);
	hash[2][1][0] = HashPiece(Edge, RC.rc[Front][1][0] + RC.rc[Left][1][2] * 10);


	//Back ------
	colorID[0] = Back * 100 + 11;
	hash[0][1][1] = HashPiece(Center, RC.rc[Back][1][1]);

	// Corners
	hash[0][0][0] = HashPiece(Corner, RC.rc[Back][0][2] + RC.rc[Up][0][0] * 10 + RC.rc[Left][0][0] * 100);
	hash[0][0][2] = HashPiece(Corner, RC.rc[Back][0][0] + RC.rc[Up][0][2] * 10 + RC.rc[Right][0][2] * 100);

	hash[0][2][0] = HashPiece(Corner, RC.rc[Back][2][2] + RC.rc[Bottom][2][0] * 10 + RC.rc[Left][2][0] * 100);
	hash[0][2][2] = HashPiece(Corner, RC.rc[Back][2][0] + RC.rc[Bottom][2][2] * 10 + RC.rc[Right][2][2] * 100);

	// Edges
	hash[0][0][1] = HashPiece(Edge, RC.rc[Back][0][1] + RC.rc[Up][0][1] * 10);
	hash[0][2][1] = HashPiece(Edge, RC.rc[Back][2][1] + RC.rc[Bottom][2][1] * 10);

	hash[0][1][2] = HashPiece(Edge, RC.rc[Back][1][0] + RC.rc[Right][1][2] * 10);
	hash[0][1][0] = HashPiece(Edge, RC.rc[Back][1][2] + RC.rc[Left][1][0] * 10);


	// Right ------
	hash[1][1][2] = HashPiece(Center, RC.rc[Right][1][1]);

	// Edges
	hash[1][0][2] = HashPiece(Edge, RC.rc[Right][0][1] + RC.rc[Up][1][2] * 10);
	hash[1][2][2] = HashPiece(Edge, RC.rc[Right][2][1] + RC.rc[Bottom][1][2] * 10);


	// Left ------
	hash[1][1][0] = HashPiece(Center, RC.rc[Left][1][1]);

	// Edges
	hash[1][0][0] = HashPiece(Edge, RC.rc[Left][0][1] + RC.rc[Up][1][0] * 10);
	hash[1][2][0] = HashPiece(Edge, RC.rc[Left][2][1] + RC.rc[Bottom][1][0] * 10);

	// Up ------
	hash[1][0][1] = HashPiece(Center, RC.rc[Up][1][1]);

	// Bottom ------
	hash[1][2][1] = HashPiece(Center, RC.rc[Bottom][1][1]);
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

	switch (pieceID / 1000)
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


// What is a G1 state? Extending from Kociemba's definition, its a state where the rubik can be solved by using
// only U D F2 B2 R2 L2 moves. Its a state where every cubie is oriented and edges in UD slice are in their slice.
// Looking further into what makes a cubie an oriented cubie... I understood the logic behind it this way
// Corner cubies should be facing Up/Down, matching the color of either face (Up/Down)
// Edge cubies should be oriented as explained in ZZ guide, Eoline section

int PuzzleState::GoalDistanceEstimate_G1(RubikCube node, int stage)
{
	// Return the estimated cost to goal from this node
	int cost = 0;

	int corners[8] = { 0, 2, 20, 22, 200, 202, 220, 222 };
	int edges[8] = { 1, 100, 102, 201, 21, 120, 122, 221 }; // Bottom layer edges
	int edgesUD[4] = { 10, 12, 210, 212 }; // Middle layer edges

	int zyx = 0;

	int hash[3][3][3];
	HashRC(node, hash);

	switch (stage)
	{
	case 0: // Edge orientation
		for (int edge : edges)
		{
			int id = hash[edge / 100][edge / 10 % 10][edge % 10];
			DesiredCoords(hash, id, zyx);

			// Checking for Orientation
			if (id / 10 % 10 == hash[1][1][0] % 10 || id / 10 % 10 == hash[1][1][2] % 10) 
				cost += abs(edge / 100 - zyx / 100) + abs(edge / 10 % 10 - zyx / 10 % 10) + abs(edge % 10 - zyx % 10) + 1;; // Bad orientation!
			if (id / 10 % 10 == hash[2][1][1] % 10 || id / 10 % 10 == hash[0][1][1] % 10)
			{ // Questionable Orientation, checking second color's
				if (id % 10 == hash[1][0][1] || id % 10 == hash[1][2][1]) 
					cost += abs(edge / 100 - zyx / 100) + abs(edge / 10 % 10 - zyx / 10 % 10) + abs(edge % 10 - zyx % 10) + 1; //Bad orientation!
			}
		}

		for (int edge : edgesUD)
		{
			int id = hash[edge / 100][edge / 10 % 10][edge % 10];

			// Checking for Orientation
			if (id % 10 == hash[1][1][0] % 10 || id % 10 == hash[1][1][2] % 10) 
				cost += abs(edge / 100 - zyx / 100) + abs(edge / 10 % 10 - zyx / 10 % 10) + abs(edge % 10 - zyx % 10) + 1; // Bad orientation!
			if (id % 10 == hash[2][1][1] % 10 || id % 10 == hash[0][1][1] % 10)
			{ // Questionable Orientation, checking second color's
				if (id / 10 % 10 == hash[1][0][1] || id / 10 % 10 == hash[1][2][1]) 
					cost += abs(edge / 100 - zyx / 100) + abs(edge / 10 % 10 - zyx / 10 % 10) + abs(edge % 10 - zyx % 10) + 1; //Bad orientation!
			}
		}
		break;

	case 1: // Corner & eUD
		// Corner orientation
		for(int i = 0; i < 9; i += 2)
			{
			if (!(node.rc[Up][i/3][i%3] == node.rc[Up][1][1] || node.rc[Up][i/3][i%3] == node.rc[Bottom][1][1]))
				{
					int id = hash[i/3][0][i%3];
					DesiredCoords(hash, id, zyx);

					cost += abs(i/3 - zyx / 100) + abs(0 - zyx / 10 % 10) + abs(i%3 - zyx % 10) + 1;
				}

				if (!(node.rc[Bottom][i/3][i%3] == node.rc[Bottom][1][1] || node.rc[Bottom][i/3][i%3] == node.rc[Up][1][1]))
				{
					int id = hash[2 - i/3][2][i%3];
					DesiredCoords(hash, id, zyx);

					cost += abs(2 - i/3 - zyx / 100) + abs(2 - zyx / 10 % 10) + abs(i%3 - zyx % 10) + 1;
				}
			}

		for (int edge : edgesUD)
		{
			int id = hash[edge / 100][edge / 10 % 10][edge % 10];

			// Checking for Orientation
			if (id % 10 == hash[1][1][0] % 10 || id % 10 == hash[1][1][2] % 10)
				cost += abs(edge / 100 - zyx / 100) + abs(edge / 10 % 10 - zyx / 10 % 10) + abs(edge % 10 - zyx % 10) + 1; // Bad orientation!
			if (id % 10 == hash[2][1][1] % 10 || id % 10 == hash[0][1][1] % 10)
			{ // Questionable Orientation, checking second color's
				if (id / 10 % 10 == hash[1][0][1] || id / 10 % 10 == hash[1][2][1])
					cost += abs(edge / 100 - zyx / 100) + abs(edge / 10 % 10 - zyx / 10 % 10) + abs(edge % 10 - zyx % 10) + 1; //Bad orientation!
			}
		}



		//for (int corner : corners)
		//{
		//	int id = hash[corner / 100][corner / 10 % 10][corner % 10];
		//	if ((id / 10 % 10 != hash[1][0][1] % 10) && (id / 10 % 10 != hash[1][2][1] % 10)) cost += 5;

		//	//if (id % 10 != hash[1][0][1] % 10 && id % 10 != hash[1][2][1] % 10) cost += 5;
		//}

		//for (int edge : edgesUD)
		//{

		//}

		/*int badEdge = 0;
		for (int eUD : edgesUD)
		{
			int id = hash[eUD / 100][eUD / 10 % 10][eUD % 10];
			DesiredCoords(hash, id, zyx);

			int isEdge = 0;
			for (int check : edgesUD)
			{
				if (zyx == check) isEdge = 1;
			}

			if (isEdge != 1) badEdge++;
		}
		cost += badEdge * 5;*/

		break;

	}

	return cost;
}

int PuzzleState::GoalDistanceEstimate_Solve(RubikCube node)
{
	// Return the estimated cost to goal from this node
	int cost = 0;

	/*int hash[3][3][3];
	HashRC(node, hash);

	int cubies[20] = { 0, 2, 20, 22, 200, 202, 220, 222, 1, 10, 12, 21, 100, 102, 120, 122, 201, 210, 212, 221 };
	int zyx = 0;

	for (int cube : cubies)
	{
		int id = hash[cube / 100][cube / 10 % 10][cube % 10];
		DesiredCoords(hash, id, zyx);

		int calc = abs(cube / 100 - zyx / 100) + abs(cube / 10 % 10 - zyx / 10 % 10) + abs(cube % 10 - zyx % 10);

		cost += calc;
	}*/

	for (int f = 0; f < 6; f++)
	{
		for (int i = 0; i < 9; i++)
		{
			if (node.rc[f][i / 3][i % 3] != node.rc[f][1][1]) cost += 1;
		}
	}

	return cost;
}

bool PuzzleState::IsGoal(RubikCube& RC)
{
	for (int f = 0; f < 6; f++)
	{
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 3; x++)
				if (RC.rc[f][y][x] != RC.rc[f][1][1]) return false;
	}

	return true;
}

void PuzzleState::SumCost(PuzzleState& node, int G)
{
	node.g = G;
	//node.h = GoalDistanceEstimate(node.RC);
	node.f = node.g + node.h;
}



