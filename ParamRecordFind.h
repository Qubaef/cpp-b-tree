#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamRecordFind : Parameter
{
public:
	ParamRecordFind(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
