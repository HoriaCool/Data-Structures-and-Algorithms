// Copyright 2019 Nedelcu Horia (nedelcu.horia.alexandru@gmail.com)

/**
*    SkipList Implementation:
*   
*    I implemented the data structure so that it can store items
* of the same value without wasting memory (each node keeps the number
* of times the key appears, count). Also I wanted the data structure to
* supports random access by index and key, insertion and deletion in
* logarithmic time with order maintained. For this I have kept 2 LOGN SIZE
* vectors representing the path to find a node and the index of each node
* in the path (path, index_path, details in implementation).
*    Search to find by key / index:
*    The search starts at the highest level and attempts to move to the
* right (next) as long as the key is lower or the number of nodes that are
* skipped doesn't exceed the index.
*/

#ifndef SKIP_LIST_H_
#define SKIP_LIST_H_

#include <ctime>
#include <random>
#include <iostream>
#include <exception>

#define N_MAX 200000
#define H_MAX 20
#define JUMP_TO_NULL -1

template <class T> class DefaulComparator;
template <class T> class Node;
template <class T, class Comparator = DefaulComparator<T> > class SkipList;

template <class T>
class DefaulComparator {
 public:
    bool operator()(T const &lhs, T const &rhs) {
        return lhs > rhs;
    }
};

template <class T>
class Node {
 public:
    T data_;
    int count_;
    Node **next_;
    int *jump_;

    Node(T, int, int count_ = 1);
    Node(Node<T>&);
    ~Node();
    Node& operator=(Node<T>&);
};

template <class T, class Comparator>
class SkipList {
    int max_capacity_;
    int max_height_;
    int num_elem_;
    int num_nodes_;
    int *index_path_;
    Node<T> **path_, *head_;
    Comparator comp_;
    std::minstd_rand new_rand_;

 public:
    SkipList();
    SkipList(const SkipList<T, Comparator>&);
    ~SkipList();
    SkipList<T, Comparator>& operator=(const SkipList<T, Comparator>&);

    class iterator {
        Node<T>* itr;

     public:
        iterator();
        explicit iterator(Node<T>*);
        iterator(const iterator&);

        iterator& operator=(const iterator&);
        bool operator==(const iterator&);
        bool operator!=(const iterator&);
        iterator& operator++();
        const T& operator*();
    };

    iterator begin();
    iterator end();
    iterator findKey(const T&);

    int size();
    int length();
    int capacity();
    bool isEmpty();

    int countKey(const T&);
    bool searchKey(const T&);
    void insertKey(const T&, int count_ = 1);
    void eraseKey(const T&, int count_ = 1);

    const T& topKey();
    const T& operator[](int index);

 private:
    int getNewHeight();
};

/*
 * Implementation:
 */

template <class T>
Node<T> :: Node(T data, int height, int count): data_(data),
    count_(count), next_(nullptr), jump_(nullptr) {
    try {
        next_ = new Node<T>*[height]();
        jump_ = new int[height]();

        for (int i = 0; i < height; ++i) {
            jump_[i] = JUMP_TO_NULL;
        }
    } catch (std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << '\n';
    }
}

template <class T>
Node<T> :: ~Node() {
    delete[] next_;
    delete[] jump_;
}

/*
 * These are shallow copy assignments and constructor (Rule of three).
 * Each node doesn't preserve its height, because it doesn't need and
 * it is a waste of memory.
 */
template <class T>
Node<T> :: Node(Node<T>& other) = default;

template <class T>
Node<T>& Node<T> :: operator=(Node<T>& other) = default;

template <class T, class Comparator>
SkipList<T, Comparator> :: iterator ::
iterator(): itr(nullptr) {}

template <class T, class Comparator>
SkipList<T, Comparator> :: iterator ::
iterator(Node<T>* other_itr): itr(other_itr) {}

template <class T, class Comparator>
SkipList<T, Comparator> :: iterator ::
iterator(const iterator& other): itr(other.itr) {}

template <class T, class Comparator>
typename SkipList<T, Comparator> :: iterator&
SkipList<T, Comparator> :: iterator ::
operator=(const iterator& other) {
    itr = other.itr;
    return *this;
}

