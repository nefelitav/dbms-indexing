#include "SHT.h"

int SHT_CreateSecondaryIndex(char *sfileName, char *attrName, int attrLength, long int buckets, char *fileName)
{
	SHT_info *file;
	int fd;
	int i;
	int blockNum;
	int attrPerBlock = BLOCK_SIZE / sizeof(int);  //The number of block pointers that can be stored in a block of buckets
	int indexBlocks = buckets / attrPerBlock + 1; //How many blocks of buckets we need

	if (BF_CreateFile(sfileName) != 0) //create file
	{
		BF_PrintError("Unable to create file.\n");
		return -1;
	}
	if ((fd = BF_OpenFile(sfileName)) < 0) //open file and get file descriptor
	{
		BF_PrintError("Unable to open file.\n");
		return -1;
	}
	if (BF_AllocateBlock(fd) != 0) //allocate first block
	{
		BF_PrintError("Unable to allocate block.\n");
		return -1;
	}
	if (BF_ReadBlock(fd, 0, (void **)&file) != 0) //find block
	{
		BF_PrintError("Unable to read block.\n");
		return -1;
	}
	memcpy(&(file->fileDesc), &fd, sizeof(int)); //initialize first block
	memcpy(&(file->attrName), &attrName, sizeof(attrName));
	memcpy(&(file->attrLength), &attrLength, sizeof(int));
	memcpy(&(file->numBuckets), &buckets, sizeof(long int));
	memcpy(&(file->fileName), &fileName, sizeof(fileName));
	int *checkMark = (int *)file;
	checkMark[BLOCK_SIZE / sizeof(int)] = 1; //1 means that we are dealing with an SHT file
	if (BF_WriteBlock(fd, 0) < 0)			 //write block
	{
		BF_PrintError("Unable to write block.\n");
		return -1;
	}

	for (i = 1; i <= indexBlocks; i++) //create blocks of buckets -> in our case only one
	{
		if (BF_AllocateBlock(fd) != 0) //allocate block of buckets
		{
			BF_PrintError("Unable to allocate block.\n");
			return -1;
		}
	}

	return BF_CloseFile(fd);
}

SHT_info *SHT_OpenSecondaryIndex(char *sfileName)
{
	SHT_info *temp, *file = (SHT_info *)malloc(sizeof(SHT_info));
	int fd = BF_OpenFile(sfileName);
	if (fd < 0)
	{
		BF_PrintError("Unable to open file.\n");
		return NULL;
	}
	if (BF_ReadBlock(fd, 0, (void **)&temp) != 0) //read first block
	{
		BF_PrintError("Unable to read block.\n");
		BF_CloseFile(fd);
		return NULL;
	}
	int *checkMark = (int *)temp;
	if (checkMark[BLOCK_SIZE / sizeof(int)] != 1)
	{
		return NULL;
	}
	memcpy(file, temp, sizeof(SHT_info)); //get header info

	if (file->numBuckets < 1 || file == NULL) //not a hash table or empty
		return NULL;
	return file;
}

int SHT_CloseSecondaryIndex(SHT_info *header_info)
{
	if (BF_CloseFile(header_info->fileDesc) != 0)
	{
		BF_PrintError("Unable to close file.\n");
		return -1;
	}
	free(header_info); //deallocate memory
	header_info = NULL;
	return 0;
}

int hash_function(SHT_info header_info, char *surname)
{
	unsigned long hash = 5381;
	int c;

	while (c = *surname++)
		hash = ((hash << 5) + hash) + c;

	return hash % header_info.numBuckets;
}

