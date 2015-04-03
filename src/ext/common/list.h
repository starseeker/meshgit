#ifndef LIST_H
#define LIST_H

// TODO: finished the list container to substitute the stl in lru.

template<typename T>
struct slist {
    struct node {
        node() : value() {}
        node(const T& t) : value(t) {}
        node*   prev;
        node*   next;
        T       value;
    };
    
    slist() { _empty_init(); }
    slist(const slist& rhs) {
        _empty_init();
        const node* n = rhs.head();
        while(n != rhs.tail()) {
            push_back(n->value);
            n = n->next;
        }
    }
    ~slist() { clear(); }
    
    void push_back(const T& v) {
        insert(_n, v); 
    }
    
    void push_front(const T& v) {
        insert(_n->next, v);
    }
    
    void pop_front() {
        remove(head());
    }
    
    void pop_back() {
        remove(last());
    }
    
    node* insert(node* p, const T& v) {
        node* t = new node(v);
        insert_node(p, t);
        return t;
    }
    
    void insert_node(node* p, node* t) {
        t->next = p;
        t->prev = p->prev;
        p->prev->next = t;
        p->prev = t;
        _l++; 
    }
    
    void remove(node* p) {
        remove_node(p);
        delete p;
    }
    
    void remove_node(node* p) {
        if(empty()) return;
        p->prev->next = p->next;
        p->next->prev = p->prev;
        _l--;
    }
    
    void move(node* p, node *t) {
        if (empty()) return;
        if (p == t) return;
        remove_node(t);
        insert_node(p, t);
    }
    
    bool empty() { return _n->next == _n->prev; }
    
    void clear() {
        if (!empty()) {
            node *n = _n->next;
            while(n != _n) {
                node* t = n;
                n = n->next;
                delete t;
            }
            _empty_init();
        }
    }
    
    void _empty_init() {
        _n = &empty_node;
        _n->next = _n;
        _n->prev = _n;
        _l = 0;
    }
    
    node* tail() {
        return _n;
    }
    
    const node* tail() const {
        return _n;
    }
    
    node* head() {
        return _n->next;
    }
    
    const node* head() const {
        return _n->next;
    }
    
    node* last() {
        return _n->prev;
    }
    
    const node* last() const {
        return _n->prev;
    }

    node    empty_node;
    int     _l;
    node*   _n;
};

#endif // LIST_H