template <class T, class Comparator>
bool
SkipList<T, Comparator> :: iterator ::
operator== (const iterator& other) {
    return itr == other.itr;
}

template <class T, class Comparator>
bool
SkipList<T, Comparator> :: iterator ::
operator!= (const iterator& other) {
    return itr != other.itr;
}

template <class T, class Comparator>
typename SkipList<T, Comparator> :: iterator&
SkipList<T, Comparator> :: iterator ::
operator++() {
    itr = itr->next_[0];
    return *this;
}

template <class T, class Comparator>
const T&
SkipList<T, Comparator> :: iterator ::
operator*() {
    return itr->data_;
}

template <class T, class Comparator>
SkipList<T, Comparator> :: SkipList(): max_capacity_(N_MAX), max_height_(H_MAX),
    num_elem_(0), num_nodes_(0), index_path_(nullptr), path_(nullptr),
    head_(nullptr), comp_(), new_rand_() {
    unsigned int seed = static_cast<unsigned int>(time(nullptr));
    T tmp = T();

    try {
        head_ = new Node<T>(tmp, max_height_);
        path_ = new Node<T>*[max_height_];
        index_path_ = new int[max_height_]();
    } catch (std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << '\n';
    }

    new_rand_.seed(seed);
}

template <class T, class Comparator>
SkipList<T, Comparator> :: SkipList(const SkipList<T, Comparator>& other):
    max_capacity_(other.max_capacity_), max_height_(other.max_height_),
    num_elem_(other.num_elem_), num_nodes_(other.num_nodes_),
    index_path_(nullptr), path_(nullptr), head_(nullptr),
    comp_(), new_rand_() {
    unsigned int seed = static_cast<unsigned int>(time(nullptr));
    T tmp = T();

    try {
        head_ = new Node<T>(tmp, max_height_);
        path_ = new Node<T>*[max_height_];
        index_path_ = new int[max_height_]();
    } catch (std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << '\n';
    }

    SkipList<T, Comparator> :: iterator it;
    for (it = other.begin(); it != other.end(); ++it) {
        this->insertKey(*it);
    }

    new_rand_.seed(seed);
}

template <class T, class Comparator>
SkipList<T, Comparator> :: ~SkipList() {
    Node<T>* tmp;

    while (head_) {
        tmp = head_;
        head_ = head_->next_[0];
        delete tmp;
    }

    delete[] path_;
    delete[] index_path_;
}

template <class T, class Comparator>
SkipList<T, Comparator>&
SkipList<T, Comparator> :: operator=(const SkipList<T, Comparator>& other) {
    Node<T>* tmp_node;

    while (head_) {
        tmp_node = head_;
        head_ = head_->next_[0];
        delete tmp_node;
    }

    delete[] path_;
    delete[] index_path_;

    max_capacity_ = other.max_capacity_;
    max_height_ = other.max_height_;
    num_elem_ = other.num_elem_;
    num_nodes_ = other.num_nodes_;

    T tmp = T();

    try {
        head_ = new Node<T>(tmp, max_height_);
        path_ = new Node<T>*[max_height_];
        index_path_ = new int[max_height_]();
    } catch (std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << '\n';
    }

    SkipList<T, Comparator> :: iterator it;
    for (it = other.begin(); it != other.end(); ++it) {
        this->insertKey(*it);
    }
}

template <class T, class Comparator>
typename SkipList<T, Comparator> :: iterator
SkipList<T, Comparator> :: begin() {
    return iterator(head_->next_[0]);
}

template <class T, class Comparator>
typename SkipList<T, Comparator> :: iterator
SkipList<T, Comparator> :: end() {
    return iterator(nullptr);
}

/*
 * Search for key in skip list and return an iterator to its
 * position or end iterator otherwise.
 * O(logn)
 */
template <class T, class Comparator>
typename SkipList<T, Comparator> :: iterator
SkipList<T, Comparator> :: findKey(const T& key) {
    Node<T> *it = head_;

    for (int i = max_height_ - 1; i > -1; --i) {
        while (it->next_[i] && comp_(key, it->next_[i]->data_)) {
            it = it->next_[i];
        }
    }

    if (!it->next_[0] || key != it->next_[0]->data_) {
        return iterator(nullptr);
    } else {
        return iterator(it->next_[0]);
    }
}

