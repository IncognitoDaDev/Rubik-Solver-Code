#include "AStarSearch.h"
#include <iostream>
#include <algorithm>

using namespace std;

void AStarSearch::Algorithm(PuzzleState &stateRC)
{
	AStarSearch algo; // Initializing a A* Search

	// Orient the edges so the next stage is simplified to only 6 possible moves (L R U L' R' U')
	stateRC.f = stateRC.GoalDistanceEstimate_EdgeOrient(stateRC);
	algo.EdgeOrient(stateRC);
	cout << endl << "f: " << stateRC.f << " / g: " << stateRC.g << endl;
	stateRC.RC.ReadRubik(stateRC.RC.dir, stateRC.RC.rc);
	cout << endl << "Edges are now oriented..." << endl;

	// DEBUG
	/*int edges[12] = { 1, 21, 100, 102, 120, 122, 201, 221, 10, 12, 210, 212 };
	int corners[8] = { 0, 2, 20, 22, 200, 202, 220, 222 };
	int centers[6] = { 211, 101, 11, 110, 121, 112 };
	int hash[3][3][3]; stateRC.HashRC(stateRC.RC, hash);
	for (int z = 0; z < 3; z++)
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 3; x++)
			{
					int ok = 1;
					for (int corner : edges)
						if (corner / 100 == z && corner / 10 % 10 == y && corner % 10 == x) ok = 0;
					for (int center : centers)
						if (center / 100 == z && center / 10 % 10 == y && center % 10 == x) ok = 0;

					if (ok == 1) hash[z][y][x] = 777;
			}

	stateRC.DehashRC(hash, stateRC.RC.rc);*/
	// DEBUG

	// Orient the corners so the next stage is simplified to only 8 possible moves (U D U' D' R2 L2 F2 B2)
	stateRC.g = 0;
	stateRC.f = stateRC.GoalDistanceEstimate_CornerOrient(stateRC);
	algo = AStarSearch(); // Initializing a NEW A* search
	algo.CornerOrient(stateRC);
	cout << endl << "f: " << stateRC.f << " / g: " << stateRC.g << endl;
	stateRC.RC.ReadRubik(stateRC.RC.dir, stateRC.RC.rc);
	cout << endl << "Corners are now oriented..." << endl;

	stateRC.g = 0;
	stateRC.f = stateRC.GoalDistanceEstimate_EdgeUDPosition(stateRC);
	algo = AStarSearch(); // Initializing a NEW A* search
	algo.EdgeUDPosition(stateRC);
	cout << endl << "f: " << stateRC.f << " / g: " << stateRC.g << endl;
	stateRC.RC.ReadRubik(stateRC.RC.dir, stateRC.RC.rc);
	cout << endl << "EdgesUD are now in their isolated layer..." << endl;

	// Solving the rest of the cube that is in G1 state (Can only use U D L2 R2 F2 B2 moves)
	stateRC.g = 0;
	stateRC.f = stateRC.GoalDistanceEstimate(stateRC);
	algo = AStarSearch(); // Initializing a NEW A* search
	algo.SolveRest(stateRC);
	cout << endl << "f: " << stateRC.f << " / g: " << stateRC.g << endl;
	stateRC.RC.ReadRubik(stateRC.RC.dir, stateRC.RC.rc);

	cout << endl << stateRC.returnSolution() << endl;
}

bool AStarSearch::AddSuccesor(PuzzleState& node)
{
	clopen.push_back(node);
	return true;
}

bool AStarSearch::GetSuccessors_EdgeOrient(PuzzleState nodeParent)
{
	if (nodeParent.IsGoal(nodeParent)) return true;

	char basicMoves[7] = "UDFBRL";

	for (int i = 0; i < strlen(basicMoves); i++)
	{
		PuzzleState NewNode;
		// Making an succesor for the basic/clockwise move
		nodeParent.RC.copyRC(NewNode.RC.rc, nodeParent.RC.rc);
		NewNode.RC.actionRubik(NewNode.RC.dir, NewNode.RC.rc, basicMoves[i], false);

		NewNode.copyMoves(nodeParent);

		NewNode.SumCost_EdgeOrient(NewNode, nodeParent);
		NewNode.moves.push_back(i);
		AddSuccesor(NewNode);


		//PuzzleState NewNode2;
		//// And another one for the prime/anticlockwise move
		//nodeParent.RC.copyRC(NewNode2.RC.rc, nodeParent.RC.rc);
		//NewNode2.RC.actionRubik(NewNode2.RC.dir, NewNode2.RC.rc, basicMoves[i], true);

		//NewNode2.copyMoves(nodeParent);

		//NewNode2.SumCost_EOLine(NewNode2, nodeParent);
		//NewNode2.moves.push_back(i + 6);
		//AddSuccesor(NewNode2);
	}
	return false;
}

