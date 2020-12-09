#include "ParamReset.h"


ParamReset::ParamReset(DataManager* dataManager)
	: Parameter("reset", dataManager)
{
}


string ParamReset::toString()
{
	return "reset - reset B-Tree and data to initial state";
}


void ParamReset::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 1)
	{
		dataManager->reset();
		Communicator::output_success("Successfully reset all data.");
	}
	else if (userInputParsed.size() < 1)
	{
		Communicator::output_error("Too few arguments were given. Should be 1.");
	}
	else if (userInputParsed.size() > 1)
	{
		Communicator::output_error("Too many arguments were given. Should be 1.");
	}
}