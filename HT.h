#ifndef HT
#define HT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"
#define LENGTH 25

typedef struct
{
    int fileDesc;
    char attrType;
    char *attrName;
    int attrLength;
    long int numBuckets;
} HT_info;

typedef struct
{
    int id;
    char name[15];
    char surname[25];
    char address[50];
} Record;

int HT_CreateIndex(char *fileName, char attrType, char *attrName, int attrLength, long int buckets);
HT_info *HT_OpenIndex(char *fileName);
int HT_CloseIndex(HT_info *header_info);
int HT_InsertEntry(HT_info header_info, Record record);
int HT_InsertEntry2(HT_info header_info, Record record);
int HT_DeleteEntry(HT_info header_info, void *value);
int HT_GetAllEntries(HT_info header_info, void *value);
int HashStatistics(char *filename);
int hashNum(HT_info header_info, int id);
int findItem(HT_info *header_info, int id);
#endif
