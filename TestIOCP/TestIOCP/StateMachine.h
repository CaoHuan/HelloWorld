#pragma once

#include "States.h"

_CDH_BEGIN


template<typename T>
class SocketStateMachine
{
private:
	State<T> *pCurrentState;

public:
	SocketStateMachine(State<T> *state) : pCurrentState(State) 
	{
	}

	void GoState(State<T> *pNewState);

	void ChangeState(State<T> *pNewState);

	State<T>* CurrentState();

	bool IsInState(const State<T>& state) const
	{
		if (typeid(*pCurrentState) == typeid(state) )
		{
			return true;
		}

		return false;
	}

	
};

_CDH_END