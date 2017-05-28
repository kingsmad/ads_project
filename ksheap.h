/*************************************************************************
    > In god we trust
    > File Name: ksheap.h
************************************************************************/
#include <vector>
using namespace std;

struct KSHeap {
    virtual int top() = 0;
    virtual void pop() = 0;
    virtual int size() = 0;
    virtual void push(int, int) = 0;
    virtual void init() = 0;
};

struct BinomialHeap : public KSHeap {
    int size();
    int top();
    void pop();
    void push(int x, int w);
    BinomialHeap();
private:
    int get_newnode(int, int);
    void merge(int x);
    void init();
    int find_min();
private:
    int ttl[32], nsz;
    vector<int> w, next, ch, ord, val, fa;
    vector<int> idle_nodes;
};

struct BinaryHeap : public KSHeap {
    vector<int> vb, w, val, idle_nodes;
    int nsz;
private:
    bool cmp(int x, int y);
    int get_newnode(int, int);
    void init();
    inline int lc(int o) { return 2*o+1;}
    inline int rc(int o) { return 2*o+2;}
public:
    int top();
    int size();
    void pop();
    void push(int sid, int w1);
    BinaryHeap();
};

struct FourwayHeap : public KSHeap {
    vector<int> vb, w, val, idle_nodes;
    int nsz;
private:
    bool cmp(int x, int y);
    int get_newnode(int, int);
    void init();
    int ch(int o, int k);
    int find_pa(int o);
public:
    int top();
    int size();
    void pop();
    void push(int sid, int w1);
    FourwayHeap();
};

struct PairingHeap : public KSHeap {
    vector<int> vb, w, next, pre, \
        idle_tags, val, ch; 
    int root, nsz;
    int top();
    int size();
    void pop();
    void push(int x, int w1);
    PairingHeap();
private:
    int get_newnode(int x, int w1);
    void init();
    int meld(int x, int y);
};
