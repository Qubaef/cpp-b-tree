#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamCaching : Parameter
{
public:
	ParamCaching(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};

