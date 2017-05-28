/*************************************************************************
    > In god we trust
    > File Name: heap_benchmark.cpp
************************************************************************/
#include <cstdio>
#include "ksheap.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include <stack>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <climits>
#include <stdarg.h>
#include <unistd.h>
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
#define lc(o) (o<<1)
#define rc(o) (o<<1|1)
//ll powmod(ll a,ll b, ll MOD) {ll res=1;a%=MOD;for(;b;b>>=1){if(b&1)res=res*a%MOD;a=a*a%MOD;}return res;}
//void buginfo(const char* f, ...) {if(!debug)return;va_list al; va_start(al, f);vprintf(f, al);va_end(al);}
/*------------------ head-------------------*/
const int maxn = 1e6 + 10;
//const int maxbitsz = 1024;

struct FakeEncoder {
    int in_fd, tfd, compfd, hs[maxn], root, nsz;
    int w[maxn*2], v[maxn*2], lc[maxn*2], rc[maxn*2], fa[maxn*2];
    string st[maxn];

    /* Heaps for testing*/

    void encode(KSHeap* Q) {
        lseek(in_fd, 0, SEEK_SET);
        FILE* fp = fdopen(in_fd, "r");
        int d;
        while(fscanf(fp, "%d", &d)==1) ++hs[d];

        /*build Huffman-tree*/
        rep(i, 0, maxn) {
            if (!hs[i]) continue;
            int x = nsz++;
            v[x] = i; w[x] = hs[i];
            Q->push(x, w[x]);
        }

        while(Q->size() >= 2) {
            int x = Q->top(); Q->pop();
            int y = Q->top(); Q->pop();
            
            int z = nsz++;
            lc[z] = x, rc[z] = y;
            w[z] = w[x] + w[y];
            fa[x] = z, fa[y] = z;
            v[z] = -1;

            Q->push(z, w[z]);
        }

        root = Q->top(); Q->pop();

        /* print table to file*/
        //print_table(root);
    }

    void init(const char* input, const char* op_table, const char* op_encode) {
        if (in_fd) close(in_fd);
        if (tfd) close(tfd);
        if (compfd) close(compfd);

        in_fd = open(input, O_RDONLY, 0644);
        tfd = open(op_table, O_WRONLY|O_CREAT|O_TRUNC, 0644);
        compfd = open(op_encode, O_WRONLY|O_CREAT|O_TRUNC, 0644);

        if (in_fd==-1 || tfd==-1 || compfd==-1) {
            printf("Opening wrong input file for encoding...\n");
            return;
        }

        nsz = 1; root = -1;
        fill(st, st+maxn, "");
        memset(hs, 0, sizeof(hs));
        memset(w, 0, sizeof(w));
        memset(v, -1, sizeof(v));
        memset(lc, -1, sizeof(lc));
        memset(rc, -1, sizeof(rc));
        memset(fa, -1, sizeof(fa));
    }

    void print_table(int root) {
        rep(u, 0, maxn) if (v[u]!=-1) {
            string tmp; int c = u;
            while(c!=root) {
                if (lc[fa[c]]==c) tmp.pb('0');
                else tmp.pb('1');
                c = fa[c];
            }
            reverse(all(tmp));
            st[v[u]] = tmp;
        }

        /* print the compress-table as txt*/
        lseek(tfd, 0, SEEK_SET);
        FILE* fp = fdopen(tfd, "w");
        rep(i, 0, maxn) if (st[i]!="") {
            fprintf(fp, "%d ===> %s\n", i, st[i].c_str());
        }
        fflush(fp);
    }

} fake_encoder;

BinomialHeap Q1;
BinaryHeap Q2;
FourwayHeap Q3;
PairingHeap Q4;

int main(int argc, char** argv) {
    if (!argv[1]) 
        return 0 * printf("Please assign input files\n");

    fake_encoder.init(argv[1], "code_table.txt", "encoded.bin");
    clock_t s_tm, rtm;

    KSHeap* Q = &Q1;  
    rtm = 0;
    printf("Checking BinomialHeap building huffman-tree for 10 times\n");
    rep(i, 0, 10)  {
        fake_encoder.init(argv[1], "code_table.txt", "encoded.bin");
        Q->init();
        //printf("Running %dth time for case 1\n", i); 
        s_tm = clock();
        fake_encoder.encode(Q);
        rtm += clock() - s_tm;
    }
    printf("Average Execution time of BinomialHeap is %f seconds\n", (float)rtm/(10*CLOCKS_PER_SEC));
    
    Q = &Q2;  
    rtm = 0;
    printf("Checking BinaryHeap building huffman-tree for 10 times\n");
    rep(i, 0, 10) {
        fake_encoder.init(argv[1], "code_table.txt", "encoded.bin");
        Q->init();
        //printf("Running %dth time for case 2\n", i); 
        s_tm = clock();
        fake_encoder.encode(Q);
        rtm += clock() - s_tm;
    }
    printf("Average Execution time of BinaryHeap is %f seconds\n", (float)rtm/(10*CLOCKS_PER_SEC));

    Q = &Q3;  
    rtm = 0;
    printf("Checking FourwayHeap building huffman-tree for 10 times\n");
    rep(i, 0, 10) {
        fake_encoder.init(argv[1], "code_table.txt", "encoded.bin");
        Q->init();
        //printf("Running %dth time for case 3\n", i); 
        s_tm = clock();
        fake_encoder.encode(Q);
        rtm += clock() - s_tm;
    }
    printf("Average Execution time of FourwayHeap is %f seconds\n", (float)rtm/(10*CLOCKS_PER_SEC));

    Q = &Q4;  
    rtm = 0;
    printf("Checking PairingHeap building huffman-tree for 10 times\n");
    rep(i, 0, 10) { 
        fake_encoder.init(argv[1], "code_table.txt", "encoded.bin");
        Q->init();
        //printf("Running %dth time for case 4\n", i); 
        s_tm = clock();
        fake_encoder.encode(Q);
        rtm += clock() - s_tm;
    }
    printf("Average Execution time of PairingHeap is %f seconds\n", (float)rtm/(10*CLOCKS_PER_SEC));
}
