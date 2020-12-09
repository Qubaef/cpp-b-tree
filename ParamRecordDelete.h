#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamRecordDelete : Parameter
{
public:
	ParamRecordDelete(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