void AStarSearch::EdgeOrient(PuzzleState &stateRC)
{
	PuzzleState node_current;
	open.push_back(stateRC);

	int nr = 0;

	while (!open.empty())
	{
		node_current = open[0];

		for (PuzzleState& node : open)
			if (node_current.f > node.f) node_current = node;

		if (open.size() > nr * 100)
		{
			std::cout << open.size() << " - g: " << node_current.g << "/ f: " << node_current.f << std::endl;
			nr++;
		}

		if (node_current.IsGoal(node_current) || node_current.g == node_current.f) break;

		clopen.clear();
		GetSuccessors_EdgeOrient(node_current);

		for (vector<PuzzleState>::iterator n = clopen.begin(); n != clopen.end();)
		{
			int found = 0;

			for (PuzzleState node : open)
				if ((*n).f <= node.f && node.IsSameState(*n))
				{
					n = clopen.erase(n);
					found = 1;
					break;
				}

			if (found != 1) // The node was erased? Don't bother checking in closed list then!
				for (PuzzleState node : closed)
					if ((*n).f <= node.f && node.IsSameState(*n))
					{
						n = clopen.erase(n);
						found = 1;
						break;
					}

			if (found != 1) open.push_back(*n);

			n += 1 - found;
		}

		for (auto a = open.begin(); a != open.end();)
		{
			if (node_current.IsSameState(*a))
			{
				a = open.erase(a);
				break;
			}
			a += 1;
		}

		closed.push_back(node_current);
	}

	stateRC = node_current;

	cout << endl << "f: " << node_current.f << " / g: " << node_current.g << endl;
	node_current.RC.ReadRubik(node_current.RC.dir, node_current.RC.rc);

	//return node_current;

	/*int hash[3][3][3];
	node_current.HashRC(node_current.RC, hash);
	node_current.DebugHash(hash);*/
}

bool AStarSearch::GetSuccessors_CornerOrient(PuzzleState nodeParent)
{
	if (nodeParent.IsGoal(nodeParent)) return true;

	char basicMoves[7] = "UDFBRL";

	for (int i = 0; i < strlen(basicMoves); i++)
	{
		//if (i == 2 || i == 3) continue;
		PuzzleState NewNode;
		// Making an succesor for the basic/clockwise move
		nodeParent.RC.copyRC(NewNode.RC.rc, nodeParent.RC.rc);
		NewNode.RC.actionRubik(NewNode.RC.dir, NewNode.RC.rc, basicMoves[i], false);

		NewNode.copyMoves(nodeParent);
		if (i == 2 || i == 3)
		{
			NewNode.RC.actionRubik(NewNode.RC.dir, NewNode.RC.rc, basicMoves[i], false);
			NewNode.moves.push_back(i);
		}
		NewNode.SumCost_CornerOrient(NewNode, nodeParent);
		NewNode.moves.push_back(i);
		AddSuccesor(NewNode);

		if (i == 2 || i == 3) continue; // Only for F2 AND B2
		PuzzleState NewNode2;
		// And another one for the prime/anticlockwise move
		nodeParent.RC.copyRC(NewNode2.RC.rc, nodeParent.RC.rc);
		NewNode2.RC.actionRubik(NewNode2.RC.dir, NewNode2.RC.rc, basicMoves[i], true);

		NewNode2.copyMoves(nodeParent);

		NewNode2.SumCost_CornerOrient(NewNode2, nodeParent);
		NewNode2.moves.push_back(i + 6);
		AddSuccesor(NewNode2);
	}

	return false;
}

