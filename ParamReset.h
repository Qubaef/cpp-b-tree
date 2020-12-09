#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamReset : Parameter
{
public:
	ParamReset(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
