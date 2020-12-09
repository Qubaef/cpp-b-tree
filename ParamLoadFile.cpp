#include "ParamLoadFile.h"

ParamLoadFile::ParamLoadFile(DataManager* dataManager)
	: Parameter("load", dataManager)
{
}


string ParamLoadFile::toString()
{
	return "load [path of the file to load] - load file containing instructions to perform on B-Tree";
}


void ParamLoadFile::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 2)
	{
		if (FILE *file = fopen(userInputParsed[1].c_str(), "r")) {
			fclose(file);

			dataManager->resetDiskOperationsCounter();

			ifstream targetFile;
			targetFile.open(userInputParsed[1]);

			char operation;
			// Loop through file
			int key;
			MemoryPointer pagePos;
			double* buffer;
			Record record;

			while (targetFile.get(operation))
			{
				switch (operation)
				{
				case 'f':
					// Find
					targetFile >> key;

					record = dataManager->find(key, pagePos);

					if (record.found)
					{
						Communicator::output_success("Successfully found record " + to_string(key) + ".");
						record.printRecord();
					}
					else
					{
						Communicator::output_error("Record " + to_string(key) + " not found in the B-Tree.");
					}

					break;
				case 'i':
					// Insert
					targetFile >> key;

					buffer = new double[Record::getParamsNumber()];
					for (int i = 0; i < Record::getParamsNumber(); i++)
					{
						targetFile >> buffer[i];
					}

					record = Record(buffer);

					if (dataManager->insert(key, record))
					{
						Communicator::output_success("Record successfully inserted.");
					}
					else
					{
						Communicator::output_error("Key already present in the B-Tree.");
					}

					delete[] buffer;

					break;
				case 'm':
					// Modify
					targetFile >> key;

					buffer = new double[Record::getParamsNumber()];
					for (int i = 0; i < Record::getParamsNumber(); i++)
					{
						targetFile >> buffer[i];
					}

					record = Record(buffer);

					if (dataManager->modify(key, record))
					{
						Communicator::output_success("Record successfully modified.");
					}
					else
					{
						Communicator::output_error("Record of given key not found in the B-Tree.");
					}

					delete[] buffer;

					break;
				case 'd':
					// Delete
					targetFile >> key;

					if (dataManager->deleteKey(key))
					{
						Communicator::output_success("Successfully removed record " + to_string(key) + ".");
					}
					else
					{
						Communicator::output_error("Record " + to_string(key) + " not found in the B-Tree.");
					}

					break;
				case 'p':
					// Print Data
					dataManager->printBTreeData();
					Communicator::output_success("Successfully printed whole tree data.");

					break;
				case '\n':
					break;
				default:
					Communicator::output_warn(string("Unknown operation: ") += operation);
					break;
				}
			}

			targetFile.close();

			Communicator::output_inform("Disk operations: " + to_string(dataManager->getDiskOperations()));
			dataManager->resetDiskOperationsCounter();

			Communicator::output_success("Successfully loaded data from file " + userInputParsed[1]);
		}
		else
		{
			Communicator::output_error("Given file not accessible.");
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

