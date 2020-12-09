#pragma once
#include "stdafx.h"
#include "FileManager.h"
#include "MemoryPointer.h"
#include "Record.h"

class BPage
{
private:
	const int maxElementsNumber = (DISK_PAGE_SIZE - (2 * sizeof(MemoryPointer) + 2 * sizeof(unsigned))) / (2 * sizeof(MemoryPointer) + sizeof(unsigned));

	FileManager* fileManager;

public:
	MemoryPointer pagePos;
	MemoryPointer parentPos;
	unsigned elementsNumber;
	vector<unsigned> keysArray;
	vector<MemoryPointer> recordsArray;
	vector<MemoryPointer> childrenArray;

	// Default constructor
	BPage();

	// Constructor to initialize new page
	BPage(FileManager* fileManager, MemoryPointer pagePos, MemoryPointer parentPos);

	// Constructor to load existing page
	BPage(FileManager* fileManager, MemoryPointer pagePos);

	// Load BPage from its pagePos
	void load();

	// Save BPage to its pagePos
	void save();

	// Print page's structure to console
	void printStructure(int level);

	// Find key in the page
	// returned 0 means that element was not found
	// returned -1 means that result contains address of next page
	// returned 1 means that result contains address of the record
	int find(unsigned key, MemoryPointer& result);

	// Get index of given child
	// returns -1 if not found
	int getChildIndex(MemoryPointer& childAddress);

	// Get index of given child
	// return -1 if not found
	int getKeyIndex(unsigned key);

	// Check if page is leaf
	bool isLeaf();

	// Insert record
	void insert(unsigned key, MemoryPointer recordAddress, MemoryPointer childRight = MemoryPointer());

	// Delete key and assigned key
	void deleteKey(int keyIndex);
};
