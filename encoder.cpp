/*************************************************************************
    > In god we trust
    > File Name: main.cpp
************************************************************************/
#ifndef ROACH_HEADER
#define ROACH_HEADER
#include "ksheap.h"
#include <time.h>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <queue>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <climits>
#include <string>
#include <vector>
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
//ll powmod(ll a,ll b, ll MOD) {ll res=1;a%=MOD;for(;b;b>>=1){if(b&1)res=res*a%MOD;a=a*a%MOD;}return res;}
//void buginfo(const char* f, ...) {if(!debug)return;va_list al; va_start(al, f);vprintf(f, al);va_end(al);}
#endif
/*------------------ head-------------------*/
const int maxn = 1e6 + 10;
const int maxbitsz = 1024;

struct Encoder {
    int in_fd, tfd, compfd, hs[maxn], root, nsz;
    int w[maxn*2], v[maxn*2], lc[maxn*2], rc[maxn*2], fa[maxn*2];
    string st[maxn];

    //BinomialHeap Q;
    BinaryHeap Q;
    //FourwayHeap Q;
    //PairingHeap Q;

    void encode() {
        lseek(in_fd, 0, SEEK_SET);
        FILE* fp = fdopen(in_fd, "r");
        int d;
        while(fscanf(fp, "%d", &d)==1) ++hs[d];

        /*build Huffman-tree*/
        clock_t s_tm = 0;
        rep(i, 0, maxn) {
            if (!hs[i]) continue;
            int x = nsz++;
            v[x] = i; w[x] = hs[i];
            Q.push(x, w[x]);
        }

        while(Q.size() >= 2) {
            int x = Q.top(); Q.pop();
            int y = Q.top(); Q.pop();
            
            int z = nsz++;
            lc[z] = x, rc[z] = y;
            w[z] = w[x] + w[y];
            fa[x] = z, fa[y] = z;
            v[z] = -1;

            Q.push(z, w[z]);
        }
        printf("building huffman-tree time is %f\n", (float)(clock()-s_tm)/(CLOCKS_PER_SEC));

        root = Q.top(); Q.pop();

        /* print table to file*/
        print_table(root);

        /* compress the input file*/
        compress_file(fp);
    }

    void init(const char* input, const char* op_table, const char* op_encode) {
        in_fd = open(input, O_RDONLY, 0644);
        tfd = open(op_table, O_WRONLY|O_CREAT|O_TRUNC, 0644);
        compfd = open(op_encode, O_WRONLY|O_CREAT|O_TRUNC, 0644);

        if (in_fd==-1 || tfd==-1 || compfd==-1) {
            printf("Opening wrong input file for encoding...\n");
            return;
        }

        nsz = 1; root = -1;
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

    void print_binary(int fd, char* buf, int len, int& bsz, const string& s) {
        int btot = 8 * len;  
        rep(i, 0, s.size()) {
            if (bsz >= btot) { 
                ksflush(fd, buf, len);
                bsz = 0; memset(buf, 0, len);
            }

            uint8_t c = (s[i]=='1') ? 1:0;
            int x = bsz/8, y = bsz%8;
            buf[x] |= (c << (7-y)); 
            ++bsz;
        }
    }

    int ksflush(int fd, char* buf, int bytes) {
        if (bytes == 0) return 0; 
        
        int r = write(fd, buf, bytes);  
        if (r == -1) {
            printf("Write erro occured when ksflush\n");
            return -1;
        }
        
        return 0;
    }

    void compress_file(FILE* fp) {
        lseek(in_fd, 0, SEEK_SET);
        int len = sizeof(int) * maxbitsz;
        char* p = (char*)malloc(len+1); /*extra byte for flush*/
        int bsz = 0, d;

        while(fscanf(fp, "%d", &d)==1) 
            print_binary(compfd, p, len, bsz, st[d]);

        /* last 3-bits represents of how many bits were used
         * in the previous byte*/
        int x = bsz/8, y = bsz%8;
        p[x] = (uint8_t)y;
        ksflush(compfd, p, x+1);

        free(p);
        fsync(in_fd);
    }
} encoder;


int main(int argc, char** argv) {
    if (!argv[1]) 
        return 0 * printf("Please assign input file\n");
    
    encoder.init(argv[1], "code_table.txt", "encoded.bin");
    encoder.encode();

    return 0;
}
