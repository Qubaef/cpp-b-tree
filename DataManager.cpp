#include "DataManager.h"

DataManager::DataManager()
{
	// Set default caching mode (partial)
	cachingMode = 0;

	// Load header's data to memory
	loadBTreeHeader();
	loadFreeSpace();
}


void DataManager::loadBTreeHeader()
{
	char* buffer = new char[DISK_PAGE_SIZE];

	// Load header and check if it is valid
	if (fileManager.loadBuffer(FileManager::B_TREE_FILE, MemoryPointer(headerPageNumber, headerPageOffset), DISK_PAGE_SIZE, buffer) == DISK_PAGE_SIZE)
	{
		// Load root page address
		memcpy(&rootPageAddress, &buffer[0], sizeof(MemoryPointer));
	}
	else
	{
		// If header page is not complete, initialize BTree (it will set values, so that next load is not required)
		BTreeInit();
		rootPageAddress = MemoryPointer(initialRootPageNumber, initialRootPageOffset);
	}

	delete[] buffer;
}


void DataManager::saveBTreeHeader()
{
	char* buffer = new char[DISK_PAGE_SIZE];
	int iterator = 0;

	// Save root page address
	memcpy(&buffer[iterator], &rootPageAddress, sizeof(MemoryPointer));

	fileManager.saveBuffer(FileManager::B_TREE_FILE, MemoryPointer(headerPageNumber, headerPageOffset), DISK_PAGE_SIZE, buffer);
}


void DataManager::loadFreeSpace()
{
	long freeSpaceSize = fileManager.getCurrentEndPos(FileManager::FREE_SPACE_FILE);

	if (freeSpaceSize != 0)
	{
		char* buffer = new char[freeSpaceSize];
		fileManager.loadBuffer(FileManager::FREE_SPACE_FILE, MemoryPointer(0, 0), freeSpaceSize, buffer);

		int iterator = 0;

		// Load free page positions
		unsigned number;
		memcpy(&number, &buffer[iterator], sizeof(unsigned));
		iterator += sizeof(unsigned);

		freePagePositions.clear();

		if (number != 0)
		{
			freePagePositions.resize(number);
			memcpy(&freePagePositions[0], &buffer[iterator], sizeof(MemoryPointer) * number);
			iterator += sizeof(unsigned) * freePagePositions.size();
		}

		// Load free record positions
		memcpy(&number, &buffer[iterator], sizeof(unsigned));
		iterator += sizeof(unsigned);

		freeRecordPositions.clear();

		if (number != 0)
		{
			freeRecordPositions.resize(number);
			memcpy(&freeRecordPositions[0], &buffer[iterator], sizeof(MemoryPointer) * number);
		}

		delete[] buffer;
	}
}


void DataManager::saveFreeSpace()
{
	long size = sizeof(unsigned) * 2 + (freePagePositions.size() + freeRecordPositions.size()) * sizeof(MemoryPointer);
	char* buffer = new char[size];
	int iterator = 0;

	// Save number of free page positions
	unsigned number = freePagePositions.size();
	memcpy(&buffer[iterator], &number, sizeof(unsigned));
	iterator += sizeof(unsigned);

	if (number != 0)
	{
		memcpy(&buffer[iterator], &freePagePositions[0], sizeof(MemoryPointer) * number);
		iterator += sizeof(MemoryPointer) * number;
	}

	// Save number of free record positions
	number = freeRecordPositions.size();
	memcpy(&buffer[iterator], &number, sizeof(unsigned));
	iterator += sizeof(unsigned);

	if (number != 0)
	{
		memcpy(&buffer[iterator], &freeRecordPositions[0], sizeof(MemoryPointer) * number);
	}


	fileManager.saveBuffer(FileManager::FREE_SPACE_FILE, MemoryPointer(0, 0), size, buffer);

	delete[] buffer;
}


void DataManager::setRootPageAddress(MemoryPointer rootAddress)
{
	rootPageAddress = rootAddress;
	saveBTreeHeader();
}


