/*************************************************************************
    > In god we trust
    > File Name: ksheap.cpp
************************************************************************/
//const bool debug = false;
#include "ksheap.h"
#include <assert.h>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <stack>
#include <queue>
#include <climits>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstring>
#include <algorithm>
#include <stdarg.h>
using namespace std;
#define per(i,a,n) for(int i=n-1;i>=a;--i)
#define rep(i,a,n) for(int i=a;i<n;++i)
#define erep(i,a,n) for(int i=a;i<=n;++i)
#define fi first
#define se second
#define all(x) (x).begin(), (x).end()
#define pb push_back
#define mp make_pair
typedef long long ll;
typedef vector<int> VI;
typedef pair<int, int> PII;
/*ll powmod(ll a,ll b, ll MOD) {ll res=1;a%=MOD;for(;b;b>>=1){if(b&1)res=res*a%MOD;a=a*a%MOD;}return res;}
void buginfo(const char* f, ...) {if(!debug)return;va_list al; va_start(al, f);vprintf(f, al);va_end(al);}*/
/*------------------ head-------------------*/

BinomialHeap::BinomialHeap() {
    init();
}

void BinomialHeap::init() {
    nsz = 0;
    memset(ttl, -1, sizeof(ttl));
    w.clear(); next.clear(); ch.clear(); ord.clear();
    val.clear(); fa.clear();
    idle_nodes.clear();
}

int BinomialHeap::size() {
    return nsz - idle_nodes.size();
}

int BinomialHeap::find_min() {
    int ans = -1;
    rep(i, 0, 32) if (ttl[i]!=-1) {
        if (ans == -1) ans = ttl[i];
        else if (w[ttl[i]] < w[ans]) ans=ttl[i];
    }
    return ans;
}

int BinomialHeap::top() {
    return val[find_min()];
}

void BinomialHeap::pop() {
    int p = find_min(), od = ord[p];
    assert(ttl[od]==p);
    ttl[od] = -1;

    /* a bug was here, similar to the case that do
     * not modify anything during traversing on a map*/
    vector<int> vr;
    for(int i=ch[p]; i!=-1; i=next[i]) vr.pb(i);
    for (int i: vr) { fa[i]=-1; next[i]=-1; merge(i);}

    idle_nodes.pb(p);
    w[p]=0, next[p]=-1, ch[p]=-1; 
    fa[p] = -1, ord[p]=0, val[p]=-1;
}

int BinomialHeap::get_newnode(int v1, int w1) {
    if (!idle_nodes.empty()) {
        int p = idle_nodes.back();
        idle_nodes.pop_back();

        val[p] = v1, w[p] = w1;
        next[p] = ch[p] = -1;
        ord[p] = 0;
        fa[p] = -1;
        return p;
    }

    val.pb(v1), w.pb(w1); 
    next.pb(-1); ch.pb(-1); 
    ord.pb(0); fa.pb(-1);
    return nsz++;
}

void BinomialHeap::merge(int x) {
    int p = ord[x]; 
    while(p<32) {
        if (ttl[p] == -1) {
            ttl[p] = x;
            break;
        } else {
            int y = ttl[p];
            assert(y != x);
            if (w[x] < w[y]) {
                int tmp = ch[x];
                ch[x] = y;
                next[y] = tmp;
                ++ord[x]; fa[y] = x;
            } else {
                int tmp = ch[y];
                ch[y] = x;
                next[x] = tmp;
                ++ord[y]; fa[x] = y;
                x = y; /*change x to new root*/
            }
            ttl[p++] = -1;
        }
    }
}

void BinomialHeap::push(int v1, int w1) {
    int x = get_newnode(v1, w1);
    merge(x);
}

BinaryHeap::BinaryHeap() {
    init();
}

void BinaryHeap::init() {
    nsz = 0;
    vb.clear(); w.clear(); val.clear();
    idle_nodes.clear();
}

int BinaryHeap::top() {
    return val[vb.front()];
}

int BinaryHeap::size() {
    return vb.size();
}

void BinaryHeap::pop() {
    swap(vb[0], vb.back());
    idle_nodes.pb(vb.back());
    vb.pop_back();

    int o = 0, len=vb.size(), q;
    while(1) {
        if (lc(o)>=len && rc(o)>=len) break;
        if (lc(o)<len && rc(o)<len)
            q = (cmp(lc(o), rc(o))) ? lc(o):rc(o);
        else if (lc(o) < len) q = lc(o);
        else q = rc(o);

        if (o == q) {
            printf("HHH");
            //exit(0);
        }
        if (cmp(o, q)) break;
        swap(vb[o], vb[q]);
        o = q;
    }
}

