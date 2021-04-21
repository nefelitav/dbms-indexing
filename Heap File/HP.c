#include "HP.h"
//compile and run : gcc -o HP HP.c -no-pie BF_64.a && ./HP

int HP_CreateFile(char *fileName, char attrType, char *attrName, int attrLength)
{
    int fd;
    void *block;

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
    if (BF_GetBlockCounter(fd) != 1) //check if there is only one block
    {
        BF_PrintError("Unable to get block counter.\n"); //otherwise,some mistake has been made
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
    if (BF_WriteBlock(fd, 0) < 0) //write block
    {
        BF_PrintError("Unable to write block.\n");
        return -1;
    }
    return BF_CloseFile(fd); //close file
}

HP_info *HP_OpenFile(char *fileName)
{
    void *block;
    HP_info *file = (HP_info *)malloc(sizeof(HP_info));
    int fd = BF_OpenFile(fileName); //open file

    if (fd < 0)
    {
        BF_PrintError("Unable to open file.\n");
        return NULL;
    }

    if (BF_ReadBlock(fd, 0, &block) != 0) //read block
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

    //printf("%d\n", file->fileDesc);                         //debug
    //printf("%c\n", file->attrType);
    //printf("%s\n", file->attrName);
    //printf("%d\n", file->attrLength);
    if (file == NULL) //failure
        return NULL;
    return file;
}

int HP_CloseFile(HP_info *header_info)
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

int HP_InsertEntry(HP_info header_info, Record record)
{
    int blockNum, b, blankblock = -1, blankrec = -1, x, f = 1;
    char empty;
    void *block;

    if ((blockNum = BF_GetBlockCounter(header_info.fileDesc)) < 0) //get count of blocks
    {
        BF_PrintError("Unable to get block counter.\n");
        return -1;
    }
    //printf("%d\n",blockNum);
    if (blockNum > 1)
    {
        for (b = 1; b < blockNum; b++) //for each block
        {
            if (BF_ReadBlock(header_info.fileDesc, b, &block) != 0) //read each block
            {
                BF_PrintError("Unable to read block.\n");
                return -1;
            }
            for (x = 0; x < 5; x++) //for each record of this block
            {

                f = 1; //check if empty entry
                for (int j = 0; j < 96; j++)
                {
                    strncpy(&empty, block, sizeof(char));
                    if (empty != 0) //if not empty then go ahead
                    {
                        f = 0;
                        block -= j;
                        break;
                    }
                    else
                    {
                        block++; //keep looking
                    }
                }

                if (f == 0)
                {                            //already a record here
                    block += sizeof(Record); //move in block
                }
                else if (f == 1) //no record here
                {
                    blankblock = b; //blank block
                    blankrec = x;   //blank record
                    //printf("%d,%d\n",blankblock,blankrec);
                    break;
                }
                //printf("block = %ld bytes\n",sizeof(block));
                //printf("record = %ld bytes\n",sizeof(record));
            }
        }
    }

    if (blankblock == -1) //no empty entry to fill
    {
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

        memcpy(block, &(record.id), sizeof(int)); //copy info on block
        block += 4;
        strncpy(block, record.name, sizeof(char) * 15);
        block += 15; //always increment block pointer,to add new info
        strncpy(block, record.surname, sizeof(char) * 25);
        block += 25;
        strncpy(block, record.address, sizeof(char) * 50);
        if (BF_WriteBlock(header_info.fileDesc, blockNum) != 0) //write block
        {
            BF_PrintError("Unable to write block.\n");
            return -1;
        }
        //printf("%d\n",blockNum);
        return blockNum;
    }
    else
    {

        if (BF_ReadBlock(header_info.fileDesc, blankblock, &block) != 0)
        {
            BF_PrintError("Unable to read block.\n");
            return -1;
        }
        block += (blankrec * sizeof(Record));

        memcpy(block, &(record.id), sizeof(int)); //copy info on block

        block += 4;
        strncpy(block, record.name, sizeof(char) * 15);
        block += 15; //always increment block,to add new info
        strncpy(block, record.surname, sizeof(char) * 25);
        block += 25;
        strncpy(block, record.address, sizeof(char) * 50);

        if (BF_WriteBlock(header_info.fileDesc, blankblock) != 0) //write block
        {
            BF_PrintError("Unable to write block.\n");
            return -1;
        }
        return blankblock;
    }
}

int HP_DeleteEntry(HP_info header_info, void *value)
{
    int blockNum, tmp, x;
    void *block;
    Record *record;

    int counter = BF_GetBlockCounter(header_info.fileDesc); //number of blocks
    if (counter < 0)
    {
        BF_PrintError("Unable to get block counter.\n");
        return -1;
    }
    else if (counter == 1)
        return 0;

    for (blockNum = 1; blockNum < counter; blockNum++)                 //go thru blocks
    {                                                                  //starting from the first record
        if (BF_ReadBlock(header_info.fileDesc, blockNum, &block) != 0) //read blocks
        {
            BF_PrintError("Unable to read block.\n");
            return -1;
        }
        for (x = 0; x < 5; x++) //for each record of this block
        {
            memcpy(&tmp, block, sizeof(int)); //get first int -> id
            if (tmp == *(int *)value)
            {
                //printf("%d, %d =",tmp,*(int*)value);
                record = (Record *)block;
                memset(record, 0, sizeof(Record));
                if (BF_WriteBlock(header_info.fileDesc, blockNum) != 0) //write block
                {
                    BF_PrintError("Unable to write block.\n");
                    return -1;
                }
                return 0;
            }
            block += sizeof(Record); //next record
        }
    }
    return -1;
}

int HP_GetAllEntries(HP_info header_info, void *value)
{
    int blockNum, tmp, x, count = 0, f = 1;
    void *block;
    char empty;
    Record *record = malloc(sizeof(Record));

    int counter = BF_GetBlockCounter(header_info.fileDesc); //count blocks
    if (counter < 0)
    {
        BF_PrintError("Unable to get block counter.\n");
        free(record);
        return -1;
    }
    else if (counter == 1)
    {
        free(record);
        return 1;
    }

    if (value == NULL) //print all entries
    {
        for (blockNum = 1; blockNum < counter; blockNum++)
        {
            if (BF_ReadBlock(header_info.fileDesc, blockNum, &block) != 0)
            {
                BF_PrintError("Unable to read block.\n");
                free(record);
                return -1;
            }
            for (x = 0; x < 5; x++) //for each record of this block
            {
                f = 1; //check if empty entry
                for (int j = 0; j < 96; j++)
                {
                    strncpy(&empty, block, sizeof(char));
                    if (empty != 0)
                    {
                        f = 0; //not empty,go ahead
                        block -= j;
                        break;
                    }
                    else
                    {
                        block++; //keep looking
                    }
                }

                if (f == 1) //empty->nothing to print here,go to next entry
                {
                    continue;
                }

                memcpy(&(record->id), block, sizeof(int));
                block += 4;
                strncpy(record->name, block, sizeof(char) * 15);
                block += 15; //always increment block pointer,to add new info
                strncpy(record->surname, block, sizeof(char) * 25);
                block += 25;
                strncpy(record->address, block, sizeof(char) * 50);
                block += 52;

                printf("%d ", record->id); //print info
                printf("%s ", record->name);
                printf("%s ", record->surname);
                printf("%s\n", record->address);
                count++;
            }
        }
        free(record);
        return count;
    }

    for (blockNum = 1; blockNum < counter; blockNum++)
    {
        if (BF_ReadBlock(header_info.fileDesc, blockNum, &block) != 0)
        {
            BF_PrintError("Unable to read block.\n");
            free(record);
            return -1;
        }

        for (x = 0; x < 5; x++) //for each record of this block
        {

            f = 1; //check if empty entry
            for (int j = 0; j < 96; j++)
            {
                strncpy(&empty, block, sizeof(char));
                if (empty != 0)
                {
                    f = 0; //not empty,go ahead
                    block -= j;
                    break;
                }
                else
                {
                    block++; //keep looking
                }
            }

            if (f == 1) //empty->nothing to print here,go to next entry
            {
                continue;
            }

            memcpy(&tmp, block, sizeof(int)); //get first int
            if (tmp == *(int *)value)
            {
                memcpy(&(record->id), block, sizeof(int));
                block += 4;
                strncpy(record->name, block, sizeof(char) * 15);
                block += 15; //always increment block pointer,to add new info
                strncpy(record->surname, block, sizeof(char) * 25);
                block += 25;
                strncpy(record->address, block, sizeof(char) * 50);
                block += 52;

                printf("%d ", record->id); //print info
                printf("%s ", record->name);
                printf("%s ", record->surname);
                printf("%s\n", record->address);
                count++;
            }
            else
            {
                block += 96;
            }
        }
    }
    free(record);
    return count;
}
