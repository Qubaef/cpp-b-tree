#include "ParamRecordDelete.h"

ParamRecordDelete::ParamRecordDelete(DataManager* dataManager)
	: Parameter("recDelete", dataManager)
{
}


string ParamRecordDelete::toString()
{
	return "recDelete [key of the record] - delete record of given key in B-Tree";
}


void ParamRecordDelete::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2)
	{
		int key = stoi(userInputParsed[1]);

		if (key >= 0)
		{
			dataManager->resetDiskOperationsCounter();

			if (dataManager->deleteKey(key))
			{
				Communicator::output_success("Successfully removed record " + to_string(key) + ".");
			}
			else
			{
				Communicator::output_error("Record " + to_string(key) + " not found in the B-Tree.");
			}

			Communicator::output_inform("Disk operations: " + to_string(dataManager->getDiskOperations()));
			dataManager->resetDiskOperationsCounter();
		}
		else
		{
			Communicator::output_error("Key must be greater or equal 0.");
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

