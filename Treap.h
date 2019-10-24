using namespace std;

template <typename T> struct Treap {
    T key;
    int priority;
    Treap<T> *left, *right;
    bool nil;
    int nr_nodes;

    Treap() : priority(-1), left(NULL), right(NULL), nil(true), nr_nodes(0) {}

    void addData(T key, int priority) {
        this->nil = false;
        this->key = key;
        this->priority = priority;
        this->nr_nodes = 1;
        this->left = new Treap();
        this->right = new Treap();
    }

    void delData() {
        this->nil = true;
        this->priority = -1;
        delete this->left;
        delete this->right;
        this->nr_nodes = 0;
    }

    bool isNil() {
        return this->nil;
    }

    bool find(T key) {
        if (this->isNil()) {
            return false;
        }

        Treap<T>* it = this;

        while (!it->nil) {
            if (it->key == key) {
                return true;
            } else if (it->key < key) {
                it = it->right;
            } else {
                it = it->left;
            }
        }

        return false;
    }

    void rotateRight(Treap<T> *&f) {
        Treap* l = f->left;
        f->left = l->right;
        l->right = f;
        
        f->nr_nodes = f->left->nr_nodes + f->right->nr_nodes + 1;
        l->nr_nodes = l->left->nr_nodes + l->right->nr_nodes + 1;

        f = l;
    }

    void rotateLeft(Treap<T> *&f) {
   	    Treap* r = f->right;
        f->right = r->left;
        r->left = f;

        f->nr_nodes = f->left->nr_nodes + f->right->nr_nodes + 1;
        r->nr_nodes = r->left->nr_nodes + r->right->nr_nodes + 1;

        f = r;
    }

    void insert(Treap<T> *&f, T key, int priority) {
        if (f->isNil()) {
            f->addData(key, priority);

            return;
        }

        if (key < f->key) {
            insert(f->left, key, priority);
        } else {
            insert(f->right, key, priority);
        }

        if (f->left->priority > f->priority) {
            rotateRight(f);
        } else if (f->right->priority > f->priority) {
            rotateLeft(f);
        }

        f->nr_nodes = f->left->nr_nodes + f->right->nr_nodes + 1;
    }

    void erase(Treap<T> *&f, T key) {
        if (f->isNil()) {
            return ;
        }

        bool notdel = true;

        if (key < f->key) {
            erase(f->left, key);
        } else if (key > f->key) {
            erase(f->right, key);
        } else if (f->left->isNil() && f->right->isNil()) {
            f->delData();
            notdel = false;
        } else {
            if (f->left->priority > f->right->priority) {
                rotateRight(f);
                erase(f->right, key);
            } else {
                rotateLeft(f);
                erase(f->left, key);
            }
        }

        if (notdel) {
            f->nr_nodes = f->left->nr_nodes + f->right->nr_nodes + 1;
        }
    }

    void dfs(Treap<T>* root) {
    	if (root->isNil()) {
    		return;
    	}

    	dfs(root->left);
    	cout << root->key << ' ';
    	dfs(root->right);
    }

    void inOrder() {
        if (this->isNil()) {
            return ;
        }

        dfs(this);
    }

    void dfs_(Treap<T>* root) {
    	if (root->isNil()) {
    		return;
    	}

    	cout << root->priority << ' ';
    	dfs(root->left);
    	dfs(root->right);
    }

    void preOrder(int level = 0) {
       	if (this->isNil()) {
            return ;
        }

        dfs(this);
    }

    T findK(int k) {
    	Treap<T>* it = this;

    	while (true) {
    		if (k == it->left->nr_nodes + 1) {
    			return it->key;
    		} else if (k > it->left->nr_nodes + 1) {
    			k -= it->left->nr_nodes + 1;
    			it = it->right;
    		} else {
    			it = it->left;
    		}
    	}

        return 0;
    }
};
