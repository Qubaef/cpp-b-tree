#include "ParamPrintTree.h"

ParamPrintTree::ParamPrintTree(DataManager* dataManager)
	: Parameter("printTree", dataManager)
{
}


string ParamPrintTree::toString()
{
	return "printTree - print B-Tree structure without data";
}


void ParamPrintTree::execute(vector<string> userInputParsed)
{
	if (userInputParsed.size() == 1)
	{
		dataManager->resetDiskOperationsCounter();

		Communicator::output_inform("Printing tree from the top:");
		dataManager->printBTreeStructure();
		Communicator::output_success("Successfully printed whole tree structure.");

		Communicator::output_inform("Disk operations: " + to_string(dataManager->getDiskOperations()));
		dataManager->resetDiskOperationsCounter();
	}
	else if (userInputParsed.size() > 1)
	{
		Communicator::output_error("Too many arguments were given. Should be 0.");
	}
}

