#ifndef __TRIE_H
#define __TRIE_H

#include <iostream> 
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

int ALPHABET_SIZE = 26;

template <typename T>
class Trie
{
 private:
 public:
    int count;
    std::vector<Trie<T> *> children;
    T value;
    bool isEndOfWord;
  public:
    Trie() {}
    
    Trie(int capacity, T value)
      : count(0),
        children(capacity, NULL),
        value(value) { }
        
    Trie(int capacity)
      : count(0),
        children(capacity, NULL) { }
        
    ~Trie() {}

 	void insert(std::string key, T value) {
		Trie<T> *it = this;

		for (int i = 0; i < (int) key.size(); ++i) {
			if (it->children[key[i] - 'a']) {
				it = it->children[key[i] - 'a'];
				it->count++;
			} else {
				it->children[key[i] - 'a'] = new Trie(ALPHABET_SIZE);
				it = it->children[key[i] - 'a'];
				it->count = 1;
				it->isEndOfWord = false;
			}
		}

		it->value = value;
		it->isEndOfWord = true;
    }

	bool search(std::string key, T &val) {
 		Trie<T> *it = this;

 		for (int i = 0; i < (int) key.size(); ++i) {
 			if (!it->children[key[i] - 'a']) {
 				return false;
 			} else {
 				it = it->children[key[i] - 'a'];
 			}
 		}

 		val = it->value;
        return it->isEndOfWord;
    }
    
    bool remove(std::string key) {
    	T tmp;
    	if (!search(key, tmp)) {
    		return false;
    	}

    	Trie<T> *ant_it, *it = this;

    	for (int i = 0; i < (int) key.size(); ++i) {
    		if (it->children[key[i] - 'a']) {
				ant_it = it;
				it = it->children[key[i] - 'a'];
				it->count--;

				if (!it->count) {
					ant_it->children[key[i] - 'a'] = NULL;
				}

				if (ant_it != this && !ant_it->count) {
					delete ant_it;
				}
			}
		}

		it->isEndOfWord = false;

		return true;
    }
    
    int numWordsWithPrefix(std::string prefix) {
    	Trie<T> *it = this;

    	if (prefix == "") {
    		int sum_count = 0;
    		for (int i = 0; i < ALPHABET_SIZE; ++i) {
    			if (this->children[i]) {
    				sum_count += this->children[i]->count;
    			}
    		}
    		return sum_count;
    	}

 		for (int i = 0; i < (int) prefix.size(); ++i) {
 			if (!it->children[prefix[i] - 'a']) {
 				return 0;
 			} else {
 				it = it->children[prefix[i] - 'a'];
 			}
 		}

        return it->count;
    }
  
};
#endif
