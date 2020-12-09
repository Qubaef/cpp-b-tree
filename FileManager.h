#pragma once
#include "stdafx.h"
#include "MemoryPointer.h"
#include "Communicator.h"

using namespace std;

class FileManager
{
	const string mainDirectory = "data";

public:
	int diskOperations;
	FileManager();

	// Types of files to operate on
	enum FileType { B_TREE_FILE, DATA_FILE, FREE_SPACE_FILE, LAST };

	// Get path of given fileTyp
	inline string getFileTypePath(FileType fileType);

	// Save buffer to given file
	int saveBuffer(FileType fileType, MemoryPointer memoryPtr, int size, void* buffer);

	// Load buffer from given file
	int loadBuffer(FileType fileType, MemoryPointer memoryPtr, int size, void* buffer);

	// Append buffer to given file and return its position
	long appendBuffer(FileType fileType, int size, void* buffer);

	// Returns current end position 
	long getCurrentEndPos(FileType fileType);

	// Delete file from given path
	void deleteFile(string filepath);

	// Reset class and target directory
	void reset();
};
