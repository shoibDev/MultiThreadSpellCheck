#include "trie.h"
#include <stdlib.h>

TrieNode* createTrieNode(void) {
    TrieNode* pNode = (TrieNode*)malloc(sizeof(TrieNode));
    if (pNode) {
        int i;
        pNode->isEndOfWord = false;
        for (i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = NULL;
    }
    return pNode;
}

void insertWord(TrieNode* root, const char* word) {
    TrieNode* crawl = root;
    while (*word) {
        if (isalpha(*word)) { // Check if the character is alphabetic
            int index = tolower(*word) - 'a'; // Convert to lowercase and find index
            if (!crawl->children[index])
                crawl->children[index] = createTrieNode();
            crawl = crawl->children[index];
        }
        word++;
    }
    crawl->isEndOfWord = true;
}

int searchWord(TrieNode* root, const char* word) {
    TrieNode* crawl = root;
    while (*word) {
        if (isalpha(*word)) { // Check if the character is alphabetic
            int index = tolower(*word) - 'a'; // Convert to lowercase and find index
            if (!crawl->children[index])
                return 0;
            crawl = crawl->children[index];
        }
        word++;
    }
    return crawl != NULL && crawl->isEndOfWord;
}

void loadWordsFromFile(TrieNode* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    
    char word[100]; // Assuming a word won't exceed 99 characters
    while (fscanf(file, "%99s", word) == 1) {
        insertWord(root, word);
    }
    
    fclose(file);
}

// Hash function for a word
unsigned long hashWord(const char* word, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *word++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % capacity;
}

// Create a new hashmap
Hashmap* createHashmap(int capacity) {
    Hashmap* hashmap = (Hashmap*)malloc(sizeof(Hashmap));
    hashmap->capacity = capacity;
    hashmap->buckets = (HashmapEntry**)malloc(sizeof(HashmapEntry*) * capacity);
    for (int i = 0; i < capacity; i++)
        hashmap->buckets[i] = NULL;
    return hashmap;
}

// Insert or increment a word in the hashmap
void insertHashmap(Hashmap* hashmap, const char* word) {
    unsigned long index = hashWord(word, hashmap->capacity);
    HashmapEntry* entry = hashmap->buckets[index];
    while (entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            entry->count++;
            return;
        }
        entry = entry->next;
    }
    HashmapEntry* newEntry = (HashmapEntry*)malloc(sizeof(HashmapEntry));
    newEntry->word = strdup(word);
    newEntry->count = 1;
    newEntry->next = hashmap->buckets[index];
    hashmap->buckets[index] = newEntry;
}

// Print all entries in the hashmap
void printHashmap(Hashmap* hashmap) {
    for (int i = 0; i < hashmap->capacity; i++) {
        HashmapEntry* entry = hashmap->buckets[i];
        while (entry != NULL) {
            printf("%s: %d\n", entry->word, entry->count);
            entry = entry->next;
        }
    }
}

// Free the hashmap
void freeHashmap(Hashmap* hashmap) {
    for (int i = 0; i < hashmap->capacity; i++) {
        HashmapEntry* entry = hashmap->buckets[i];
        while (entry != NULL) {
            HashmapEntry* temp = entry;
            entry = entry->next;
            free(temp->word);
            free(temp);
        }
    }
    free(hashmap->buckets);
    free(hashmap);
}

void checkAndRecordMisspelledWord(TrieNode* root, Hashmap* hashmap, const char* word) {
    if (!searchWord(root, word)) {
        insertHashmap(hashmap, word);
    }
}

// Function to return the sum of all values in the hashmap
int sumHashmapValues(Hashmap* hashmap) {
    int sum = 0;
    for (int i = 0; i < hashmap->capacity; i++) {
        HashmapEntry* entry = hashmap->buckets[i];
        while (entry != NULL) {
            sum += entry->count;
            entry = entry->next;
        }
    }
    return sum;
}

void getTopMisspelledWords(Hashmap* hashmap, char topWords[3][256], int topCounts[3]) {
    // Initialize the top words and counts to zero
    memset(topWords, 0, sizeof(topWords[0][0]) * 3 * 256);
    memset(topCounts, 0, sizeof(topCounts[0]) * 3);

    for (int i = 0; i < hashmap->capacity; i++) {
        HashmapEntry* entry = hashmap->buckets[i];
        while (entry != NULL) {
            // Check against each top word
            for (int j = 0; j < 3; j++) {
                if (entry->count > topCounts[j]) {
                    // Shift the lower two words down
                    if (j < 2) {
                        memcpy(topWords[2], topWords[1], 256);
                        topCounts[2] = topCounts[1];
                    }
                    if (j < 1) {
                        memcpy(topWords[1], topWords[0], 256);
                        topCounts[1] = topCounts[0];
                    }

                    // Insert the new top word
                    strcpy(topWords[j], entry->word);
                    topCounts[j] = entry->count;
                    break; // Break since the entry has been placed
                }
            }
            entry = entry->next;
        }
    }
}

void freeTrie(TrieNode* root) {
    if (root == NULL)
        return;

    // Recursively free all children
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            freeTrie(root->children[i]);
        }
    }

    // Free the current node
    free(root);
}



