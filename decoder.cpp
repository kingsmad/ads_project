/*************************************************************************
    > In god we trust
    > File Name: main.cpp
************************************************************************/
#ifndef ROACH_HEADER
#define ROACH_HEADER
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
//const int maxbitsz = 1024;

struct Decoder {
    int v[maxn*2], lc[maxn*2], rc[maxn*2];
    int nsz, ffd, tfd, ofd;
    int getlc(int x) {
        if (lc[x] != -1) return lc[x]; 
        return lc[x] = nsz++;
    }
    int getrc(int x) {
        if (rc[x] != -1) return rc[x];
        return rc[x] = nsz++;
    }

    inline bool isleaf(int x) { return v[getlc(x)]==-1 && v[getrc(x)]==-1; }

    int init(const char* binfile, const char* code_table, \
            const char* decode_output) {
        if (ffd) close(ffd);
        if (tfd) close(tfd);
        if (ofd) close(ofd);
        ffd = open(binfile, O_RDONLY, 0644);
        tfd = open(code_table, O_RDONLY, 0644);
        ofd = open(decode_output, O_WRONLY|O_CREAT|O_TRUNC, 0644);
        if (ffd==-1 || tfd==-1 || ofd==-1) {
            printf("\ndecoder got wrong input-file or table-file.\n");
            return -1;
        }
        memset(lc, -1, sizeof(lc));
        memset(rc, -1, sizeof(rc));
        memset(v, -1, sizeof(v));
        nsz = 1;
        return 0;
    }

    int find_node(const char* s) {
        int len = strlen(s);
        int x = 0;
        rep(i, 0, len)
            x = (s[i]=='0') ? getlc(x):getrc(x);
        return x;
    }

    void rebuild_table() {
        lseek(tfd, 0, SEEK_SET);
        FILE* fp = fdopen(tfd, "r"); 
        memset(v, -1, sizeof(v));
        int d; char s[100];
        while(1) {
            int y = fscanf(fp, "%d ===> %s\n", &d, s);
            if (y != 2) break; 
            int x = find_node(s);
            v[x] = d;
        }
    }

    int file_size(int fd) {
        struct stat s;
        if (fstat(fd, &s) == -1) {
            printf("\nfstat returned erro;");
            return -1;
        }
        return s.st_size;
    }

    void decode() {
        rebuild_table();

        uint8_t buf[1028];
        vector<int> ans;

        int len = 0, o = 0;
        int fsize=file_size(ffd), clen = 0;
        while((len=read(ffd, buf, 1024)) > 0) {
            clen += len;
            if (clen+1==fsize) {
                len += read(ffd, buf+len, 1);
                ++clen;
            }
            int tot = len * 8;
            if (clen == fsize)
                tot -= buf[len-1] + 8;

            rep(i, 0, tot) {
                int x = i/8, y = i%8;
                int tmp = (1<<(7-y)) & buf[x];
                
                o = (tmp>0) ? getrc(o) : getlc(o);
                if (v[o]!=-1) {
                    ans.pb(v[o]);
                    o = 0;
                }
            }
        }
        
        FILE* fp = fdopen(ofd, "w");       
        for(int d: ans) 
            fprintf(fp, "%d\n", d);
        return;
    }
} decoder;

int main(int argc, char** argv) {
    if (!argv[1] || !argv[2]) 
        return 0 * printf("Please assign encoded_file_name && \
                code_table_file_name\n");
    //const char* s = "sample_input_small.txt";
    //const char* s = "sample_input_large.txt";
    
    //decoder.init("compressed.bin", "table_encode.txt", "recovered.txt");
    decoder.init(argv[1], argv[2], "decoded.txt");
    decoder.decode();

    return 0;
}