void AStarSearch::CornerOrient(PuzzleState& stateRC)
{
	PuzzleState node_current;
	open.push_back(stateRC);

	int nr = 0;

	while (!open.empty())
	{
		node_current = open[0];

		for (PuzzleState& node : open)
			if (node_current.f > node.f) node_current = node;

		if (open.size() > nr * 100)
		{
			std::cout << open.size() << " - g: " << node_current.g << "/ f: " << node_current.f << std::endl;
			nr++;
		}

		if (node_current.IsGoal(node_current) || node_current.g == node_current.f) break;

		clopen.clear();
		GetSuccessors_CornerOrient(node_current);

		for (vector<PuzzleState>::iterator n = clopen.begin(); n != clopen.end();)
		{
			int found = 0;

			for (PuzzleState node : open)
				if ((*n).f <= node.f && node.IsSameState(*n))
				{
					n = clopen.erase(n);
					found = 1;
					break;
				}

			if (found != 1) // The node was erased? Don't bother checking in closed list then!
				for (PuzzleState node : closed)
					if ((*n).f <= node.f && node.IsSameState(*n))
					{
						n = clopen.erase(n);
						found = 1;
						break;
					}

			if (found != 1) open.push_back(*n);

			n += 1 - found;
		}

		for (auto a = open.begin(); a != open.end();)
		{
			if (node_current.IsSameState(*a))
			{
				a = open.erase(a);
				break;
			}
			a += 1;
		}

		closed.push_back(node_current);
	}

	stateRC = node_current;

	//return node_current;

	/*int hash[3][3][3];
	node_current.HashRC(node_current.RC, hash);
	node_current.DebugHash(hash);*/
}

bool AStarSearch::GetSuccessors_EdgeUDPosition(PuzzleState nodeParent)
{
	if (nodeParent.IsGoal(nodeParent)) return true;

	char basicMoves[7] = "UDFBRL";

	for (int i = 0; i < strlen(basicMoves); i++)
	{
		PuzzleState NewNode;
		// Making an succesor for the basic/clockwise move
		nodeParent.RC.copyRC(NewNode.RC.rc, nodeParent.RC.rc);
		NewNode.RC.actionRubik(NewNode.RC.dir, NewNode.RC.rc, basicMoves[i], false);

		NewNode.copyMoves(nodeParent);
		if (i == 2 || i == 3)
		{
			NewNode.RC.actionRubik(NewNode.RC.dir, NewNode.RC.rc, basicMoves[i], false);
			NewNode.moves.push_back(i);
		}
		
		NewNode.SumCost_EdgeUDPosition(NewNode, nodeParent);
		NewNode.moves.push_back(i);
		AddSuccesor(NewNode);

		if (i == 2 || i == 3) continue;
		PuzzleState NewNode2;
		// And another one for the prime/anticlockwise move
		nodeParent.RC.copyRC(NewNode2.RC.rc, nodeParent.RC.rc);
		NewNode2.RC.actionRubik(NewNode2.RC.dir, NewNode2.RC.rc, basicMoves[i], true);

		NewNode2.copyMoves(nodeParent);

		NewNode2.SumCost_EdgeUDPosition(NewNode2, nodeParent);
		NewNode2.moves.push_back(i + 6);
		AddSuccesor(NewNode2);
	}
	return false;
}

