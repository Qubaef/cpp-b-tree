#include "Record.h"

Record::Record()
	: found(false)
{
}


Record::Record(double a, double y, double c, double z, double x)
	: found(true), a(a), y(y), c(c), z(z), x(x)
{
	// Calculate record's value, as it will be used to sorting
	value = calculateValue(a, y, c, z, x);
}


Record::Record(double* buffer)
	:found(true), a(buffer[0]), y(buffer[1]), c(buffer[2]), z(buffer[3]), x(buffer[4])
{
	// Calculate record's value, as it will be used to sorting
	value = calculateValue(a, y, c, z, x);
}


double Record::calculateValue(double a, double y, double c, double z, double x)
{
	return 10 * a * (pow(x, 2.0) + 3 * pow(c, 3.0) * pow(z, 4.0) - 5 * pow(y, 7.0));
}


double Record::getValue()
{
	return value;
}


int Record::getParameters(double*& buffer)
{
	buffer = new double[size];
	buffer[0] = a;
	buffer[1] = y;
	buffer[2] = c;
	buffer[3] = z;
	buffer[4] = x;

	return size * sizeof(double);
}


int Record::appendParameters(double* buffer)
{
	buffer[0] = a;
	buffer[1] = y;
	buffer[2] = c;
	buffer[3] = z;
	buffer[4] = x;

	return size;
}


void Record::printRecord()
{
	printf_s("% 11.6lf : a: %-8.5lf y: %-8.5lf c: %-9.5lf z: %-9.5lf x: %-9.5lf\n", value, a, y, c, z, x);
}


int Record::randomize(double*& buffer)
{
	buffer = new double[size];
	buffer[0] = ((double)rand() / (RAND_MAX) * (maxValue - minValue) + minValue);
	buffer[1] = ((double)rand() / (RAND_MAX) * (maxValue - minValue) + minValue);
	buffer[2] = ((double)rand() / (RAND_MAX) * (maxValue - minValue) + minValue);
	buffer[3] = ((double)rand() / (RAND_MAX) * (maxValue - minValue) + minValue);
	buffer[4] = ((double)rand() / (RAND_MAX) * (maxValue - minValue) + minValue);

	return size;
}


int Record::input(double*& buffer)
{
	buffer = new double[size];

	for (int i = 0; i < size; i++)
	{
		buffer[i] = Communicator::input_double();
	}

	return size;
}


int Record::getSize()
{
	return size * sizeof(double);
}


int Record::getParamsNumber()
{
	return size;
}


void Record::printRecord(double a, double y, double c, double z, double x)
{
	printf_s("% 11.6lf : a: %-8.5lf y: %-8.5lf c: %-9.5lf z: %-9.5lf x: %-9.5lf\n", calculateValue(a, y, c, z, x), a, y, c, z, x);
}

