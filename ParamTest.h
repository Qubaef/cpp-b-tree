#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamTest : Parameter
{
public:
	ParamTest(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
