# MultiThreadSpellCheck

MultiThreadSpellCheck is a high-performance spell checking program built in C, utilizing multithreading to efficiently process multiple documents simultaneously. This program leverages Trie data structures for word storage and quick look-up, and implements a thread-safe approach using mutexes to manage shared resources effectively.

## Features

- **Concurrent Spell Checking**: Leverages pthreads to handle multiple files concurrently, speeding up the spell checking process over large datasets.
- **Trie Data Structure**: Utilizes a trie for efficient storage and retrieval of dictionary words, optimizing the spell checking process.
- **Thread Safety**: Implements mutex locks to ensure that file writing operations and access to shared counters are safely executed across multiple threads.
- **Scalability**: Designed to scale efficiently with the increase in the number of input files and dictionary size without compromising on performance.

## Getting Started

### Prerequisites

- GCC Compiler
- Linux-based OS for pthreads support

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/MultiThreadSpellCheck.git
   cd MultiThreadSpellCheck
   ```
2. Compile the program:
   ```bash
   make
   ./A2checker [-l]
   ```
- -l: Optional flag to log results to a file instead of displaying them on screen upon exit.

  ## Menu Navigation

  1. Start a new spellchecking task: Enter dictionary and text file names, and start the spell-checking in a new thread.
  2. Exit: Exit the program. If tasks are still running, their status will be displayed. Outputs a summary of all spell-checking tasks upon exiting.
 
  ## Exiting
  Upon exiting, if no tasks are running, the program will display or log:

- Total number of files processed.
- Total number of spelling errors encountered.
- The three most common misspellings across all tasks.
