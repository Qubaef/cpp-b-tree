#include "MemoryPointer.h"


MemoryPointer::MemoryPointer()
{
	this->offset = MEMORY_NULL;
}


MemoryPointer::MemoryPointer(int pageNumber, unsigned short offset)
{
	this->pageNumber = pageNumber;
	this->offset = offset;
}


MemoryPointer::MemoryPointer(long position)
{
	this->pageNumber = position / DISK_PAGE_SIZE;
	this->offset = (unsigned short)(position - pageNumber * DISK_PAGE_SIZE);
}



MemoryPointer& MemoryPointer::operator=(const MemoryPointer& mp)
{
	this->offset = mp.offset;
	this->pageNumber = mp.pageNumber;

	return *this;
}


bool MemoryPointer::operator<(const MemoryPointer& mp) const
{
	return this->getPosition() < mp.getPosition();
}


long MemoryPointer::getPosition() const
{
	return pageNumber * DISK_PAGE_SIZE + offset;
}
