#include "HT.h"
int HT_CreateIndex(char *fileName, char attrType, char *attrName, int attrLength, long int buckets)
{
    void *block;
    int fd;
    int i;
    int blockNum;
    int attrPerBlock = BLOCK_SIZE / sizeof(int);  //The number of block pointers that can be stored in a block of buckets
    int indexBlocks = buckets / attrPerBlock + 1; //How many blocks of buckets we need

    if (BF_CreateFile(fileName) != 0) //create file
    {
        BF_PrintError("Unable to create file.\n");
        return -1;
    }
    if ((fd = BF_OpenFile(fileName)) < 0) //open file and get file descriptor
    {
        BF_PrintError("Unable to open file.\n");
        return -1;
    }
    if (BF_AllocateBlock(fd) != 0) //allocate first block
    {
        BF_PrintError("Unable to allocate block.\n");
        return -1;
    }
    if (BF_ReadBlock(fd, 0, &block) != 0) //find block
    {
        BF_PrintError("Unable to read block.\n");
        return -1;
    }

    strncpy(block++, (char *)&attrType, sizeof(char)); //initialize first block
    strncpy(block, attrName, sizeof(char) * LENGTH);
    block += LENGTH; //always increment block pointer,to add new info
    memcpy(block, &attrLength, sizeof(int));
    block += 4;
    memcpy(block, &buckets, sizeof(long int));

    if (BF_WriteBlock(fd, 0) < 0) //write block
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

HT_info *HT_OpenIndex(char *fileName)
{

    HT_info *file = (HT_info *)malloc(sizeof(HT_info));
    void *block;
    char x;
    int fd = BF_OpenFile(fileName);
    if (fd < 0)
    {
        BF_PrintError("Unable to open file.\n");
        return NULL;
    }
    if (BF_ReadBlock(fd, 0, &block) != 0) //read first block
    {
        BF_PrintError("Unable to read block.\n");
        BF_CloseFile(fd);
        return NULL;
    }
    file->fileDesc = fd;
    strncpy(&(file->attrType), block++, sizeof(char));     //copy info from block
    strncpy(file->attrName, block, sizeof(char) * LENGTH); //always increment block pointer,to add new info
    block += LENGTH;
    file->attrName[LENGTH - 1] = '\0'; //to make sure thats the end of string
    memcpy(&(file->attrLength), block, sizeof(int));
    block += 4;
    memcpy(&(file->numBuckets), block, sizeof(long int));
    if (file->numBuckets < 1 || file == NULL) //not a hash table or empty
        return NULL;
    return file;
}

int HT_CloseIndex(HT_info *header_info)
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

int HT_InsertEntry(HT_info header_info, Record record)
{
    char filled;
    int f = 1, i, j;
    int blockNum, blankrec = -1;
    void *block;
    int *ptr;
    int *base;
    int pos = HashFunction(header_info, record.id);
    int attrPerBlock = BLOCK_SIZE / header_info.attrLength;        //How many block indexes fit in a block
    int indexBlocks = header_info.numBuckets / (attrPerBlock - 1); //Number of blocks of buckets
    int blockOfBuckets = pos / (attrPerBlock - 1) + 1;             //In which block of buckets the info is
    int indexInBlock = pos % (attrPerBlock - 1);                   //Index in the block of buckets

    if ((blockNum = BF_GetBlockCounter(header_info.fileDesc)) < 2) //get count of blocks
    {
        BF_PrintError("Unable to get block counter.\n");
        return -1;
    }
    if (BF_ReadBlock(header_info.fileDesc, blockOfBuckets, &block) != 0) //find block
    {
        BF_PrintError("Unable to read block.\n");
        return -1;
    }
    block = block + indexInBlock;
    ptr = block;
    if (*ptr == 0) //no bucket yet that corresponds to the result of the hash function
    {
        if (BF_AllocateBlock(header_info.fileDesc) != 0)
        {
            BF_PrintError("Unable to allocate block.\n");
            return -1;
        }
        *ptr = blockNum;
        if (BF_WriteBlock(header_info.fileDesc, blockOfBuckets) != 0) //write block of buckets
        {
            BF_PrintError("Unable to write block.\n");
            return -1;
        }

        if (BF_ReadBlock(header_info.fileDesc, blockNum, &block) != 0) //find block
        {
            BF_PrintError("Unable to read block.\n");
            return -1;
        }
    }
    else
    {
        if (BF_ReadBlock(header_info.fileDesc, *ptr, &block) != 0) //find block
        {
            BF_PrintError("Unable to read block.\n");
            return -1;
        }
    }
    base = block;
    for (i = 0; i < BLOCK_SIZE / sizeof(Record); i++) //for every record position - 5 in total
    {
        f = 1;                   //check if empty entry
        for (j = 0; j < 96; j++) //for every char in record space
        {
            strncpy(&filled, block, sizeof(char));
            if (filled != 0) //if not empty then go to next record position
            {
                f = 0;
                block -= j; //go to base of block
                break;
            }
            block++; //keep looking
        }
        if (f == 1) //no record here
        {
            block -= 96;
            memcpy(block, &record, sizeof(Record));                        //copy info on block
            if ((blockNum = BF_GetBlockCounter(header_info.fileDesc)) < 2) //get count of blocks
            {
                BF_PrintError("Unable to get block counter.\n");
                return -1;
            }
            return blockNum; //insertion is done
        }
        block += sizeof(Record); //move in block
    }

    if (blankrec == -1) //full bucket
    {
        block = base;
        block += BLOCK_SIZE - sizeof(int); //at the end of the block ,point to the overflow bucket
        memcpy(block, &blockNum, sizeof(int));
        if (BF_AllocateBlock(header_info.fileDesc) != 0) //allocate new block
        {
            BF_PrintError("Unable to allocate block.\n");
            return -1;
        }
        if (BF_ReadBlock(header_info.fileDesc, blockNum, &block) != 0) //read last block
        {
            BF_PrintError("Unable to read block.\n");
            return -1;
        }
        memcpy(block, &record, sizeof(Record));                 //copy info on block
        if (BF_WriteBlock(header_info.fileDesc, blockNum) != 0) //write block
        {
            BF_PrintError("Unable to write block.\n");
            return -1;
        }
        return blockNum;
    }
}

/*
int HT_DeleteEntry(HT_info header_info, void *value)
{
}

int HT_GetAllEntries(HT_info header_info, void *value)
{
}
*/

int HashFunction(HT_info header_info, int id)
{
    return id % header_info.numBuckets;
}