#pragma once
#include "stdafx.h"

struct MemoryPointer
{
public:
	unsigned pageNumber;
	unsigned short offset;

	MemoryPointer();
	MemoryPointer(int pageNumber, unsigned short offset);
	MemoryPointer(long position);

	MemoryPointer& operator=(const MemoryPointer &mp);
	bool operator<(const MemoryPointer& mp) const;

	long getPosition() const;
};
