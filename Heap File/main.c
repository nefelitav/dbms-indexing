#include "HP.h"

int main(int argc, char **argv)
{
    HP_info *info;
    char fileName[10];
    strcpy(fileName, "file");
    int blockId, blocks;
    void *value;
    int c, x = 0, len = 0;
    FILE *fp;
    char *str = malloc(sizeof(char) * 15);
    char ch;
    Record record[15000];

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
    if (HP_CreateFile(fileName, 'c', "name", 5) != 0)
    {
        printf("Error creating file");
        return -1;
    }
    if ((info = HP_OpenFile(fileName)) == NULL)
    {
        printf("Error opening file");
        return -1;
    }
    //printf("%d\n",info->attrName);
    for (int i = 0; i < 1000; i++) //insert 1000 entries from 15k
    {
        if ((blockId = HP_InsertEntry(*info, record[i])) == -1)
        {
            printf("Error inserting entry\n");
            return -1;
        }
    }

    value = &(record[5].id); //delete entry with id the same as record[5]'s
    if (HP_DeleteEntry(*info, value) != 0)
    {
        printf("Error deleting entry\n");
        return -1;
    }

    //value = &(record[0].id);                                  //uncomment in order to find entries with id the same as value -->should be only one
    if ((blocks = HP_GetAllEntries(*info, NULL)) == -1) //print all entries
    {
        printf("Error printing entries\n");
        return -1;
    }
    if (HP_CloseFile(info) != 0) //close file
    {
        printf("Error closing file\n");
        return -1;
    }
    return 0;
}
