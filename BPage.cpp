#include "BPage.h"

BPage::BPage()
{

}


BPage::BPage(FileManager* fileManager, MemoryPointer pagePos, MemoryPointer parentPos)
{
	this->fileManager = fileManager;
	this->pagePos = pagePos;
	this->parentPos = parentPos;

	// Generate empty page
	elementsNumber = 0;

	if (maxElementsNumber < BTREE_D * 2)
	{
		Communicator::output_warn("D parameter too high in relation to disk page size");
	}
}


BPage::BPage(FileManager* fileManager, MemoryPointer pagePos)
{
	this->fileManager = fileManager;
	this->pagePos = pagePos;

	// Load existing page
	load();
}


void BPage::load()
{
	char* buffer = new char[DISK_PAGE_SIZE];
	fileManager->loadBuffer(FileManager::B_TREE_FILE, pagePos, DISK_PAGE_SIZE, buffer);

	// Analyze buffer input and parse it to page data

	// Parse page data to buffer
	int i = 0;

	// ParentPos
	memcpy(&parentPos, &buffer[i], sizeof(MemoryPointer));
	i += sizeof(MemoryPointer);

	// ElementsNumber
	memcpy(&elementsNumber, &buffer[i], sizeof(unsigned));
	i += sizeof(unsigned);

	if (elementsNumber != 0)
	{
		// KeysArray
		keysArray.resize(elementsNumber);
		memcpy(&keysArray[0], &buffer[i], sizeof(unsigned) * elementsNumber);
		i += sizeof(unsigned) * keysArray.size();

		// RecordsArray
		recordsArray.resize(elementsNumber);
		memcpy(&recordsArray[0], &buffer[i], sizeof(MemoryPointer) * elementsNumber);
		i += sizeof(MemoryPointer) * recordsArray.size();
	}

	// ChildrenNumber
	int childrenNumber;
	memcpy(&childrenNumber, &buffer[i], sizeof(unsigned));
	i += sizeof(unsigned);

	if (childrenNumber != 0)
	{
		// ChildrenArray
		childrenArray.resize(childrenNumber);
		memcpy(&childrenArray[0], &buffer[i], sizeof(MemoryPointer) * (childrenNumber));
	}


	delete[] buffer;
}


void BPage::save()
{
	char* buffer = new char[DISK_PAGE_SIZE];

	// Set offset to MEMORY_NULL value if page is Root
	if (pagePos.pageNumber == parentPos.pageNumber)
	{
		parentPos.offset = MEMORY_NULL;
	}

	// Parse page data to buffer
	int i = 0;

	// ParentPos
	memcpy(&buffer[i], &parentPos, sizeof(MemoryPointer));
	i += sizeof(MemoryPointer);

	// ElementsNumber
	memcpy(&buffer[i], &elementsNumber, sizeof(unsigned));
	i += sizeof(unsigned);

	if (elementsNumber != 0)
	{
		// KeysArray
		memcpy(&buffer[i], &keysArray[0], sizeof(unsigned) * elementsNumber);
		i += sizeof(unsigned) * elementsNumber;

		// RecordsArray
		memcpy(&buffer[i], &recordsArray[0], sizeof(MemoryPointer) * elementsNumber);
		i += sizeof(MemoryPointer) * elementsNumber;
	}

	// ChildrenNumber
	int childrenNumber = childrenArray.size();
	memcpy(&buffer[i], &childrenNumber, sizeof(unsigned));
	i += sizeof(unsigned);

	// ChildrenArray
	if (childrenNumber != 0)
	{
		memcpy(&buffer[i], &childrenArray[0], sizeof(MemoryPointer) * childrenNumber);
	}

	fileManager->saveBuffer(FileManager::B_TREE_FILE, pagePos, DISK_PAGE_SIZE, buffer);

	delete[] buffer;
}


