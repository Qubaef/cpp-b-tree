#pragma once
#include "stdafx.h"
#include "Parameter.h"
#include "Communicator.h"

class ParamRecordModify : Parameter
{
public:
	ParamRecordModify(DataManager* dataManager);

	string toString();
	void execute(vector<string> userInputParsed);
};
