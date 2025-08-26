#include "AStarSearch.h"
#include <iostream>
#include <algorithm>

using namespace std;

bool AStarSearch::AddSuccesor(PuzzleState& node)
{
	clopen.push_back(node);
	return true;
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

		NewNode.SumCost(NewNode, nodeParent);
		NewNode.moves.push_back(i);
		AddSuccesor(NewNode);

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

void AStarSearch::Algorithm()
{
	PuzzleState node_current;
	open.push_back(clopen[0]);

	int nr = 0;

	while (!open.empty())
	{
		node_current = open[0];

		for (PuzzleState& node : open)
			if (node_current.f > node.f) node_current = node;

		if (open.size() > nr * 50)
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

	cout << endl << "f: " << node_current.f << " / g: " << node_current.g << endl;
	node_current.RC.ReadRubik(node_current.RC.dir, node_current.RC.rc);

	cout << endl << node_current.returnSolution() << endl;

	//return node_current;

	/*int hash[3][3][3];
	node_current.HashRC(node_current.RC, hash);
	node_current.DebugHash(hash);*/
}