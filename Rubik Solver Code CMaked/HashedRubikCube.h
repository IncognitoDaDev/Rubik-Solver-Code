#pragma once
class HashedRubikCube
{
public:
	int rc[3][3][3];
	int oldMove = -1;

	enum Orientation
	{
		Up = 0,
		Down = 1,
		Front = 2,
		Back = 3,
		Right = 4,
		Left = 5
	};

	void hashRC(int (&rc)[6][3][3]);
	void dehashRC(int (&rc)[6][3][3]);

	void action(int move);
	void undo();

	void rotateFace(int face, bool prime, bool repeat);
	void SwitchCubie(int cubie1, int cubie2, int type);

	void extractFaceData(int face, int (&ma)[3][3]);
};

