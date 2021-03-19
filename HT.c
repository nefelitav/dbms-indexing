#include "HT.h"
int HT_CreateIndex( char *fileName, char attrType, char* attrName, int attrLength,long int buckets){

	HT_info* file;
    int fd;
    int i;
    int blockNum;
    int attrPerBlock = BLOCK_SIZE/sizeof(int);			      //The number of block pointers that can be stored in a block of buckets
	int indexBlocks = buckets/attrPerBlock+1;			      //How many blocks of buckets we need
    
    if (BF_CreateFile(fileName) != 0)                        //create file
    {
        BF_PrintError("Unable to create file.\n");
        return -1;
    }
    if ((fd = BF_OpenFile(fileName)) < 0)                    //open file and get file descriptor
    {
        BF_PrintError("Unable to open file.\n");
        return -1;
    }
    if (BF_AllocateBlock(fd) != 0)                            //allocate first block
    {
        BF_PrintError("Unable to allocate block.\n");
        return -1;
    }
    if (BF_ReadBlock(fd, 0, (void**)&file) != 0)              //find block
    {
        BF_PrintError("Unable to read block.\n");
        return -1;
    }

	memcpy(&(file->fileDesc),&fd,sizeof(int));                //initialize first block
	memcpy(&(file->attrName),&attrName,sizeof(attrName));
    memcpy(&(file->attrLength),&attrLength,sizeof(int));
	memcpy(&(file->numBuckets),&buckets,sizeof(long int));
    memcpy(&(file->fileName),&fileName,sizeof(fileName));             

    if (BF_WriteBlock(fd, 0) < 0)                             //write block
    {
        BF_PrintError("Unable to write block.\n");
        return -1;
    }

    for(i = 1;i <= indexBlocks; i++)                          //create blocks of buckets -> in our case only one
    {                                                       
        if (BF_AllocateBlock(fd) != 0)                        //allocate block of buckets
        {
            BF_PrintError("Unable to allocate block.\n");
            return -1;
        }
    }

    return BF_CloseFile(fd); 
}
HT_info* HT_OpenIndex( char *fileName){
	
	HT_info* temp, *file = (SHT_info*)malloc(sizeof(HT_info));
	int fd = BF_OpenFile(fileName);				                  
	if (fd < 0)
    {
        BF_PrintError("Unable to open file.\n");
        return NULL;
    }
	if (BF_ReadBlock(fd, 0 ,(void**)&temp) != 0)             //read first block
    {
        BF_PrintError("Unable to read block.\n");
        BF_CloseFile(fd);
        return NULL;
    }
    memcpy(file,temp,sizeof(HT_info));                      //get header info

    if (file->numBuckets < 1 || file == NULL)                //not a hash table or empty
        return NULL;
	return file;															
}
int HT_CloseIndex( HT_info* header_info ){

	if (BF_CloseFile(header_info->fileDesc) != 0)
    {
        BF_PrintError("Unable to close file.\n");
        return -1;
    }
    free(header_info);                                       //deallocate memory
    header_info = NULL;
    return 0;
}
int HT_InsertEntry(HT_info header_info, Record record){
										
}

int HT_DeleteEntry( HT_info header_info, void *value){
	
}

int HashStatistics( char* filename){
	
}
int hashNum(HT_info header_info, int id){		
	return id % header_info.numBuckets;
}
int HT_GetAllEntries(HT_info header_info,void *value){		

}

