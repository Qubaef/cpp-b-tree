#pragma once
#include "stdafx.h"
#include "Communicator.h"

struct Record
{
private:
	static const int size = 5;
	static const int minValue = 0;
	static const int maxValue = 1;

	// Record parameters
	double a;
	double y;
	double c;
	double z;
	double x;

	// Value of the record
	long double value;

	static double calculateValue(double a, double y, double c, double z, double x);

public:
	bool found;

	Record();
	Record(double a, double y, double c, double z, double x);
	Record(double* buffer);

	// return value of the record
	double getValue();

	// return buffer of parameters and the size of the buffer in reference parameter
	int getParameters(double*& buffer);

	// append given buffer with record parameters; return number of added elements
	int appendParameters(double* buffer);

	// print record
	void printRecord();

	// return buffer representing random record (without its initialization)
	static int randomize(double*& buffer);

	// return buffer containing input record (without its initialization)
	static int input(double*& buffer);

	// return size of the record (not equal to Record object size)
	static int getSize();

	// return number of parameters which are joined in one record
	static int getParamsNumber();

	// print record for given parameters
	static void printRecord(double a, double y, double c, double z, double x);
};
