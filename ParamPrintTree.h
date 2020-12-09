#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamPrintTree : Parameter
{
public:
	ParamPrintTree(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
