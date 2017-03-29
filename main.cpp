/*************************************************************************
    > In god we trust
    > File Name: main.cpp
************************************************************************/
const bool debug = false;
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
ll powmod(ll a,ll b, ll MOD) {ll res=1;a%=MOD;for(;b;b>>=1){if(b&1)res=res*a%MOD;a=a*a%MOD;}return res;}
void buginfo(const char* f, ...) {if(!debug)return;va_list al; va_start(al, f);vprintf(f, al);va_end(al);}
/*------------------ head-------------------*/
const int maxn = 1e6 + 10;
const int maxbitsz = 1024;

int w[maxn*2];
struct Encoder {
    int in_fd, tfd, compfd, hs[maxn], root, nsz;
    int v[maxn*2], lc[maxn*2], rc[maxn*2], fa[maxn*2];
    string st[maxn];
    struct cmp {
        bool operator()(const int x, const int y) {
            return w[x] > w[y];
        }
    };
    priority_queue<int, vector<int>, cmp> Q;

    void encode() {
        FILE* fp = fdopen(in_fd, "r");
        int d;
        while(fscanf(fp, "%d", &d)==1) ++hs[d];

        /*build Huffman-tree*/
        rep(i, 0, maxn) {
            if (!hs[i]) continue;
            int x = nsz++;
            v[x] = i; w[x] = hs[i];
            Q.push(x);
        }

        while(Q.size() >= 2) {
            int x = Q.top(); Q.pop();
            int y = Q.top(); Q.pop();
            
            int z = nsz++;
            lc[z] = x, rc[z] = y;
            w[z] = w[x] + w[y];
            fa[x] = z, fa[y] = z;
            v[z] = -1;

            Q.push(z);
        }

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
            buginfo("\ndecoder got wrong input-file or table-file.\n");
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
    /*if (!argv[1]) 
        return 0 * printf("Please assign input file\n");*/
    //const char* s = "sample_input_small.txt";
    const char* s = "sample_input_large.txt";

    encoder.init(s, "table_encode.txt", "compressed.bin");
    encoder.encode();

    decoder.init("compressed.bin", "table_encode.txt", "recovered.txt");
    decoder.decode();

    return 0;
}