bool BinaryHeap::cmp(int x, int y) {
    return w[vb[x]] < w[vb[y]]; 
}

int BinaryHeap::get_newnode(int x, int w1) {
    int r;
    if (!idle_nodes.empty()) {
        r = idle_nodes.back();
        idle_nodes.pop_back();
        w[r] = w1, val[r] = x;
    } else {
        assert(nsz == w.size());
        r = nsz++;
        w.pb(w1); val.pb(x);
    }

    return r;
}

void BinaryHeap::push(int x, int w1) {
    int u = get_newnode(x, w1);
    vb.pb(u);

    int p = vb.size() - 1;
    while(cmp(p, (p-1)/2)) {
        swap(vb[p], vb[(p-1)/2]);
        p = (p-1)/2;
    }
}

FourwayHeap::FourwayHeap() {
    init();
}

void FourwayHeap::init() {
    nsz = 0;
    vb.clear(); vb.pb(-1); vb.pb(-1); vb.pb(-1);
    w.clear(); val.clear();
    idle_nodes.clear();
}

int FourwayHeap::top() {
    return val[vb[3]];
}

int FourwayHeap::size() {
    return vb.size() - 3;
}

bool FourwayHeap::cmp(int x, int y) {
    return w[vb[x]] < w[vb[y]];
}

void FourwayHeap::pop() {
    swap(vb[3], vb[vb.size()-1]);
    //int d = vb.back();
    idle_nodes.pb(vb.back());
    vb.pop_back();

    int o = 3, len=vb.size(), q=-1;
    while(1) {
        rep(i, 1, 5) { /* 1-th to 4-th*/
            int x = ch(o, i);
            if (x >= len || !cmp(x, o)) continue;
            if (q==-1) q = x; 
            else if (cmp(x, q)) q = x;
            else continue;
        }
        if (q == -1) break;
        swap(vb[o], vb[q]);
        o = q; q = -1;
    }
}

int FourwayHeap::get_newnode(int x, int w1) {
    int r;
    if (!idle_nodes.empty()) {
        r = idle_nodes.back();
        idle_nodes.pop_back();
        w[r] = w1, val[r] = x;
    } else {
        assert(nsz == w.size());
        r = nsz++;
        w.pb(w1); val.pb(x);
    }

    return r;
}

/* node o, k-th child, 1<=k<=4*/
int FourwayHeap::ch(int o, int k) { 
    o -= 3;
    o = 4 * o + k;
    return o + 3;
}

int FourwayHeap::find_pa(int o) {
    return (o-4)/4 + 3;
}

void FourwayHeap::push(int x, int w1) {
    int u = get_newnode(x, w1);
    vb.pb(u);

    int p = vb.size() - 1;
    while(p>=3 && cmp(p, find_pa(p))) {
        swap(vb[p], vb[find_pa(p)]);
        p = find_pa(p);
    }
}

PairingHeap::PairingHeap() {
    init();
}

void PairingHeap::init() {
    root = -1;
    w.clear(); next.clear(); nsz = 0;
    idle_tags.clear(); val.clear(); ch.clear();
    root = -1;
}

int PairingHeap::top() {
    assert(root != -1);
    return val[root];
}

int PairingHeap::size() {
    return nsz - idle_tags.size();
}

void PairingHeap::pop() {
    idle_tags.pb(root);
    vector<int> vr, v1;
    for(int i=ch[root]; i!=-1; i=next[i]) vr.pb(i);
    for(int i: vr) next[i] = -1;
    if (vr.size()==0) { assert(size()==0); root=-1; return;}
    if (vr.size()==1) { root=vr.front(); return;}

    /* first round */
    for (int i=0; i<vr.size(); i+=2)
        if (i+1 < vr.size())
            v1.pb(meld(vr[i], vr[i+1]));
        else v1.pb(vr[i]);

    /* second round */
    root = v1.back();
    per(i, 0, v1.size()-1) 
        root = meld(v1[i], root);
}

int PairingHeap::meld(int x, int y) {
    if (w[x] < w[y]) swap(x, y);
    int tmp = ch[y];
    ch[y] = x;
    next[x] = tmp;
    return y;
}

void PairingHeap::push(int x, int w1) {
    int u = get_newnode(x, w1);
    root = (root==-1) ? u : meld(root, u);
}

int PairingHeap::get_newnode(int x, int w1) {
    if (!idle_tags.empty()) {
        int r = idle_tags.back();
        idle_tags.pop_back();
        w[r] = w1, val[r] = x;
        next[r] = ch[r] = -1;
        return r;
    }
    int r = nsz++;
    w.pb(w1); val.pb(x); next.pb(-1); ch.pb(-1);
    return r;
}