void DataManager::BTreeInit()
{
	// Set default rootPageAddress
	rootPageAddress = MemoryPointer(initialRootPageNumber, initialRootPageOffset);

	// Clear positions
	freePagePositions.clear();
	freeRecordPositions.clear();

	// Save header page
	saveBTreeHeader();
	saveFreeSpace();

	// Generate root page and save it
	BPage rootPage(&fileManager, rootPageAddress, rootPageAddress);
	rootPage.save();
}


BPage& DataManager::getBTreePage(MemoryPointer pageMemoryPointer)
{
	if (cachedPages.find(pageMemoryPointer) == cachedPages.end())
	{
		// If page was not cached
		cachedPages.insert({ pageMemoryPointer, BPage(&fileManager, pageMemoryPointer) });
	}

	return cachedPages[pageMemoryPointer];
}


void DataManager::removeBTreePage(MemoryPointer pageMemoryPointer)
{
	cachedPages.erase(pageMemoryPointer);
	freePagePositions.push_back(pageMemoryPointer);

	saveFreeSpace();
}


MemoryPointer DataManager::saveRecord(Record& record)
{
	double* buffer;
	int size = record.getParameters(buffer);

	if (!freeRecordPositions.empty())
	{
		// If there is free space for record use it instead of appending
		MemoryPointer freeRecordPos = freeRecordPositions.back();
		freeRecordPositions.pop_back();
		fileManager.saveBuffer(FileManager::DATA_FILE, freeRecordPos, size, buffer);
		return freeRecordPos;
	}

	// If there is no free space for record, create new by appending
	long position = fileManager.appendBuffer(FileManager::DATA_FILE, size, buffer);
	return MemoryPointer(position);
}


void DataManager::saveRecord(MemoryPointer& recordPos, Record& record)
{
	double* buffer;
	int size = record.getParameters(buffer);

	fileManager.saveBuffer(FileManager::DATA_FILE, recordPos, size, buffer);
}


Record DataManager::loadRecord(MemoryPointer recordAddress)
{
	double* buffer = new double[Record::getSize() / sizeof(double)];
	fileManager.loadBuffer(FileManager::DATA_FILE, recordAddress, Record::getSize(), buffer);
	Record loadedRecord(buffer);
	delete[] buffer;

	return loadedRecord;
}


void DataManager::removeRecord(MemoryPointer recordAddress)
{
	freeRecordPositions.push_back(recordAddress);
	saveFreeSpace();
}


MemoryPointer DataManager::getNewBTreePagePos()
{
	if (!freePagePositions.empty())
	{
		// If free page pos is available, use it instead of creating new
		MemoryPointer freePagePos = freePagePositions.back();
		freePagePositions.pop_back();
		return freePagePos;
	}

	// If there are no free page positions, create new at the end of the file
	return MemoryPointer(fileManager.getCurrentEndPos(FileManager::B_TREE_FILE));
}


bool DataManager::compensationInsertPossible(BPage& parent, int childIndex)
{
	return getBTreePage(parent.childrenArray[childIndex]).elementsNumber < 2 * BTREE_D;
}


int DataManager::checkInsertCompensation(BPage& parent, int childPageIndex)
{
	if (childPageIndex == 0)
	{
		// If child if first element (right side available)
		if (compensationInsertPossible(parent, childPageIndex + 1))
		{
			return childPageIndex + 1;
		}
	}
	else if (childPageIndex == static_cast<int>(parent.elementsNumber))
	{
		// If child is last element (left side available)
		if (compensationInsertPossible(parent, childPageIndex - 1))
		{
			return childPageIndex - 1;
		}
	}
	else
	{
		// If child is somewhere in the middle (both sides available)
		// Right
		if (compensationInsertPossible(parent, childPageIndex + 1))
		{
			return childPageIndex + 1;
		}

		//Left
		if (compensationInsertPossible(parent, childPageIndex - 1))
		{
			return childPageIndex - 1;
		}
	}

	return -1;
}


bool DataManager::compensationDeletePossible(BPage& parent, BPage& underflowPage, int childIndex)
{
	return getBTreePage(parent.childrenArray[childIndex]).elementsNumber + underflowPage.elementsNumber >= 2 * BTREE_D;
}