void BPage::printStructure(int level)
{
	printf_s("Page's number: %d\n", pagePos.pageNumber);
	printf_s(" Page height: %d\n", level);
	if (level != 0) printf_s(" Parent's number: %d\n", parentPos.pageNumber);
	printf_s(" Elements number: %d\n", elementsNumber);

	printf_s(" Keys:");
	for (int i = 0; i < keysArray.size(); i++)
	{
		printf_s(" %d", keysArray[i]);
	}
	printf_s("\n");

	printf_s(" Children:");
	for (int i = 0; i < childrenArray.size(); i++)
	{
		if (childrenArray[i].offset != MEMORY_NULL)
		{
			printf_s(" %d", childrenArray[i].pageNumber);
		}
		else
		{
			printf_s(" NIL");
		}

	}
	printf_s("\n");
}


int BPage::find(unsigned key, MemoryPointer& result)
{
	int mid = keysArray.size() / 2;
	int jumpSize = mid / 2;
	int resultParam = 0;

	// Bisection until destination of element is found
	while (elementsNumber > 0)
	{
		// Check exact match
		if (key == keysArray[mid])
		{
			result = recordsArray[mid];
			resultParam = 1;
			break;
		}

		// Check left direction
		if (key < keysArray[mid])
		{
			// Check if key is singular
			if (mid == 0)
			{
				result = childrenArray.front();
				resultParam = -1;
				break;
			}

			// Check left gap from key
			if (key >= keysArray[mid - 1])
			{
				if (key == keysArray[mid - 1])
				{
					result = recordsArray[mid - 1];
					resultParam = 1;
					break;
				}

				if (key > keysArray[mid - 1])
				{
					result = childrenArray[mid];
					resultParam = -1;
					break;
				}
			}
			else
			{
				// Set new mid and continue loop
				mid -= jumpSize;
				jumpSize = (jumpSize / 2 == 0 ? 1 : jumpSize / 2);
				continue;
			}
		}

		// Check right direction
		if (key > keysArray[mid])
		{
			// Check if key is singular
			if (mid == keysArray.size() - 1)
			{
				result = childrenArray.back();
				resultParam = -1;
				break;
			}

			// Check right gap from key
			if (key <= keysArray[mid + 1])
			{
				if (key == keysArray[mid + 1])
				{
					result = recordsArray[mid + 1];
					resultParam = 1;
					break;
				}

				if (key < keysArray[mid + 1])
				{
					result = childrenArray[mid + 1];
					resultParam = -1;
					break;
				}
			}
			else
			{
				// Set new mid and continue loop
				mid += jumpSize;
				jumpSize = (jumpSize / 2 == 0 ? 1 : jumpSize / 2);
				continue;
			}
		}
	}

	// If result is null, 
	if (result.offset == MEMORY_NULL)
	{
		resultParam = 0;
	}

	return resultParam;
}


int BPage::getChildIndex(MemoryPointer& childAddress)
{
	for (int i = 0; i < childrenArray.size(); i++)
	{
		if (childAddress.getPosition() == childrenArray[i].getPosition())
		{
			return i;
		}
	}

	return -1;
}


int BPage::getKeyIndex(unsigned key)
{
	for (int i = 0; i < keysArray.size(); i++)
	{
		if (key == keysArray[i])
		{
			return i;
		}
	}

	return -1;
}


bool BPage::isLeaf()
{
	if (!childrenArray.empty())
	{
		if (childrenArray[0].offset == MEMORY_NULL)
		{
			return true;
		}

		return false;
	}

	// Only case when empty page exists is when there is initial page, so it is both root and leaf
	return true;
}


void BPage::insert(unsigned key, MemoryPointer recordAddress, MemoryPointer childRight)
{
	int index = 0;
	for (; index < keysArray.size(); index++)
	{
		if (key < keysArray[index])
		{
			break;
		}
	}

	// If page was empty, add one additional child
	if (elementsNumber == 0)
	{
		childrenArray.push_back(MemoryPointer());
	}

	keysArray.insert(keysArray.begin() + index, key);
	recordsArray.insert(recordsArray.begin() + index, recordAddress);
	childrenArray.insert(childrenArray.begin() + index + 1, childRight);
	elementsNumber++;
}


void BPage::deleteKey(int keyIndex)
{
	elementsNumber--;
	keysArray.erase(keysArray.begin() + keyIndex);
	recordsArray.erase(recordsArray.begin() + keyIndex);

	// Assuming that deletion takes place in leaf, where all children are NIL
	// (erasing from the middle requires reallocation)
	childrenArray.pop_back();
}