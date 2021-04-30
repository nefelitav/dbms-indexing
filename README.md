# Heap File 
compile and run : make && ./main
clean : make clean

In main.c i read records from records15K.txt and then i call the functions i implemented in HP.c.
I create a heap file , i insert records in it , i delete records according to the id, i print records according to the id and i close the file.

Every HP function calls a corresponding BF function (block file).
In HP_InsertEntry I check so that if there are empty entries in each block, I enter the new entry there to save space. Otherwise, the new entry will either go into an existing block that has not yet been filled, or a new block will be created for this record. In HP_DeleteEntry I set to zero the whole record that is to be deleted.
In HP_GetAllEntries, I either display the records with a specific id (each record has a unique id, so I only wait for 1 record), or I display all the records if the value points to NULL.

# Hash Table 
compile and run : make && ./main
clean : make clean

In main.c i read records from records15K.txt and then i call the functions i implemented in HT.c.
In HT.c i create a block that contains a pointer to each bucket, where i insert records.If a bucket is full, i create an overflow bucket.
Each bucket corresponds to a value , which is the result of a hash function.
In this way, we accomplish uniformity and high speed.
