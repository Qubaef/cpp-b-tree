#include "ParamRecordFind.h"

ParamRecordFind::ParamRecordFind(DataManager* dataManager)
	: Parameter("find", dataManager)
{
}


string ParamRecordFind::toString()
{
	return "find [key of the record] - find record of given key in B-Tree";
}


void ParamRecordFind::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2)
	{
		int key = stoi(userInputParsed[1]);

		if (key >= 0)
		{
			dataManager->resetDiskOperationsCounter();

			MemoryPointer pagePos;
			Record result = dataManager->find(key, pagePos);

			if (result.found)
			{
				Communicator::output_success("Successfully found record " + to_string(key) + ".");
				result.printRecord();
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

