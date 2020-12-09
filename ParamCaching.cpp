#include "ParamCaching.h"

ParamCaching::ParamCaching(DataManager* dataManager)
	: Parameter("caching", dataManager)
{
}


string ParamCaching::toString()
{
	string mode = "error";

	if (dataManager->getCachingMode() == 0)
	{
		mode = "partial";
	}
	else if (dataManager->getCachingMode() == 1)
	{
		mode = "full";
	}

	return "caching [partial/full] - set caching mode OR [clear] - clear stored pages | current mode: " + mode;
}


void ParamCaching::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2)
	{
		if (userInputParsed[1] == "partial")
		{
			dataManager->setCachingMode(0);
			Communicator::output_success("Successfully set cachingMode to partial.");
		}
		else if (userInputParsed[1] == "full")
		{
			dataManager->setCachingMode(1);
			Communicator::output_success("Successfully set cachingMode to full.");
		}
		else if (userInputParsed[1] == "clear")
		{
			dataManager->resetDiskOperationsCounter();

			dataManager->clearCache(true);
			Communicator::output_success("Successfully cleared and saved all cached pages.");

			Communicator::output_inform("Disk operations: " + to_string(dataManager->getDiskOperations()));
		}
		else
		{
			Communicator::output_error("Unknown operation: " + userInputParsed[1]);
		}
	}
	else if (userInputParsed.size() < 2)
	{
		Communicator::output_error("Too few arguments were given. Should be 1.");
	}
	else if (userInputParsed.size() > 2)
	{
		Communicator::output_error("Too many arguments were given. Should be 1.");
	}
}