template <class T, class Comparator>
int
SkipList<T, Comparator> :: size() {
    return num_elem_;
}

template <class T, class Comparator>
int
SkipList<T, Comparator> :: length() {
    return num_nodes_;
}

template <class T, class Comparator>
int
SkipList<T, Comparator> :: capacity() {
    return max_capacity_;
}

template <class T, class Comparator>
bool
SkipList<T, Comparator> :: isEmpty() {
    return num_nodes_ == 0;
}

/*
 * Search for key in skip list and returns how many
 * times the key is on the skip list.
 * O(logn)
 */
template <class T, class Comparator>
int
SkipList<T, Comparator> :: countKey(const T& key) {
    Node<T> *it = head_;

    for (int i = max_height_ - 1; i > -1; --i) {
        while (it->next_[i] && comp_(key, it->next_[i]->data_)) {
            it = it->next_[i];
        }
    }

    if (!it->next_[0] || key != it->next_[0]->data_) {
        return 0;
    } else {
        return it->next_[0]->count_;
    }
}

template <class T, class Comparator>
bool
SkipList<T, Comparator> :: searchKey(const T& key) {
    return countKey(key);
}

/*
 * Insert count keys in the skip list (count = 1 default).
 * O(logn)
 */
template <class T, class Comparator>
void
SkipList<T, Comparator> :: insertKey(const T& key, int count) {
    try {
        if (count < 1) {
            throw 1;
        }
    } catch (...) {
        std::cerr << "Standard exception: insertKey negative number of keys\n";
        return;
    }

    num_elem_ += count;

    int height, curr_index, old_jump;
    Node<T> *tmp, *node, *it;

    if (!searchKey(key)) {
        ++num_nodes_;

        height = getNewHeight();
        curr_index = 0;

        node = new Node<T>(key, height, count);
        it = head_;

        for (int i = max_height_ - 1; i > -1; --i) {
            while (it->next_[i] && comp_(key, it->next_[i]->data_)) {
                curr_index += it->jump_[i] + 1;
                it = it->next_[i];
            }

            // For each level retain the last node and its position in skiplist
            // jump_ specify the node level how many nodes skip
            index_path_[i] = curr_index;
            path_[i] = it;
        }


        // Nodes at the same height point to the
        // new node levels and change their jump_
        tmp = path_[0]->next_[0];
        path_[0]->next_[0] = node;
        node->next_[0] = tmp;

        old_jump = path_[0]->jump_[0];
        path_[0]->jump_[0] = 0;
        node->jump_[0] = old_jump;  // 0 or JUMP_TO_NULL

        for (int i = 1; i < height; ++i) {
            tmp = path_[i]->next_[i];
            path_[i]->next_[i] = node;
            node->next_[i] = tmp;

            old_jump = path_[i]->jump_[i];
            // The current node jumps as many nodes as it reaches the node on a
            // lower level path + as many nodes as it reaches the node inserted
            path_[i]->jump_[i] = index_path_[i - 1] - index_path_[i]
                + path_[i - 1]->jump_[i - 1];
            // The inserted node jumps over remained nodes or jumps to NULL
            node->jump_[i] = (old_jump == JUMP_TO_NULL)? JUMP_TO_NULL:
                old_jump - path_[i]->jump_[i];
        }

        // The rest of the path with higher levels jump over an extra node
        for (int i = height; i < max_height_; ++i) {
            if (path_[i]->jump_[i] != JUMP_TO_NULL) {
                ++path_[i]->jump_[i];
            }
        }
    } else {
        it = head_;

        for (int i = max_height_ - 1; i > -1; --i) {
            while (it->next_[i] && comp_(key, it->next_[i]->data_)) {
                it = it->next_[i];
            }
        }

        it->next_[0]->count_ += count;
    }
}

/*
 * Erase count keys from the skip list (count = 1 default).
 * O(logn)
 */
