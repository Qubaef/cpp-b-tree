#include "ParamRecordsInsert.h"

ParamRecordsInsert::ParamRecordsInsert(DataManager* dataManager)
	: Parameter("recInsert", dataManager)
{
}


string ParamRecordsInsert::toString()
{
	return "recInsert [number of records to insert] [-r to randomize record's data] - insert given number of records to B-Tree";
}


void ParamRecordsInsert::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2 || userInputParsed.size() == 3)
	{
		dataManager->resetDiskOperationsCounter();

		int recordsNumber = stoi(userInputParsed[1]);
		bool random = false;

		// Check if records should be random
		if (userInputParsed.size() == 3 && userInputParsed[2] == "-r")
		{
			random = true;
		}

		for (int i = 0; i < recordsNumber; i++)
		{
			double* buffer = nullptr;
			unsigned key = 0;

			Communicator::output_inform("Record " + to_string(i + 1));

			cout << "Input record's key: " << endl;
			cin >> key;

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

			Communicator::output_inform("Inserting record " + to_string(key) + ":");
			newRecord.printRecord();

			if (dataManager->insert(key, newRecord))
			{
				Communicator::output_success("Record successfully inserted.");
			}
			else
			{
				Communicator::output_error("Key already present in the B-Tree.");
			}
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

