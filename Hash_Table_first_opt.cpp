#include "Hash_Table_pointer.h"

#ifdef DEBUG
    #define VERIFY_TABLE(table) HashTable_Verify (table)

#else
    #define VERIFY_TABLE(table) ( (void)0)

#endif




int main ()
{
    srand (time (0)); 
    HashTable table = {};
    HashTable_Init (&table  , 1000000); 

   
    size_t search_word_count = 600000;  
    char** words_to_find = (char**)calloc (search_word_count  , sizeof (char*));
    GenerateRandomWordsForSearch (words_to_find  , search_word_count  , 5);

    uint64_t start = rdtsc ();  
    HashTable_InsertFromFile (&table  , "words.txt");
    // HashTable_BucketStats (&table  , "buckets_stat.txt");

    
    HashTable_FindWords (&table  , words_to_find  , search_word_count);
    uint64_t end = rdtsc ();
    printf ("\nTicks1: %lu\n\n"  , end - start);

    
    for (size_t i = 0; i < search_word_count; ++i) {
        free (words_to_find[i]);
    }
    free (words_to_find);

    HashTable_Destroy (&table);
    return 0;
}




void HashTable_Insert (HashTable* table  , const char* key)
{
    my_assert (table == NULL);
    my_assert (key == NULL);

    uint64_t hash = Hash_djb2 (key);
    size_t index = hash % table->size;

    ListNode* node = table->buckets[index];

    
    while (node) {
        if (strcmp (node->key  , key) == 0) {
            return; 
        }
        node = node->next;
    }

    // Вставка в начало списка
    ListNode* new_node = (ListNode*)calloc (sizeof (ListNode)  , 1);
    new_node->key = strdup (key);
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    table->count++;
    VERIFY_TABLE(table);
}


bool HashTable_Find (const HashTable* table  , const char* key)
{
    my_assert (table == NULL);
    my_assert (key == NULL);

    uint64_t hash = Hash_djb2 (key);
    size_t index = hash % table->size;

    ListNode* node = table->buckets[index];

    while (node) {
        if (strcmp (node->key  , key) == 0)
            return true;
        node = node->next;
    }                       

    return false;
}


uint64_t Hash_djb2 (const char* str)
{
    my_assert (str == NULL);

    uint64_t hash = 5381;       
    int c = 0;

    while ( (c = *str++)) {
        hash = ( (hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}


void HashTable_Init (HashTable* table  , size_t element_count)
{
    my_assert (table == NULL);

    table->size = element_count / LOAD_FACTOR + 1;
    table->buckets = (ListNode**)calloc (table->size  , sizeof (ListNode*));
    table->count = 0;
}


void HashTable_Destroy (HashTable* table)
{
    my_assert (table == NULL);

    for (size_t i = 0; i < table->size; ++i) {
        ListNode* node = table->buckets[i];
        while (node) {
            ListNode* next = node->next;
            free (node->key);
            free (node);
            node = next;
        }
    }

    free (table->buckets);
    table->buckets = NULL;
    table->size = 0;
    table->count = 0;
}


void HashTable_Dump (const HashTable* table)
{
    my_assert (table == NULL);

    if (!table || !table->buckets) return;

    for (size_t i = 0; i < table->size; ++i) {
        ListNode* node = table->buckets[i];
        if (!node) continue;  // пропускаем пустые бакеты

        printf ("Bucket [%zu]: "  , i);
        while (node) {
            printf ("\"%s\" → "  , node->key);
            node = node->next;
        }
        printf ("NULL\n");
    }
}


void HashTable_InsertFromFile (HashTable* table  , const char* filename)
{
    my_assert (table == NULL);
    my_assert (filename == NULL);
    FILE* file = fopen (filename  , "r");  
    if (!file) {
        perror ("Error opening file");
        return;
    }

    char word[256] = "";  

    while (fgets (word  , sizeof (word)  , file)) {
        word[strcspn (word  , "\n")] = '\0';

        if (word[0] != '\0') {
            HashTable_Insert (table  , word);  
        }
    }

    fclose (file);
}   


char* GenerateRandomWord (size_t length)
{
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t alphanum_size = sizeof (alphanum) - 1;

    char* word = (char*)calloc (length + 1  , 1);
    for (size_t i = 0; i < length; ++i) {

        int rand_index =    fast_rand () % alphanum_size;
        word[i] = alphanum[rand_index];
    }
    word[length] = '\0';

    return word;    
}


void GenerateRandomWordsForSearch (char** words_to_find  , size_t word_count  , size_t word_length)
{
    my_assert (words_to_find == NULL);

    for (size_t i = 0; i < word_count; ++i) {
        words_to_find[i] = GenerateRandomWord (word_length);
    }
}


void HashTable_FindWords (HashTable* table  , char** words_to_find  , size_t word_count)
{
    my_assert (table == NULL);
    my_assert (words_to_find == NULL);

    for (size_t i = 0; i < word_count; ++i) {
             bool found = HashTable_Find (table  , words_to_find[i]);
            // printf ("Find \"%s\" - %s\n"  , words_to_find[i]  , found ? "Found" : "Not Found");
    }
}


void HashTable_BucketStats (const HashTable* table , const char* filename)
{
    my_assert (table == NULL);
    my_assert (filename == NULL);
    

    FILE* out = fopen (filename , "a");
    if (!out) {

        perror ("Can't open file for bucket stats");
        return;
    }

    for (size_t i = 0; i < table->size; ++i) {

        size_t count = 0;
        ListNode* node = table->buckets[i];

        while (node) {

            count++;
            node = node->next;
        }
        fprintf (out,"bucket[%zu] count - %zu\n" , i , count);  
    }

    fclose (out);
}


__m128i xorshift128 (__m128i state) 
{
    __m128i tmp = state;
    tmp = _mm_xor_si128 (tmp , _mm_slli_epi64 (tmp , 23));
    tmp = _mm_xor_si128 (tmp , _mm_srli_epi64 (tmp , 17));
    tmp = _mm_xor_si128 (tmp , _mm_slli_epi64 (tmp , 45));
    return tmp;
}


uint64_t fast_rand () 
{
    static __m128i state = {}; 
    state = xorshift128 (state);
    return _mm_extract_epi64 (state , 0);
}


uint64_t rdtsc ()
{
    unsigned int lo  , hi;
    __asm__ volatile ("rdtsc" : "=a" (lo)  , "=d" (hi));
    return ( (uint64_t)hi << 32) | lo;
}