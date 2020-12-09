#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamLoadFile : Parameter
{
public:
	ParamLoadFile(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
