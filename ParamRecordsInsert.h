#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamRecordsInsert : Parameter
{
public:
	ParamRecordsInsert(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
