#pragma once
#include "stdafx.h"
#include "DataManager.h"

using namespace std;

class Parameter
{
private:
	string keyword;

protected:
	DataManager* dataManager;

public:
	Parameter(string keyword, DataManager* dataManager);

	// Return string value representing parameter
	virtual string toString() = 0;

	// Execute parameter operation
	virtual void execute(vector<string> userInputParsed) = 0;

	// Return keyword which is parameter identifier
	string getKeyword();
};
