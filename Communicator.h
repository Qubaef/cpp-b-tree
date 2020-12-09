#pragma once
#include "stdafx.h"

using namespace std;

class Communicator
{
public:
	static void display_header();

	static void output_inform(string str);
	static void output_error(string str);
	static void output_warn(string str);
	static void output_success(string str);

	static istream& input_line(string& str);
	static double input_double();
};
