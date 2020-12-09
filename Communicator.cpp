#include "Communicator.h"

void Communicator::display_header()
{
	cout << "\033[32m==== Project - Struktury baz danych ====\033[0m" << endl;
	cout << "Title : B-Tree file indexing" << endl;
	cout << "Author: Jakub Fedorowicz" << endl << endl;
}


void Communicator::output_inform(string str)
{
	cout << "\033[34m" << str << "\033[0m\n";
}


void Communicator::output_error(string str)
{
	cout << "\033[31mERROR: " << str << "\033[0m\n";
}


void Communicator::output_warn(string str)
{
	cout << "\033[33mWARNING: " << str << "\033[0m\n";
}


void Communicator::output_success(string str)
{
	cout << "\033[32m" << str << "\033[0m\n";
}


istream& Communicator::input_line(string& str)
{
	cout << "> ";
	return getline(cin, str);
}


double Communicator::input_double()
{
	double value;

	// Loop until receiving proper value
	while (true)
	{
		cout << ": ";

		if (cin >> value)
		{
			return value;
		}
		else
		{
			output_error("Entered value is not a double. Please enter proper value again.");

			// Clear cin errors
			cin.clear();
			while (cin.get() != '\n'); // empty loop
		}
	}
}