int DataManager::checkDeleteCompensation(BPage& parent, int childPageIndex)
{
	BPage& underflowPage = getBTreePage(parent.childrenArray[childPageIndex]);

	if (childPageIndex == 0)
	{
		// If child is first element (right side available)
		if (compensationDeletePossible(parent, underflowPage, childPageIndex + 1))
		{
			return childPageIndex + 1;
		}
	}
	else if (childPageIndex == parent.elementsNumber)
	{
		// If child is last element (left side available)
		if (compensationDeletePossible(parent, underflowPage, childPageIndex - 1))
		{
			return childPageIndex - 1;
		}
	}
	else
	{
		// If child is somewhere in the middle (both sides available)
		// Right
		if (compensationDeletePossible(parent, underflowPage, childPageIndex + 1))
		{
			return childPageIndex + 1;
		}

		//Left
		if (compensationDeletePossible(parent, underflowPage, childPageIndex - 1))
		{
			return childPageIndex - 1;
		}
	}

	return -1;
}


void DataManager::compensate(BPage& parent, int childLIndex, int childRIndex, int direction)
{
	BPage& childLPage = getBTreePage(parent.childrenArray[childLIndex]);
	BPage& childRPage = getBTreePage(parent.childrenArray[childRIndex]);

	// Determine direction of the compensation
	if (direction == 1)
	{
		// Elements should be moved to right side

		// Insert ancestor's element to right sibling at the beginning
		childRPage.keysArray.insert(childRPage.keysArray.begin(), parent.keysArray[childLIndex]);
		childRPage.recordsArray.insert(childRPage.recordsArray.begin(), parent.recordsArray[childLIndex]);
		childRPage.childrenArray.insert(childRPage.childrenArray.begin(), childLPage.childrenArray.back());

		// Update child's parent pos
		if (childLPage.childrenArray.back().offset != MEMORY_NULL)
		{
			getBTreePage(childLPage.childrenArray.back()).parentPos = childRPage.pagePos;
		}

		// Insert surplus element from left sibling into ancestor
		parent.keysArray[childLIndex] = childLPage.keysArray.back();
		parent.recordsArray[childLIndex] = childLPage.recordsArray.back();

		// Remove surplus element from left sibling
		childLPage.keysArray.pop_back();
		childLPage.recordsArray.pop_back();
		childLPage.childrenArray.pop_back();
	}
	else
	{
		// Elements should be moved to left side

		// Insert ancestor's element to left sibling at the end
		childLPage.keysArray.push_back(parent.keysArray[childLIndex]);
		childLPage.recordsArray.push_back(parent.recordsArray[childLIndex]);
		childLPage.childrenArray.push_back(childRPage.childrenArray.front());

		// Insert first right sibling's element to ancestor
		parent.keysArray[childLIndex] = childRPage.keysArray.front();
		parent.recordsArray[childLIndex] = childRPage.recordsArray.front();

		// Update child's parent pos
		if (childRPage.childrenArray.front().offset != MEMORY_NULL)
		{
			getBTreePage(childRPage.childrenArray.front()).parentPos = childLPage.pagePos;
		}

		// Remove first element from right sibling
		childRPage.keysArray.erase(childRPage.keysArray.begin());
		childRPage.recordsArray.erase(childRPage.recordsArray.begin());
		childRPage.childrenArray.erase(childRPage.childrenArray.begin());
	}

	// Update numbers of elements and childrenArrays
	childLPage.elementsNumber = childLPage.keysArray.size();
	childRPage.elementsNumber = childRPage.keysArray.size();
}


