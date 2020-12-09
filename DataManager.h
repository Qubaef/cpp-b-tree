#pragma once
#include "stdafx.h"
#include "MemoryPointer.h"
#include "BPage.h"
#include "FileManager.h"
#include "Record.h"

class DataManager
{
	// Const values
	const unsigned headerPageNumber = 0;
	const unsigned short headerPageOffset = 0;

	const unsigned initialRootPageNumber = 1;
	const unsigned short initialRootPageOffset = 0;

	map<MemoryPointer, BPage> cachedPages;

	// Header page data
	const int maxFreePositions = (DISK_PAGE_SIZE - sizeof(MemoryPointer) - 2 * sizeof(unsigned)) / sizeof(MemoryPointer);
	MemoryPointer rootPageAddress;
	vector<MemoryPointer> freePagePositions;
	vector<MemoryPointer> freeRecordPositions;

	FileManager fileManager;

	// Caching mode used in algorithm
	// mode = 0 - caching in one operation range
	// mode = 1 - full caching with save option on demand
	int cachingMode;

	// Load data from BTreeHeader
	void loadBTreeHeader();
	// Save data to BTreeHeader
	void saveBTreeHeader();

	// Load free space from file
	void loadFreeSpace();
	// Save free space to file
	void saveFreeSpace();

	// Read B-Tree header, set root page address and save header back
	void setRootPageAddress(MemoryPointer rootAddress);

	// Initialize B-Tree
	void BTreeInit();

	// Returns BTree page located in given location (manages the caching process)
	BPage& getBTreePage(MemoryPointer pageMemoryPointer);
	// Remove BTreePage
	void removeBTreePage(MemoryPointer pageMemoryPointer);

	// Save Record to data file and receive its position
	MemoryPointer saveRecord(Record& record);
	// Save Record to data file to given pos
	void saveRecord(MemoryPointer& recordPos, Record& record);
	// Load Record of given address
	Record loadRecord(MemoryPointer recordAddress);
	// Remove Record of given address
	void removeRecord(MemoryPointer recordAddress);

	// Get position for new BTree page
	MemoryPointer getNewBTreePagePos();

	// Check condition for insertion compensation
	// returned 'true' means that compensation is possible
	bool compensationInsertPossible(BPage& parent, int childIndex);
	// Check if there is compensation possible with one of the siblings
	// return index of sibling or -1 if not possible
	int checkInsertCompensation(BPage& parent, int childPageIndex);

	// Check condition for deletion compensation
	// returned 'true' means that compensation is possible
	bool compensationDeletePossible(BPage& parent, BPage& underflowPage, int childIndex);
	// Check if there is compensation possible with one of the siblings
	// return index of sibling or -1 if not possible
	int checkDeleteCompensation(BPage& parent, int childPageIndex);

	// Compensate given pages
	// direction = 0 - left
	// direction = 1 - right
	void compensate(BPage& parent, int childLIndex, int childRIndex, int direction);

	// Split given page
	void split(BPage& overflownPage);

	// Merge given pages
	void merge(BPage& underflownPage);

	// Collect largest pages and records
	void collectPagesAndRecords(BPage* currPage, vector<MemoryPointer*>& hPages, vector<MemoryPointer*>& hRecords, MemoryPointer& minPageHeight, MemoryPointer& minRecordHeight);

	// Print pages with their hierarchy info
	void printPagesDownside(MemoryPointer pageAddress, int level);
	// Print pages in order way
	// mode = 0 equals only keys
	// mode = 1 equals data with keys
	void printPagesSequentially(MemoryPointer pageAddress, int mode);

public:
	DataManager();

	// Perform test function
	void test();

	// Print structure of the BTree
	void printBTreeStructure();
	// Print data from BTree
	void printBTreeData();
	// Print all tre keys in sequential order
	void printKeysSequentially();

	// Clear all cached pages to release memory
	void clearCache(bool save);

	// Find record of given key in the B-Tree
	// finalPagePosition contains position of the most recent page (useful for inserting)
	Record find(unsigned key, MemoryPointer& resultPointer);

	// Insert given record to B-Tree
	// returned true means that the Record was successfully inserted
	// returned false means that the Record already exists
	bool insert(unsigned key, Record& record);

	// Delete record of given key from B-Tree
	// returned true means that Record was successfully removed
	// returned false means that given key does not exist
	bool deleteKey(unsigned key);

	// Modify given key's value
	bool modify(unsigned key, Record& record);

	// Get current caching mode
	int getCachingMode();
	// Change caching mode
	void setCachingMode(int mode);

	// Set diskOperations counter to 0
	void resetDiskOperationsCounter();

	// Increment diskOperations counter
	int getDiskOperations();

	// Clear all stored variables and data
	void reset();
};
