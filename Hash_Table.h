#include "My_features.h"
#include <cstddef>
#include <cstdint>
#include <time.h>
#include <xxhash.h>
#include <immintrin.h>



const int LOAD_FACTOR = 20;

struct ListNode {
    char key[32];
    ListNode* next;
};

struct HashTable {
    ListNode** buckets;
    size_t size;      
    size_t count;     
};

extern "C" { uint32_t crc32_hash(const char* data);}
uint64_t Hash_djb2 (const char* str);
void HashTable_Init (HashTable* table , size_t element_count);
void HashTable_Insert (HashTable* table , const char* key);
bool HashTable_Find (const HashTable* table , const char* key);
void HashTable_Destroy (HashTable* table);
void HashTable_InsertFromFile (HashTable* table , const char* filename);
char* GenerateRandomWord (size_t length);
void GenerateRandomWordsForSearch (char** words_to_find , size_t word_count , size_t word_length);
void HashTable_FindWords (HashTable* table , char** words_to_find , size_t word_count);
void HashTable_BucketStats(const HashTable* table, const char* filename);   
uint64_t fast_rand();
int fast_strcmp(const char* s1, const char* s2);
uint64_t rdtsc ();