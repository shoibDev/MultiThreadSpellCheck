#ifndef TRIE_H
#define TRIE_H
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ALPHABET_SIZE 26

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;
} TrieNode;

TrieNode* createTrieNode(void);
void insertWord(TrieNode* root, const char* word);
int searchWord(TrieNode* root, const char* word);
void loadWordsFromFile(TrieNode* root, const char* filename);

typedef struct HashmapEntry {
    char* word;
    int count;
    struct HashmapEntry* next;
} HashmapEntry;

typedef struct Hashmap {
    HashmapEntry** buckets;
    int capacity;
} Hashmap;

// Function declarations for the hashmap
Hashmap* createHashmap(int capacity);
void insertHashmap(Hashmap* hashmap, const char* word);
void printHashmap(Hashmap* hashmap);
void freeHashmap(Hashmap* hashmap);

void checkAndRecordMisspelledWord(TrieNode* root, Hashmap* hashmap, const char* word);

typedef struct SpellCheckTaskArgs {
    TrieNode* root;
    Hashmap* hashmap;
    char dictFile[256];
    char inputFile[256];
} SpellCheckTaskArgs;


int sumHashmapValues(Hashmap* hashmap);

void getTopMisspelledWords(Hashmap* hashmap, char topWords[3][256], int topCounts[3]);
void freeTrie(TrieNode* root);
#endif // TRIE_H
