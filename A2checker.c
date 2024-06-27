#include "trie.h"
#include <pthread.h>


// Global synchronization objects
pthread_mutex_t fileWriteMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t activeThreadsMutex = PTHREAD_MUTEX_INITIALIZER;

// Global counters
int activeThreads = 0;
int filesProcessed = 0;

// Function declarations
void* spellCheckingTask(void* arg);
void displaySubMenu(Hashmap* hashmap);
void waitForThreadsAndSummarize(Hashmap* hashmap);
void displayOrSaveSummary(Hashmap* hashmap, bool saveToFile);
void writeTaskResultToFile(const char* inputFile, Hashmap* threadmap);

int main(int argc, char *argv[]) {
    Hashmap* hashmap = createHashmap(100); // Assuming 100 buckets
    bool logEnabled = false;

    // Process command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            logEnabled = true;
            break;
        }
    }

    while (true) {
        printf("\nMain Menu\n1. Start a new spellchecking task\n2. Exit\nEnter your choice: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displaySubMenu(hashmap);
                break;
            case 2:
                waitForThreadsAndSummarize(hashmap);
                displayOrSaveSummary(hashmap, logEnabled);
                freeHashmap(hashmap);
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void displaySubMenu(Hashmap* hashmap) {
    char dictFile[256], inputFile[256];
    printf("\nEnter dictionary file name (or type 'back' to return): ");
    if (scanf("%s", dictFile) && strcmp(dictFile, "back") == 0) return;

    printf("Enter input text file name (or type 'back' to return): ");
    if (scanf("%s", inputFile) && strcmp(inputFile, "back") == 0) return;

    SpellCheckTaskArgs* args = (SpellCheckTaskArgs*)malloc(sizeof(SpellCheckTaskArgs));
    if (!args) {
        printf("Failed to allocate memory for thread arguments.\n");
        return;
    }

    // Initialize task arguments
    args->root = createTrieNode();
    args->hashmap = hashmap;
    strncpy(args->dictFile, dictFile, sizeof(args->dictFile) - 1);
    args->dictFile[sizeof(args->dictFile) - 1] = '\0';
    strncpy(args->inputFile, inputFile, sizeof(args->inputFile) - 1);
    args->inputFile[sizeof(args->inputFile) - 1] = '\0';

    // Create and detach thread for spell checking
    pthread_t tid;
    if (pthread_create(&tid, NULL, spellCheckingTask, (void*)args) != 0) {
        printf("Error creating thread.\n");
        free(args);
    } else {
        pthread_detach(tid);
        pthread_mutex_lock(&activeThreadsMutex);
        activeThreads++;
        pthread_mutex_unlock(&activeThreadsMutex);
        printf("Spell-checking task started...\n");
    }
}

void waitForThreadsAndSummarize(Hashmap* hashmap) {

    pthread_mutex_lock(&activeThreadsMutex);
    if (activeThreads > 0) {
        printf("Waiting for %d running threads to finish...\n", activeThreads);
        while (activeThreads > 0);
    }
    pthread_mutex_unlock(&activeThreadsMutex);

    printf("All threads have finished running.\n");
}

void* spellCheckingTask(void* arg) {
    SpellCheckTaskArgs* taskArgs = (SpellCheckTaskArgs*)arg;
    Hashmap* threadmap = createHashmap(100); // Separate map for thread-specific results
    
    // Load dictionary and process input file
    loadWordsFromFile(taskArgs->root, taskArgs->dictFile);

    FILE* file = fopen(taskArgs->inputFile, "r");
    if (!file) {
        perror("Error opening input file");
        pthread_exit(NULL);
    }

    char word[256]; 
    while (fscanf(file, "%255s", word) == 1) {
        checkAndRecordMisspelledWord(taskArgs->root, taskArgs->hashmap, word);
        checkAndRecordMisspelledWord(taskArgs->root, threadmap, word); // current process :D
    }

    // Update global state and write task results
    pthread_mutex_lock(&activeThreadsMutex);
    filesProcessed++;
    activeThreads--;
    pthread_mutex_unlock(&activeThreadsMutex);
    
    writeTaskResultToFile(taskArgs->inputFile, threadmap);

    // Clean up
    fclose(file);
    freeTrie(taskArgs->root);
    free(taskArgs);
    freeHashmap(threadmap);

    pthread_exit(NULL);
}

void writeTaskResultToFile(const char* inputFile, Hashmap* threadmap) {
    char topWords[3][256] = {{0}};
    int topCounts[3] = {0};
    getTopMisspelledWords(threadmap, topWords, topCounts);

    pthread_mutex_lock(&fileWriteMutex);
    FILE* file = fopen("mwahab_A2.out", "a"); // Append mode
    if (!file) {
        perror("Error opening output file");
    } else {
        int totalErrors = sumHashmapValues(threadmap);
        fprintf(file, "%s %d %s %s %s\n", inputFile, totalErrors, topWords[0], topWords[1], topWords[2]);
        fclose(file);
    }
    pthread_mutex_unlock(&fileWriteMutex);
}

void displayOrSaveSummary(Hashmap* hashmap, bool saveToFile) {
    char summary[1024];
    char topWords[3][256];
    int topCounts[3];
    getTopMisspelledWords(hashmap, topWords, topCounts);
    
    sprintf(summary, "Number of files processed: %d\nNumber of spelling errors: %d\nThree most common misspellings: %s (%d times), %s (%d times), %s (%d times)\n",
            filesProcessed, sumHashmapValues(hashmap), topWords[0], topCounts[0], topWords[1], topCounts[1], topWords[2], topCounts[2]);
    
    if (saveToFile) {
        pthread_mutex_lock(&fileWriteMutex);
        FILE* file = fopen("mwahab_A2.out", "a");
        fprintf(file, summary);
        pthread_mutex_unlock(&fileWriteMutex);
    } else {
        printf("%s", summary);
    }
}
