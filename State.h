#pragma once

#include <vector>

using std::vector;

class State
{
public:
	State(int childCount);
	virtual ~State(void);

	vector<State*> children;
	int value;
	void addChild(int index, State* state);
	//bool enemy;
};

