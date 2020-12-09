#include "Parameter.h"

Parameter::Parameter(string keyword, DataManager* dataManager) :
	keyword(keyword), dataManager(dataManager)
{
}


string Parameter::getKeyword()
{
	return keyword;
}
