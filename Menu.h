#pragma once
#include "stdafx.h"
#include "ParamPrintFile.h"
#include "ParamPrintTree.h"
#include "ParamRecordDelete.h"
#include "ParamRecordFind.h"
#include "ParamRecordsInsert.h"
#include "ParamReset.h"
#include "ParamCaching.h"
#include "ParamRecordModify.h"
#include "ParamLoadFile.h"
#include "ParamTest.h"
#include "Communicator.h"

using namespace std;

class Menu
{
private:
	DataManager* dataManager;
	std::vector<Parameter*> menuParameters;

	// Parse given string to vector of parameters
	vector<string> getParsedInput(string userInput);

	static void display_help(vector<Parameter*>& menuParameters);

	string toLower(string str);

public:
	// Initialize menu parameters and run it after initialization
	Menu();

	// Receive user input and manage it
	void run();
};