template <class T, class Comparator>
void
SkipList<T, Comparator> :: eraseKey(const T& key, int count) {
    try {
        if (count < 1) {
            throw 1;
        }
    } catch (...) {
        std::cerr << "Standard exception: eraseKey negative number of keys\n";
        return;
    }

    int get_count = countKey(key);
    Node<T> *tmp, *it = head_;

    if (get_count < 1) {
        return;
    }

    for (int i = max_height_ - 1; i > -1; --i) {
        while (it->next_[i] && comp(key, it->next_[i]->data_)) {
            it = it->next_[i];
        }

        path_[i] = it;
    }

    if (it->next_[0]->count_ > count) {
        num_elem_ -= count;
    } else {
        num_elem_ -= it->next_[0]->count_;
    }

    it->next_[0]->count_ -= count;
    if (it->next_[0]->count_ < 1) {
        --num_nodes_;

        tmp = it->next_[0];

        for (int i = 0; i < max_height_; ++i) {
            if (path_[i]->next_[i] != tmp) {
            	// jump less with one node
                if (path_[i]->next_[i]) {
                	// no jump to NULL
                    --path_[i]->jump_[i];
                }
            } else {
                path_[i]->next_[i] = tmp->next_[i];

                // check if now jump to NULL
                if (path_[i]->next_[i]) {
                    path_[i]->jump_[i] += tmp->jump_[i];
                } else {
                    path_[i]->jump_[i] = JUMP_TO_NULL;
                }
            }
        }

        delete tmp;
    }
}

template <class T, class Comparator>
const T&
SkipList<T, Comparator> :: topKey() {
    try {
        if (num_nodes_ < 1) {
            throw 1;
        }
    } catch (...) {
        std::cerr << "Standard exception: empty list\n";
    }

    return head_->next_[0]->data_;
}

/*
 * Return date from position index in skip list.
 * O(logn)
 */
template <class T, class Comparator>
const T&
SkipList<T, Comparator> :: operator[](int index) {
    try {
        if (index < 0 || num_nodes_ <= index) {
            throw 1;
        }
    } catch (...) {
        std::cerr << "Standard exception: index outside the bounds\n";
    }

    ++index;
    Node<T> *it = head_;
    for (int i = max_height_ - 1; i > -1; --i) {
        while (it->next_[i] && index >= it->jump_[i] + 1) {
            index -= it->jump_[i] + 1;
            it = it->next_[i];
        }
    }

    return it->data_;
}

template <class T, class Comparator>
int
SkipList<T, Comparator> :: getNewHeight() {
    int height, random;

    for (height = 1, random = new_rand_(); random & 1 && height < max_height_;
        random = (random >>= 1)? random: new_rand_(), ++height) {
    }

    return height;
}

/*
 * I've left commented code and the worse method to show that
 * the data structure supports random access by index and key,
 * insertion, deletion in logarithmic time with order maintained.
 */
template <class T, class Container>
int sortAlgorithm(T* vect, int size, bool distinct) {
    // typename Container :: iterator it;
    Container sklist;
    int i, j, k, count;

    // O(nlogn)
    for (i = 0; i < size; ++i) {
        sklist.insertKey(vect[i]);
    }

    // sort and eliminate replicates
    if (distinct == true) {
        // O(nlogn)
        for (i = 0; i < sklist.length(); ++i) {
            vect[i] = sklist[i];
        }

        // O(n)
        // for (i = 0, it = sklist.begin(); it != sklist.end(); ++it) {
        //    vect[i++] = *it;
        // }

        return sklist.length();
    } else {  // just sort
        // O(k*nlogn)
        for (i = j = 0; i < sklist.length(); ++i) {
            count = sklist.countKey(sklist[i]);

            for (k = 0; k < count; ++k) {
                vect[j++] = sklist[i];
            }
        }

        // O(k*n)
        // for (j = 0, it = sklist.begin(); it != sklist.end(); ++it) {
        //     count = sklist.countKey(*it);

        //     for (k = 0; k < count; ++k) {
        //         vect[j++] = *it;
        //     }
        // }

        return sklist.size();
    }
}

/*
 * This sorting functions return also the size if it was changed
 * by calling them with the parameter distinct equal to true.
 */
template <class T, class Comparator>
int skipListSort(T* vect, int size, bool distinct = false) {
    return sortAlgorithm<T, SkipList<T, Comparator> >(vect, size, distinct);
}

template <class T>
int skipListSort(T* vect, int size, bool distinct = false) {
    return sortAlgorithm<T, SkipList<T> >(vect, size, distinct);
}

#endif  // SKIP_LIST_H_
