#include "Hash_Table.h"

#ifdef DEBUG
    #define VERIFY_TABLE(table) HashTable_Verify (table)

#else
    #define VERIFY_TABLE(table) ((void)0)

#endif

int main ()
{
    srand (time (0)); 
    HashTable table = {};
    HashTable_Init (&table , 700000); 

   
    size_t search_word_count = 600000;  
    char** words_to_find = (char**)calloc (search_word_count , sizeof (char*));
    GenerateRandomWordsForSearch (words_to_find , search_word_count , 5);

    uint64_t start = rdtsc ();  
    HashTable_InsertFromFile (&table , "words.txt");
    // HashTable_BucketStats (&table , "buckets_stat.txt"); 

    
    HashTable_FindWords (&table , words_to_find , search_word_count);
    uint64_t end = rdtsc ();
    printf ("\nTicks1: %lu\n\n" , end - start);

    
    for (size_t i = 0; i < search_word_count; ++i) {
        free (words_to_find[i]);
    }
    free (words_to_find);

    HashTable_Destroy (&table);
    return 0;
}




void HashTable_Insert (HashTable* table , const char* key)
{
    my_assert (table == NULL);
    my_assert (key == NULL);
    
    uint64_t hash = crc32_hash (key);
    size_t index = hash % table->size;

    ListNode* node = table->buckets[index];

    char temp[32] = {};
    memcpy (temp , key , 32);

    // printf ("\ntemp = ");
    // for (int i = 0 ; i < 32 ; i++) {

    //     if (temp[i] == '\0')
    //         printf ("\\0");

    //     else
    //         printf ("%c" , temp[i]);
    

    // }

    while (node) {

        if (fast_strcmp (node->key , temp) == 0) {
            return; 
        }
        node = node->next;
    }

   
    ListNode* new_node = (ListNode*)calloc (sizeof (ListNode) , 1);
    my_assert (new_node == NULL);
    new_node->key = temp;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    table->count++;

    VERIFY_TABLE(table);
}


bool HashTable_Find (const HashTable* table , const char* key)
{
    my_assert (table == NULL);
    my_assert (key == NULL);

    uint64_t hash = crc32_hash (key);
    size_t index = hash % table->size;

    ListNode* node = table->buckets[index];

    while (node) {

        if (fast_strcmp (node->key , key) == 0)
            return true;
        node = node->next;
    }                       

    return false;
}


uint64_t Hash_djb2 (const char* str)
{
    my_assert (str == NULL);

    uint64_t hash = 5381;       
    int symbol = 0;

    while (( symbol = *str++)) {
        hash = (( hash << 5) + hash) + symbol; 
    }

    return hash;
}


void HashTable_Init (HashTable* table , size_t element_count)
{   
    my_assert (table == NULL);

    table->size = element_count / LOAD_FACTOR + 1;
    table->buckets = (ListNode**) calloc (table->size , sizeof (ListNode*));
    my_assert (table->buckets == NULL);
    table->count = 0;
}


void HashTable_Destroy (HashTable* table)
{   
    my_assert (table == NULL);

    for (size_t i = 0; i < table->size; ++i) {
        ListNode* node = table->buckets[i];
        while (node) {
            ListNode* next = node->next;
            // free (node->key);
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
    

    for (size_t i = 0; i < table->size; ++i) {
        ListNode* node = table->buckets[i];
        if (!node) continue;  // пропускаем пустые бакеты

        printf ("Bucket [%zu]: " , i);
        while (node) {
            printf ("\"%s\" → " , node->key);
            node = node->next;
        }
        printf ("NULL\n");
    }
}


void HashTable_InsertFromFile (HashTable* table , const char* filename)
{
    my_assert (table == NULL);
    my_assert (filename == NULL);

    FILE* file = fopen (filename , "r");  
    if (!file) {
        perror ("Error opening file");
        return;
    }

    char word[40] = "";  

    while (fgets (word , sizeof (word) , file)) {
        word[strcspn (word , "\n")] = '\0';

        if (word[0] != '\0') {
            HashTable_Insert (table , word);  
        }
    }

    fclose (file);
}   


char* GenerateRandomWord (size_t length)
{
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t alphanum_size = sizeof (alphanum) - 1;

    char* word = (char*)calloc (length + 1 , 1);
    for (size_t i = 0; i < length; ++i) {

        int rand_index =    fast_rand () % alphanum_size;
        word[i] = alphanum[rand_index];
    }
    word[length] = '\0';
        
    return word;    
}


void GenerateRandomWordsForSearch (char** words_to_find , size_t word_count , size_t word_length)
{
    my_assert (words_to_find == NULL);

    for (size_t i = 0; i < word_count; ++i) {
        words_to_find[i] = GenerateRandomWord (word_length);
    }
}


void HashTable_FindWords (HashTable* table , char** words_to_find , size_t word_count)
{   
    my_assert (table == NULL);
    my_assert (words_to_find == NULL);

    for (size_t i = 0; i < word_count; ++i) {
             bool found = HashTable_Find (table , words_to_find[i]);
            // printf ("Find \"%s\" - %s\n" , words_to_find[i] , found ? "Found" : "Not Found");
    }
}


void HashTable_BucketStats (const HashTable* table , const char* filename)
{
    my_assert (table == NULL);
    my_assert (filename == NULL);
    

    FILE* out = fopen (filename , "w");  
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
        fprintf (out , "%zu\n" , count);  
    }

    fclose (out);
}



uint64_t rdtsc ()
{
    unsigned int lo , hi;
    __asm__ volatile ("rdtsc" : "=a" (lo) , "=d" (hi));
    return (( uint64_t) hi << 32) | lo;
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

int fast_strcmp (const char* str1 , const char* str2)
{
    int result = 0;

    
    __asm__ __volatile__ (

        ".intel_syntax noprefix \n\t"  
        
        "vmovdqu   ymm0 , [rdi]         \n\t"  
        "vmovdqu   ymm1 , [rsi]         \n\t"  
        
        "vpxor     ymm2 , ymm0 , ymm1    \n\t"  
        
        "vptest    ymm2 , ymm2          \n\t"  
        "jz        equal               \n\t"
        
        "mov       eax , 1              \n\t"
        
        "jmp       end                 \n\t"

        "equal:                        \n\t"
        
        "mov       eax , 0              \n\t"
        "end:                          \n\t"

        ".att_syntax noprefix \n\t"    
        : "=a" (result)               // Output: результат сравнения (0 или 1)
        : "D" (str1) , "S" (str2)      // Inputs: str1 в rdi , str2 в rsi
        : "ymm0" , "ymm1" , "ymm2"      
    );

    return result;
}

bool HashTable_Verify (const HashTable* table)
{
    if (!table) {
        fprintf (stderr , "HashTable is NULL\n");
        return false;
    }

    if (!table->buckets) {
        fprintf (stderr , "HashTable buckets are NULL\n");
        return false;
    }

    size_t real_count = 0;

    for (size_t i = 0; i < table->size; ++i) {
        ListNode* node = table->buckets[i];
        while (node) {
            real_count++;
            node = node->next;
        }
    }

    if (real_count != table->count) {
        fprintf (stderr , "Mismatch in count: expected %zu , found %zu\n" , table->count , real_count);
        return false;
    }

    return true;
}
       
