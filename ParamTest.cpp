#include "ParamTest.h"

ParamTest::ParamTest(DataManager* dataManager)
	: Parameter("test", dataManager)
{
}


string ParamTest::toString()
{
	return "test";
}


void ParamTest::execute(vector<string> userInputParsed)
{
	// Perform test on dataManager
	dataManager->test();
}