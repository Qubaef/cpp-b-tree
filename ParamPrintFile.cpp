#include "ParamPrintFile.h"

ParamPrintFile::ParamPrintFile(DataManager* dataManager)
	: Parameter("printData", dataManager)
{
}


string ParamPrintFile::toString()
{
	return "printData";
}


void ParamPrintFile::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 1)
	{
		dataManager->resetDiskOperationsCounter();
		dataManager->printBTreeData();
		
		Communicator::output_success("Successfully printed whole tree data.");
		Communicator::output_inform("Disk operations: " + to_string(dataManager->getDiskOperations()));
		
		dataManager->resetDiskOperationsCounter();
	}
	else if (userInputParsed.size() > 1)
	{
		Communicator::output_error("Too many arguments were given. Should be 0.");
	}
}
