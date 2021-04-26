#include "SHT.h"
int main(int argc, char **argv)
{
    HT_info *info;
    SHT_info *sinfo;
    char fileName[10];
    strcpy(fileName, "File");
    char sfileName[20];
    strcpy(sfileName, "sfile");
    int blockId, blocks, SblockId;
    void *value;
    int c, x = 0, len = 0;
    FILE *fp;
    char *str = malloc(sizeof(char) * 15);
    char ch;
    Record record[15000];
    SecondaryRecord Srecord[15000];

    fp = fopen("records15K.txt", "r"); //get records from file
    if (fp == NULL)
        return -1;
    while (!feof(fp))
    {
        ch = fgetc(fp);
        fscanf(fp, "%d", &c);
        (record[x]).id = c;
        //printf("%d\n",(record[x]).id);
        for (int i = 0; i < 3; i++)
        {
            len = 0;
            ch = fgetc(fp);
            ch = fgetc(fp);
            while ((ch = fgetc(fp)) != '"')
                str[len++] = ch;
            str[len++] = '\0';
            //printf("%s\n",str);
            if (i == 0)
            {
                strcpy(record[x].name, str);
                //printf("%s\n",(record[x]).name);
            }
            if (i == 1)
            {
                strcpy(record[x].surname, str);
                //printf("%s\n",(record[x]).surname);
            }
            if (i == 2)
            {
                strcpy(record[x].address, str);
                //printf("%s\n",(record[x]).address);
            }
            memset(str, 0, 15);
        }
        ch = fgetc(fp);
        ch = fgetc(fp);
        //printf("\n");
        x++;
        if (x == 15000)
            break;
    }
    free(str);
    fclose(fp);
    BF_Init();
    if (HT_CreateIndex("File", 'i', "id", sizeof(int), 10) != 0)
    {
        printf("Error creating file");
        return -1;
    }
    if ((info = HT_OpenIndex("File")) == NULL)
    {
        printf("Error opening file1");
        return -1;
    }
    if (SHT_CreateSecondaryIndex(sfileName, "surname", 5, 10, fileName) != 0)
    {
        printf("Error creating file");
        return -1;
    }
    if ((sinfo = SHT_OpenSecondaryIndex(sfileName)) == NULL)
    {
        printf("Error opening2 file");
        return -1;
    }

    for (int i = 0; i < 1000; i++) //insert 1000 entries from 15k
    {
        if ((blockId = HT_InsertEntry(*info, record[i])) == -1)
        {
            printf("Error inserting entry\n");
            return -1;
        }
        Srecord[i].record = record[i];
        Srecord[i].blockId = blockId;
        if ((SblockId = SHT_SecondaryInsertEntry(*sinfo, Srecord[i])) == -1)
        {
            printf("Error inserting entry\n");
            return -1;
        }
    }

    char *searchValue = "surname_0";
    if ((blocks = SHT_SecondaryGetAllEntries(*sinfo, *info, (void *)searchValue)) == -1)
    {
        printf("Error printing entries\n");
        return -1;
    }

    //value = NULL;     //uncomment and comment next line to get all entries
    value = &(record[5].id);
    if ((blocks = HT_GetAllEntries(*info, (void *)value)) == -1) //print all entries
    {
        printf("Error printing entries\n");
        return -1;
    }

    
    if (SHT_CloseSecondaryIndex(sinfo) != 0) //close file
    {
        printf("Error closing SHT file\n");
        return -1;
    }
    //HashStatistics("sfile");

    if (HT_CloseIndex(info) != 0) //close file
    {
        printf("Error closing HT file\n");
        return -1;
    }
    //HashStatistics("File");
}