void AStarSearch::EdgeUDPosition(PuzzleState& stateRC)
{
	PuzzleState node_current;
	open.push_back(stateRC);

	int nr = 0;

	while (!open.empty())
	{
		node_current = open[0];

		for (PuzzleState& node : open)
			if (node_current.f > node.f) node_current = node;

		if (open.size() > nr * 100)
		{
			std::cout << open.size() << " - g: " << node_current.g << "/ f: " << node_current.f << std::endl;
			nr++;
		}

		if (node_current.IsGoal(node_current) || node_current.g == node_current.f) break;

		clopen.clear();
		GetSuccessors_EdgeUDPosition(node_current);

		for (vector<PuzzleState>::iterator n = clopen.begin(); n != clopen.end();)
		{
			int found = 0;

			for (PuzzleState node : open)
				if ((*n).f <= node.f && node.IsSameState(*n))
				{
					n = clopen.erase(n);
					found = 1;
					break;
				}

			if (found != 1) // The node was erased? Don't bother checking in closed list then!
				for (PuzzleState node : closed)
					if ((*n).f <= node.f && node.IsSameState(*n))
					{
						n = clopen.erase(n);
						found = 1;
						break;
					}

			if (found != 1) open.push_back(*n);

			n += 1 - found;
		}

		for (auto a = open.begin(); a != open.end();)
		{
			if (node_current.IsSameState(*a))
			{
				a = open.erase(a);
				break;
			}
			a += 1;
		}

		closed.push_back(node_current);
	}

	stateRC = node_current;

	cout << endl << "f: " << node_current.f << " / g: " << node_current.g << endl;
	node_current.RC.ReadRubik(node_current.RC.dir, node_current.RC.rc);

	//return node_current;

	/*int hash[3][3][3];
	node_current.HashRC(node_current.RC, hash);
	node_current.DebugHash(hash);*/
}

bool AStarSearch::GetSuccessors(PuzzleState nodeParent)
{
	if (nodeParent.IsGoal(nodeParent)) return true;

	char basicMoves[7] = "UDFBRL";

	for (int i = 0; i < strlen(basicMoves); i++)
	{
		PuzzleState NewNode;
		// Making an succesor for the basic/clockwise move
		nodeParent.RC.copyRC(NewNode.RC.rc, nodeParent.RC.rc);
		NewNode.RC.actionRubik(NewNode.RC.dir, NewNode.RC.rc, basicMoves[i], false);

		NewNode.copyMoves(nodeParent);
		if (i > 1)
		{
			NewNode.RC.actionRubik(NewNode.RC.dir, NewNode.RC.rc, basicMoves[i], false);
			NewNode.moves.push_back(i);
		}

		NewNode.SumCost(NewNode, nodeParent);
		NewNode.moves.push_back(i);
		AddSuccesor(NewNode);


		if (i > 1) continue; // Anticlockwise moves restrictioned to only U and D
		PuzzleState NewNode2;

		// And another one for the prime/anticlockwise move
		nodeParent.RC.copyRC(NewNode2.RC.rc, nodeParent.RC.rc);
		NewNode2.RC.actionRubik(NewNode2.RC.dir, NewNode2.RC.rc, basicMoves[i], true);

		NewNode2.copyMoves(nodeParent);

		NewNode2.SumCost(NewNode2, nodeParent);
		NewNode2.moves.push_back(i + 6);
		AddSuccesor(NewNode2);
	}
	return false;
}

void AStarSearch::SolveRest(PuzzleState &stateRC)
{
	PuzzleState node_current;
	open.push_back(stateRC);

	int nr = 0;

	while (!open.empty())
	{
		node_current = open[0];

		for (PuzzleState& node : open)
			if (node_current.f > node.f) node_current = node;

		if (open.size() > nr * 100)
		{
			std::cout << open.size() << " - g: " << node_current.g << "/ f: " << node_current.f << std::endl;
			nr++;
		}

		if (node_current.IsGoal(node_current) || node_current.g == node_current.f) break;

		clopen.clear();
		GetSuccessors(node_current);

		for (vector<PuzzleState>::iterator n = clopen.begin(); n != clopen.end();)
		{
			int found = 0;

			for (PuzzleState node : open)
				if ((*n).f <= node.f && node.IsSameState(*n))
				{
					n = clopen.erase(n);
					found = 1;
					break;
				}

			if (found != 1) // The node was erased? Don't bother checking in closed list then!
				for (PuzzleState node : closed)
					if ((*n).f <= node.f && node.IsSameState(*n))
					{
						n = clopen.erase(n);
						found = 1;
						break;
					}

			if (found != 1) open.push_back(*n);

			n += 1 - found;
		}

		for (auto a = open.begin(); a != open.end();)
		{
			if (node_current.IsSameState(*a))
			{
				a = open.erase(a);
				break;
			}
			a += 1;
		}

		closed.push_back(node_current);
	}

	stateRC = node_current;

	/*int hash[3][3][3];
	node_current.HashRC(node_current.RC, hash);
	node_current.DebugHash(hash);*/
}