void DataManager::split(BPage& overflownPage)
{
	// If page is a Root
	if (overflownPage.parentPos.offset == MEMORY_NULL)
	{
		// Create new Root Page
		MemoryPointer newRootPos = getNewBTreePagePos();
		BPage newRoot(&fileManager, newRootPos, newRootPos);
		newRoot.save();

		// Create new Sibling Page
		MemoryPointer newPagePos = getNewBTreePagePos();
		BPage newSibling(&fileManager, newPagePos, newRootPos);

		// Insert middle key to new root and add two children
		newRoot.insert(overflownPage.keysArray[BTREE_D], overflownPage.recordsArray[BTREE_D]);
		newRoot.childrenArray[0] = overflownPage.pagePos;
		newRoot.childrenArray[1] = newSibling.pagePos;
		newRoot.save();

		// Update tree header with new root address
		setRootPageAddress(newRootPos);
		// Update page's parent address
		overflownPage.parentPos = newRootPos;

		// Split data to two siblings
		newSibling.keysArray.insert(newSibling.keysArray.end(), overflownPage.keysArray.begin() + BTREE_D + 1, overflownPage.keysArray.end());
		newSibling.recordsArray.insert(newSibling.recordsArray.end(), overflownPage.recordsArray.begin() + BTREE_D + 1, overflownPage.recordsArray.end());
		newSibling.childrenArray.insert(newSibling.childrenArray.end(), overflownPage.childrenArray.begin() + BTREE_D + 1, overflownPage.childrenArray.end());
		newSibling.elementsNumber = BTREE_D;

		// Update parentPos in newSibling
		for (MemoryPointer& childPos : newSibling.childrenArray)
		{
			if (childPos.offset != MEMORY_NULL)
			{
				BPage& childPage = getBTreePage(childPos);
				childPage.parentPos = newSibling.pagePos;
			}
		}

		overflownPage.keysArray.erase(overflownPage.keysArray.begin() + BTREE_D, overflownPage.keysArray.end());
		overflownPage.recordsArray.erase(overflownPage.recordsArray.begin() + BTREE_D, overflownPage.recordsArray.end());
		overflownPage.childrenArray.erase(overflownPage.childrenArray.begin() + BTREE_D + 1, overflownPage.childrenArray.end());
		overflownPage.elementsNumber = BTREE_D;

		newSibling.save();
	}
	else
	{
		// Find parent and child index in it
		BPage& parentPage = getBTreePage(overflownPage.parentPos);
		int currentPageIndex = parentPage.getChildIndex(overflownPage.pagePos);

		// Check if compensation is possible before splitting
		int siblingIndex = checkInsertCompensation(parentPage, currentPageIndex);
		if (siblingIndex != -1)
		{
			// Compensate
			if (siblingIndex < currentPageIndex)
			{
				compensate(parentPage, siblingIndex, currentPageIndex, 0);
			}
			else
			{
				compensate(parentPage, currentPageIndex, siblingIndex, 1);
			}

			return;
		}

		// Create new sibling Page
		MemoryPointer newPagePos = getNewBTreePagePos();
		BPage newSibling(&fileManager, newPagePos, overflownPage.parentPos);

		// Perform custom insert to parentPage (with proper child insertion)
		parentPage.insert(overflownPage.keysArray[BTREE_D], overflownPage.recordsArray[BTREE_D], newSibling.pagePos);

		// Split data to two siblings
		newSibling.keysArray.insert(newSibling.keysArray.end(), overflownPage.keysArray.begin() + BTREE_D + 1, overflownPage.keysArray.end());
		newSibling.recordsArray.insert(newSibling.recordsArray.end(), overflownPage.recordsArray.begin() + BTREE_D + 1, overflownPage.recordsArray.end());
		newSibling.childrenArray.insert(newSibling.childrenArray.end(), overflownPage.childrenArray.begin() + BTREE_D + 1, overflownPage.childrenArray.end());
		newSibling.elementsNumber = BTREE_D;

		// Update parentPos in newSibling
		for (MemoryPointer& childPos : newSibling.childrenArray)
		{
			if (childPos.offset != MEMORY_NULL)
			{
				BPage& childPage = getBTreePage(childPos);
				childPage.parentPos = newSibling.pagePos;
			}
		}

		overflownPage.keysArray.erase(overflownPage.keysArray.begin() + BTREE_D, overflownPage.keysArray.end());
		overflownPage.recordsArray.erase(overflownPage.recordsArray.begin() + BTREE_D, overflownPage.recordsArray.end());
		overflownPage.childrenArray.erase(overflownPage.childrenArray.begin() + BTREE_D + 1, overflownPage.childrenArray.end());
		overflownPage.elementsNumber = BTREE_D;

		newSibling.save();

		// Continue splitting recursively until parentPage is not overflown
		if (parentPage.elementsNumber > 2 * BTREE_D)
		{
			split(parentPage);
		}
	}
}


