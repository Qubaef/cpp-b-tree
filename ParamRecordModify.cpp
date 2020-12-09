#include "ParamRecordModify.h"

ParamRecordModify::ParamRecordModify(DataManager* dataManager)
	: Parameter("recModify", dataManager)
{
}


string ParamRecordModify::toString()
{
	return "recModify [key of the record] [-r to randomize record's data] - modify record of given key";
}


void ParamRecordModify::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2 || userInputParsed.size() == 3)
	{
		dataManager->resetDiskOperationsCounter();

		int key = stoi(userInputParsed[1]);
		bool random = false;

		// Check if records should be random
		if (userInputParsed.size() == 3 && userInputParsed[2] == "-r")
		{
			random = true;
		}

		double* buffer = nullptr;

		if (random)
		{
			Record::randomize(buffer);
		}
		else
		{
			cout << "Input record's data key: " << endl;
			Record::input(buffer);
		}

		Record newRecord(buffer);
		delete[] buffer;

		newRecord.printRecord();

		if (dataManager->modify(key, newRecord))
		{
			Communicator::output_success("Record successfully modified.");
		}
		else
		{
			Communicator::output_error("Record of given key not found in the B-Tree.");
		}


		Communicator::output_inform("Disk operations: " + to_string(dataManager->getDiskOperations()));
		dataManager->resetDiskOperationsCounter();
	}
	else if (userInputParsed.size() < 2)
	{
		Communicator::output_error("Too few arguments were given. Should be 2 or 3.");
	}
	else if (userInputParsed.size() > 3)
	{
		Communicator::output_error("Too many arguments were given. Should be 2 or 3.");
	}
}

