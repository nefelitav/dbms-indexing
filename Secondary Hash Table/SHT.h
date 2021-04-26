#ifndef SHT
#define SHT
#include "HT.h"

typedef struct
{
    int fileDesc;
    char *attrName;
    int attrLength;
    long int numBuckets;
    char *fileName;
} SHT_info;

typedef struct
{
    Record record;
    int blockId;
} SecondaryRecord;

int SHT_CreateSecondaryIndex(char *sfileName, char *attrName, int attrLength, long int buckets, char *fileName);
SHT_info *SHT_OpenSecondaryIndex(char *sfileName);
int SHT_CloseSecondaryIndex(SHT_info *header_info);
int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord record);
int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value);
int HashStatistics(char *sfilename);
int hash_function(SHT_info header_info, char *surname);

#endif