void DataManager::merge(BPage& underflownPage)
{
	// Get parent and target page's index in the parent
	BPage& parent = getBTreePage(underflownPage.parentPos);
	int pageToMergeIndex = parent.getChildIndex(underflownPage.pagePos);

	// Check if compensation is possible before merging
	int siblingIndex = checkDeleteCompensation(parent, pageToMergeIndex);
	if (siblingIndex != -1)
	{
		// Compensate
		if (siblingIndex < pageToMergeIndex)
		{
			compensate(parent, siblingIndex, pageToMergeIndex, 1);
		}
		else
		{
			compensate(parent, pageToMergeIndex, siblingIndex, 0);
		}

		return;
	}

	// If compensation was not possible, perform merge

	// Get sibling to merge with
	int siblingToMergeIndex;
	if (pageToMergeIndex == 0)
	{
		// Take right sibling
		siblingToMergeIndex = 1;
	}
	else
	{
		// Take left sibling
		siblingToMergeIndex = pageToMergeIndex - 1;
	}

	// Get left and right page to merge
	int leftPageIndex = min(siblingToMergeIndex, pageToMergeIndex);
	int rightPageIndex = max(siblingToMergeIndex, pageToMergeIndex);
	BPage& leftPage = getBTreePage(parent.childrenArray[leftPageIndex]);
	BPage& rightPage = getBTreePage(parent.childrenArray[rightPageIndex]);

	//// Perform merge
	// Insert ancestor's key into left page
	leftPage.keysArray.push_back(parent.keysArray[leftPageIndex]);
	leftPage.recordsArray.push_back(parent.recordsArray[leftPageIndex]);

	// Remove inserted key from ancestor
	parent.keysArray.erase(parent.keysArray.begin() + leftPageIndex);
	parent.recordsArray.erase(parent.recordsArray.begin() + leftPageIndex);
	parent.childrenArray.erase(parent.childrenArray.begin() + leftPageIndex + 1);
	parent.elementsNumber = parent.keysArray.size();

	// Insert whole right page into left page
	leftPage.keysArray.insert(leftPage.keysArray.end(), rightPage.keysArray.begin(), rightPage.keysArray.end());
	leftPage.recordsArray.insert(leftPage.recordsArray.end(), rightPage.recordsArray.begin(), rightPage.recordsArray.end());
	leftPage.childrenArray.insert(leftPage.childrenArray.end(), rightPage.childrenArray.begin(), rightPage.childrenArray.end());
	leftPage.elementsNumber = leftPage.keysArray.size();

	// Update parent pos in switched children
	for (auto& childPos : rightPage.childrenArray)
	{
		if (childPos.offset != MEMORY_NULL)
		{
			getBTreePage(childPos).parentPos = leftPage.pagePos;
		}
	}

	// Perform additional check
	if (leftPage.elementsNumber > 2 * BTREE_D || leftPage.elementsNumber < BTREE_D)
	{
		Communicator::output_error("Error in merging process");
	}

	// Remove right page
	removeBTreePage(rightPage.pagePos);

	// Validate parent page
	if (parent.parentPos.offset == MEMORY_NULL)
	{
		// If page is root, its size cant be less than 1
		if (parent.elementsNumber == 0)
		{
			// If root is empty, remove it and replace it by leftPage
			removeBTreePage(parent.pagePos);
			// Update tree header with new root address
			setRootPageAddress(leftPage.pagePos);
			leftPage.parentPos.offset = MEMORY_NULL;
		}
	}
	else
	{
		// If page is not root
		if (parent.elementsNumber < BTREE_D)
		{
			merge(parent);
		}
	}

}


