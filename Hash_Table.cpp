#include "My_features.h"
#include <cstddef>
#include <cstdint>
#include <time.h>
const int LOAD_FACTOR = 15;

struct ListNode {
    char* key;
    ListNode* next;
};

struct HashTable {
    ListNode** buckets;
    size_t size;      
    size_t count;     
};

uint64_t Hash_djb2(const char* str)
{
    uint64_t hash = 5381;       
    int c = 0;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

void HashTable_Init(HashTable* table, size_t element_count)
{
    table->size = element_count / LOAD_FACTOR + 1;
    table->buckets = (ListNode**)calloc(table->size, sizeof(ListNode*));
    table->count = 0;
}


void HashTable_Insert(HashTable* table, const char* key)
{
    uint64_t hash = Hash_djb2(key);
    size_t index = hash % table->size;

    ListNode* node = table->buckets[index];

    // Проверка на дубликат (поиск)
    while (node) {
        if (strcmp(node->key, key) == 0) {
            return; // уже существует
        }
        node = node->next;
    }

    // Вставка в начало списка
    ListNode* new_node = (ListNode*)calloc(sizeof(ListNode)     , 1);
    new_node->key = strdup(key);
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    table->count++;
}


bool HashTable_Find(const HashTable* table, const char* key)
{
    uint64_t hash = Hash_djb2(key);
    size_t index = hash % table->size;

    ListNode* node = table->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0)
            return true;
        node = node->next;
    }

    return false;
}


void HashTable_Destroy(HashTable* table)
{
    for (size_t i = 0; i < table->size; ++i) {
        ListNode* node = table->buckets[i];
        while (node) {
            ListNode* next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }

    free(table->buckets);
    table->buckets = NULL;
    table->size = 0;
    table->count = 0;
}

void HashTable_Dump(const HashTable* table)
{
    if (!table || !table->buckets) return;

    for (size_t i = 0; i < table->size; ++i) {
        ListNode* node = table->buckets[i];
        if (!node) continue;  // пропускаем пустые бакеты

        printf("Bucket [%zu]: ", i);
        while (node) {
            printf("\"%s\" → ", node->key);
            node = node->next;
        }
        printf("NULL\n");
    }
}

void HashTable_InsertFromFile(HashTable* table, const char* filename)
{
    FILE* file = fopen(filename, "r");  
    if (!file) {
        perror("Error opening file");
        return;
    }

    char word[256] = "";  

    
    while (fgets(word, sizeof(word), file)) {
        
        word[strcspn(word, "\n")] = '\0';

        
        if (word[0] != '\0') {

            HashTable_Insert(table, word);  
        }
    }

    fclose(file);
}   

char* GenerateRandomWord(size_t length)
{
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t alphanum_size = sizeof(alphanum) - 1;

    char* word = (char*)calloc(length + 1 , 1);
    int rand_index = rand() % alphanum_size;
    for (size_t i = 0; i < length; ++i) {
        word[i] = alphanum[rand_index];
    }
    word[length] = '\0';

    return word;
}

// Генерация массива случайных слов для поиска
void GenerateRandomWordsForSearch(char** words_to_find, size_t word_count, size_t word_length)
{
    for (size_t i = 0; i < word_count; ++i) {
        words_to_find[i] = GenerateRandomWord(word_length);
    }
}

// Функция для поиска слов в таблице
void HashTable_FindWords(HashTable* table, char** words_to_find, size_t word_count)
{
    for (size_t i = 0; i < word_count; ++i) {
        bool found = HashTable_Find(table, words_to_find[i]);
        // printf("Find \"%s\" - %s\n", words_to_find[i], found ? "Found" : "Not Found");
    }
}

uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int main()
{   
    srand(time(0)); // Инициализация генератора случайных чисел

    HashTable table = {};
    HashTable_Init(&table, 1000000); 

    
    size_t search_word_count = 600000;  
    char** words_to_find = (char**)calloc(search_word_count, sizeof(char*));
    GenerateRandomWordsForSearch(words_to_find, search_word_count, 5);

    uint64_t start = rdtsc();   
    HashTable_InsertFromFile(&table, "words.txt");

    
    HashTable_FindWords(&table, words_to_find, search_word_count);
    uint64_t end = rdtsc();
    
    printf("Ticks_0: %llu\n", end - start);

    
    for (size_t i = 0; i < search_word_count; ++i) {
        free(words_to_find[i]);
    }
    free(words_to_find);

    HashTable_Destroy(&table);
    return 0;
}
