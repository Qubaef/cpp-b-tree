#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamPrintFile : Parameter
{
public:
	ParamPrintFile(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