void DataManager::collectPagesAndRecords(BPage* currPage, vector<MemoryPointer*>& hPages, vector<MemoryPointer*>& hRecords, MemoryPointer& minPageHeight, MemoryPointer& minRecordHeight)
{
	// Check page
	if (!(minPageHeight < currPage->pagePos))
	{
		hPages.push_back(&currPage->pagePos);
	}

	// Check records
	for (auto& recordPos : currPage->recordsArray)
	{
		if (!(minRecordHeight < recordPos))
		{
			hRecords.push_back(&recordPos);
		}
	}

	// Check children
	for (auto& childPos : currPage->childrenArray)
	{
		if (childPos.offset != MEMORY_NULL)
		{
			collectPagesAndRecords(&getBTreePage(childPos), hPages, hRecords, minPageHeight, minRecordHeight);
		}
	}
}


void DataManager::printPagesDownside(MemoryPointer pageAddress, int level)
{
	BPage& page = getBTreePage(pageAddress);

	// Print page
	page.printStructure(level);
	getchar();

	// Recursively print children
	for (int i = 0; i < page.childrenArray.size(); i++)
	{
		if (page.childrenArray[i].offset != MEMORY_NULL)
		{
			printPagesDownside(page.childrenArray[i], level + 1);
		}
	}
}


void DataManager::printPagesSequentially(MemoryPointer pageAddress, int mode)
{
	BPage& page = getBTreePage(pageAddress);

	// Recursively print children
	for (int i = 0; i < page.keysArray.size(); i++)
	{
		if (page.childrenArray[i].offset != MEMORY_NULL)
		{
			printPagesSequentially(page.childrenArray[i], mode);
		}

		if (mode == 0)
		{
			printf_s("%d\n", page.keysArray[i]);
		}
		else if (mode == 1)
		{
			Record record = loadRecord(page.recordsArray[i]);
			printf_s("%d : ", page.keysArray[i]);
			record.printRecord();
		}
	}

	// Print last child
	if (!page.childrenArray.empty() && page.childrenArray.back().offset != MEMORY_NULL)
	{
		printPagesSequentially(page.childrenArray.back(), mode);
	}
}


void DataManager::printBTreeStructure()
{
	printPagesDownside(rootPageAddress, 0);

	if (cachingMode == 0)
	{
		clearCache(false);
	}
}


void DataManager::printBTreeData()
{
	printPagesSequentially(rootPageAddress, 1);

	if (cachingMode == 0)
	{
		clearCache(false);
	}
}


void DataManager::printKeysSequentially()
{
	printPagesSequentially(rootPageAddress, 0);

	if (cachingMode == 0)
	{
		clearCache(false);
	}
}


void DataManager::test()
{
	Record record(1, 2, 3, 4, 5);

	int minNumber = 1;
	int maxNumber = 7000;
	int times = 10000;

	std::srand(time(NULL));

	for (int i = 0; i < times; i++)
	{
		insert(minNumber + (std::rand() % (maxNumber - minNumber + 1)), record);
	}
	printKeysSequentially();

	//for (int i = 0; i < times; i++)
	//{
	//	deleteKey(minNumber + (std::rand() % (maxNumber - minNumber + 1)));
	//}
	//printKeysSequentially();

	//for (int i = 0; i < times / 10; i++)
	//{
	//	insert(minNumber + (std::rand() % (maxNumber - minNumber + 1)), record);
	//}
	//printKeysSequentially();

	//for (int i = 0; i < times / 4; i++)
	//{
	//	deleteKey(minNumber + (std::rand() % (maxNumber - minNumber + 1)));
	//}
	//printKeysSequentially();

	//for (int i = 0; i < 100; i++)
	//{
	//	insert(i + 1, record);
	//}
}


void DataManager::clearCache(bool save)
{
	if (save)
	{
		// Save cached pages before removing
		for (auto pageRecord : cachedPages)
		{
			pageRecord.second.save();
		}
	}

	cachedPages.clear();
}


