#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"
#define MAX_BLOCKS 512 //bytes
#define LENGTH 25


typedef struct {
    int fileDesc;   //num to open file 
    char attrType;  //'c' or 'i'
    char attrName[LENGTH]; //name
    int attrLength; //length
} HP_info; 

typedef struct{ //94 bytes + padding = 96 bytes => 5 records per block
    int id;
    char name[15];
    char surname[25];
    char address[50];
}Record; 

int HP_CreateFile(char *fileName, char attrType, char* attrName, int attrLength);
HP_info* HP_OpenFile(char *fileName);
int HP_CloseFile(HP_info* header_info);
int HP_InsertEntry( HP_info header_info,Record record);
int HP_DeleteEntry(HP_info header_info,void *value);
int HP_GetAllEntries( HP_info header_info,void *value);
