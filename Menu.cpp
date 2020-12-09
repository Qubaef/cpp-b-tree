#include "Menu.h"


Menu::Menu()
{
	dataManager = new DataManager();

	// Add parameters to menu

	menuParameters.push_back((Parameter*)(new ParamReset(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamCaching(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamTest(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamRecordsInsert(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamRecordModify(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamRecordDelete(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamRecordFind(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamPrintTree(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamPrintFile(dataManager)));
	menuParameters.push_back((Parameter*)(new ParamLoadFile(dataManager)));

	// Run menu
	run();
}


vector<string> Menu::getParsedInput(string userInput)
{
	string param;
	stringstream ss(userInput);
	vector<string> userInputParsed;

	// parse userInput to vector
	while (getline(ss, param, ' '))
	{
		userInputParsed.push_back(param);
	}

	return userInputParsed;
}


void Menu::display_help(std::vector<Parameter*>& menuParameters)
{
	Communicator::output_inform("Available options:");

	for (auto &parameter : menuParameters)
	{
		cout << "< " << parameter->toString() << endl;
	}

	cout << "< help" << endl;
	cout << "< exit" << endl;
}


string Menu::toLower(string str)
{
	for (auto& ch : str)
	{
		if (ch >= 'A' && ch <= 'Z')
		{
			ch += 'a' - 'A';
		}
	}

	return str;
}


void Menu::run()
{
	string userInput;
	vector<string> userInputParsed;
	bool found;

	// Print menu header info
	Communicator::display_header();
	display_help(menuParameters);

	// Main loop of the menu to receive and analyze user input
	while (Communicator::input_line(userInput))
	{
		//// Parse input
		userInputParsed = getParsedInput(userInput);

		//// Check if entered input was not empty
		if (userInputParsed.empty())
		{
			continue;
		}

		//// Compare parsed input with parameters to find correct one
		found = false;

		for (auto &parameter : menuParameters)
		{
			if (toLower(userInputParsed[0]) == toLower(parameter->getKeyword()))
			{
				parameter->execute(userInputParsed);
				found = true;
			}
		}

		//// If parameter was not found, check if either exit or unknown command was entered
		if (!found)
		{
			if (userInputParsed[0] == "exit")
			{
				return;
			}
			else if (userInputParsed[0] == "help")
			{
				display_help(menuParameters);
			}
			else
			{
				Communicator::output_error("Parameter " + userInputParsed[0] + " not recognized.");
			}
		}
	}
}