Record DataManager::find(unsigned key, MemoryPointer& resultPointer)
{
	// Get root page address
	MemoryPointer currentMemoryPos = rootPageAddress;
	resultPointer = currentMemoryPos;
	int res = 1;

	while (res != 0)
	{
		// Load page and find key in it
		BPage& cachedPage = getBTreePage(currentMemoryPos);
		res = cachedPage.find(key, currentMemoryPos);

		// Analyze returned result
		if (res == 1)
		{
			// If res was 1, resultPointer contains address of searched record
			return loadRecord(currentMemoryPos);
		}

		// Collect most recent memoryPos if it is not NULL
		if (currentMemoryPos.offset != MEMORY_NULL)
		{
			resultPointer = currentMemoryPos;
		}

		// If res was -1, resultPointer contains address of next page to cache, so search would be continued
	}

	// If res was 0, key does not exist
	return Record();
}


bool DataManager::insert(unsigned key, Record& record)
{
	// Check if the key is already used
	MemoryPointer currentPagePos;
	Record result = find(key, currentPagePos);

	if (result.found)
	{
		// Key is already present in BTree
		return false;
	}

	// Place record in memory and receive its position
	MemoryPointer recordPosition = saveRecord(record);

	// Receive page where key belongs
	BPage& currentPage = getBTreePage(currentPagePos);

	// Insert key to the page
	currentPage.insert(key, recordPosition);

	// Check if there was enough space for the key
	if (currentPage.elementsNumber > 2 * BTREE_D)
	{
		split(currentPage);
	}

	// Save all cached pages and clear cache
	if (cachingMode == 0)
	{
		clearCache(true);
	}

	return true;
}


bool DataManager::deleteKey(unsigned key)
{
	// Check if the key is present in the B-Tree
	MemoryPointer currentPagePos;
	Record result = find(key, currentPagePos);

	if (!result.found)
	{
		// Key is not present in BTree
		return false;
	}

	// Receive page where key belongs
	BPage* currentPage = &getBTreePage(currentPagePos);
	int keyIndex = currentPage->getKeyIndex(key);

	// Remove record assigned to the key
	removeRecord(currentPage->recordsArray[keyIndex]);

	// Remove key from the page
	if (!currentPage->isLeaf())
	{
		// If page is not leaf, replace key with largest from left branch
		// Get page with largest key from the left child of the indexed key
		BPage* childPagePtr = &getBTreePage(currentPage->childrenArray[keyIndex]);

		while (!childPagePtr->isLeaf())
		{
			childPagePtr = &getBTreePage(childPagePtr->childrenArray.back());
		}

		// Replace key and record of given index with larges key from the target page
		currentPage->keysArray[keyIndex] = childPagePtr->keysArray.back();
		currentPage->recordsArray[keyIndex] = childPagePtr->recordsArray.back();

		// Remove last key from target page
		childPagePtr->deleteKey(childPagePtr->elementsNumber - 1);

		currentPage = childPagePtr;
	}
	else
	{
		// If page is leaf, remove key
		currentPage->deleteKey(keyIndex);
	}

	// Check if page requires fix
	if (currentPage->elementsNumber < BTREE_D)
	{
		// Merge instead
		merge(*currentPage);
	}

	// Save all cached pages and clear cache
	if (cachingMode == 0)
	{
		clearCache(true);
	}

	return true;
}


bool DataManager::modify(unsigned key, Record& record)
{
	// Check if the key is present in the B-Tree
	MemoryPointer currentPagePos;
	Record result = find(key, currentPagePos);

	if (!result.found)
	{
		// Key is not present in BTree
		return false;
	}

	BPage& currentPage = getBTreePage(currentPagePos);
	MemoryPointer recordPos;

	if (currentPage.find(key, recordPos) == 1)
	{
		saveRecord(recordPos, record);
	}
	else
	{
		return false;
	}

}


int DataManager::getCachingMode()
{
	return cachingMode;
}


void DataManager::setCachingMode(int mode)
{
	cachingMode = mode;
}


void DataManager::resetDiskOperationsCounter()
{
	fileManager.diskOperations = 0;
}


int DataManager::getDiskOperations()
{
	return fileManager.diskOperations;
}


void DataManager::reset()
{
	// Reset all fileManager data
	fileManager.reset();

	// Clear stored cache
	clearCache(false);

	// Initialize B-Tree header and root page
	BTreeInit();
}
