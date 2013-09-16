#include "State.h"


State::State(int childCount) : children(vector<State*>(childCount, 0))
{
}

State::~State(void)
{
}

void State::addChild(int index, State* state)
{
	children[index] = state;
}