A2checker:
	gcc -o A2checker A2checker.c trie.c -Wall -g -std=c11 -lpthread

clean:
	rm -f A2checker
