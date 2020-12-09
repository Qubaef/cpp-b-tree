#include "FileManager.h"


FileManager::FileManager()
{
	// Create main directory
	if (!CreateDirectory(mainDirectory.c_str(), NULL))
	{
		if (ERROR_PATH_NOT_FOUND == GetLastError())
		{
			Communicator::output_error("Error creating main directory: wrong path specified.");
		}
	}
	else
	{
		// If directory was successfully created, also create files inside
		reset();
	}

	diskOperations = 0;
}


inline string FileManager::getFileTypePath(FileType fileType)
{
	string filepath = mainDirectory;

	switch (fileType) {
	case B_TREE_FILE:
		filepath += "/b-tree";
		break;
	case DATA_FILE:
		filepath += "/data";
		break;
	case FREE_SPACE_FILE:
		filepath += "/freespace";
		break;
	default:
		filepath = "error";
		Communicator::output_error("Unknown file type: " + to_string(fileType));
		break;
	}

	return filepath;
}


int FileManager::saveBuffer(FileType fileType, MemoryPointer memoryPtr, int size, void* buffer)
{
	diskOperations += ceil((double)size / DISK_PAGE_SIZE);

	// Open output binary stream from file
	FILE* file = fopen(getFileTypePath(fileType).c_str(), "rb+");

	// Go to specified position in file
	fseek(file, memoryPtr.getPosition(), SEEK_SET);

	// Read given amount of bytes
	int bytesWrote = fwrite(buffer, sizeof(char), size, file);

	fclose(file);
	return bytesWrote;
}


int FileManager::loadBuffer(FileType fileType, MemoryPointer memoryPtr, int size, void* buffer)
{
	diskOperations += ceil((double)size / DISK_PAGE_SIZE);

	// Open output binary stream from file
	FILE* file = fopen(getFileTypePath(fileType).c_str(), "rb");

	// Go to specified position in file
	fseek(file, memoryPtr.getPosition(), SEEK_SET);

	long pos = ftell(file);

	// Read given amount of bytes
	int bytesRead = fread(buffer, sizeof(char), size, file);

	fclose(file);
	return bytesRead;
}


long FileManager::appendBuffer(FileType fileType, int size, void* buffer)
{
	diskOperations += ceil((double)size / DISK_PAGE_SIZE);

	// Open output binary stream from file
	FILE* file = fopen(getFileTypePath(fileType).c_str(), "ab+");

	// Append given buffer at the end of the file
	fwrite(buffer, sizeof(char), size, file);

	// Get current record position
	long position = ftell(file) - size;

	fclose(file);
	return position;
}


long FileManager::getCurrentEndPos(FileType fileType)
{
	// Open output binary stream from file
	FILE* file = fopen(getFileTypePath(fileType).c_str(), "ab+");

	// Get end position of the file
	fseek(file, 0, SEEK_END);
	long position = ftell(file);

	fclose(file);

	return position;
}


void FileManager::deleteFile(string filepath)
{
	if (remove(filepath.c_str()) != 0)
	{
		Communicator::output_error("Error deleting file: " + filepath);
	}
}


void FileManager::reset()
{
	// Reset data in used files
	for (int i = 0; i < FileType::LAST; i++)
	{
		FILE* file = fopen(getFileTypePath(FileType(i)).c_str(), "wb");
		fclose(file);
	}
}
