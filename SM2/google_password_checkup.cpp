#include <iostream>
#include <cstring>
#include<map>
#include<set>
#include <openssl/ec.h>
using namespace std;
bool openssl_sm3_hash(const void* input,unsigned char* output,int len);

struct N{//大素数模
    BIGNUM *n=BN_new();
    N(){
        BN_generate_prime_ex(n,1024,1,NULL,NULL,NULL);
    }
}n;

struct c2s{//client send to server
    //k=h[:1]考虑到算力，只取h的1byte
    BN_CTX *ctx=BN_CTX_new();
    unsigned char k;
    //v=h^a
    BIGNUM *v=BN_new();
};

struct s2c{//server send to client
    BN_CTX *ctx=BN_CTX_new();
    BIGNUM *h_ab=BN_new();
    set<string>s;
};

struct client
{
    private:
    char name[10];
    char password[20];
    unsigned char h[32];
    long long sk;
    public:
    c2s res;
    client(char *a,char* b){
        strcpy(name,a);
        strcpy(password,b);
        sk=rand()%1000;
    }

    void make_res(){
        //用sm3替代argon2
        memcpy(h,name,strlen(name));
        memcpy(h+strlen(name),password,strlen(password));
        openssl_sm3_hash(h,h,(strlen(name)+strlen(password)));
        res.k=h[0];
        BIGNUM *t=BN_new(),*e=BN_new();
        BN_set_word(t,sk);
        BN_bin2bn(h,32,e);
        BN_mod_exp(res.v,e,t,n.n,res.ctx);
    }

    int verity(s2c res){
        BIGNUM *t=BN_new(),*e=BN_new();
        BN_set_word(t,sk);
        BN_copy(e,n.n);
        BN_sub_word(e,1);
        BN_mod_inverse(t,t,e,res.ctx);
        BN_mod_exp(e,res.h_ab,t,n.n,res.ctx);
        unsigned char v[256];
        int len=BN_bn2bin(e,v);
        string str;
        str.append(reinterpret_cast<const char*>(v),len);
        return res.s.count(str);
    }
};

struct server{
    private:
    map<string,string>data;//数据库
    set<string>s[256];//数据划分
    long long sk;
    public:
    s2c res;
    server(){
        //产生私钥
        sk=rand()%1000;
        //生成数据库
        data["alice"]="123456";
        data["bob"]="123456";
        data["mike"]="12345678";
        data["Alice"]="123456";
        data["Bob"]="123456";
        data["Mike"]="12345678";
        data["salice"]="0123456";
        data["sbob"]="0123456";
        data["smike"]="012345678";
        data["sAlice"]="0123456";
        data["sBob"]="0123456";
        data["sMike"]="012345678";
    }
    void make_set(){
        for(auto t:data){
            unsigned char h[32];
            const char *f,*c;
            f=t.first.c_str();
            c=t.second.c_str();
            memcpy(h,f,strlen(f));
            memcpy(h+strlen(f),c,strlen(c));
            openssl_sm3_hash(h,h,(strlen(f)+strlen(c)));
            int k=(int)h[0];
            BN_CTX *ctx=BN_CTX_new();
            BIGNUM *a=BN_new(),*e=BN_new();
            BN_set_word(a,sk);
            BN_bin2bn(h,32,e);
            BN_mod_exp(a,e,a,n.n,ctx);
            unsigned char v[256];
            int len=BN_bn2bin(a,v);
            string str;
            str.append(reinterpret_cast<const char*>(v),len);
            s[k].insert(str);
        }
    }
    void make_res(c2s res){
        BIGNUM *a=BN_new();
        BN_set_word(a,sk);
        BN_mod_exp(this->res.h_ab,res.v,a,n.n,this->res.ctx);
        this->res.s=this->s[int(res.k)];
    }
}google;

int main(){
    client a("alice0","123456");
    //客户端发送请求
    a.make_res();
    //初始化库
    google.make_set();
    //服务器发送响应
    google.make_res(a.res);
    //验证用户名和密钥是否泄漏
    int ret=a.verity(google.res);
    if(ret){
        printf("your password has been leaked!\n");
    }
}