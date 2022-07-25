#include <openssl/ec.h>
#include<iostream>
#include <cstring>
bool openssl_sm3_hash(const void* input,unsigned char* output,int len);
unsigned char Z[]={0xBC,0x37,0x36,0xA2,0xF4,0xF6,0x77,
          0x9C,0x59,0xBD,0xCE,0xE3,0x6B,0x69,
          0x21,0x53,0xD0,0xA9,0x87,0x7C,0xC6,
          0x2A,0x47,0x40,0x02,0xDF,0x32,0xE5,
          0x21,0x39,0xF0,0xA0};


struct B2A//Bob send to alice
{
    EC_POINT *P1;
    EC_POINT *Q1;
    unsigned char e[32];
};

struct A2B//alice send to bob
{
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM* r=BN_new();
    BIGNUM* s2=BN_new();
    BIGNUM* s3=BN_new();
};

struct signaure{//bob sign
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM* r=BN_new();
    BIGNUM* s=BN_new();
};

struct BOb{
    private:
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM* d1=BN_new();
    BIGNUM* k1=BN_new();
    BIGNUM* n=BN_new();
    public:
    B2A res;
    signaure r_s;
    char *M="hello,Alice";

    void make_p1(EC_POINT*point){
        EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(1172);
        EC_GROUP_get_order(ec_group,this->n,this->ctx);
        do {
		BN_rand_range(this->d1,this->n);
	    } while (BN_is_zero(this->d1));
        BIGNUM*t=BN_new();
        BN_mod_inverse(t,this->d1,this->n,this->ctx);
        EC_POINT_mul(ec_group,point,t,NULL,NULL,this->ctx);
    }

    void make_qe(EC_POINT*point,unsigned char*e){
        EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(1172);
        char buff[256];int len=0;
        len+=sizeof(M)/sizeof(M[0]);
        memcpy(buff,M,len);
        memcpy(buff+len,this->M,strlen(this->M));
        len+=strlen(this->M);
        openssl_sm3_hash(buff,e,len);
        do{
		BN_rand_range(this->d1,this->n);
	    } while (BN_is_zero(this->d1));
        EC_POINT_mul(ec_group,point,this->k1,NULL,NULL,this->ctx);
    }
    
    void make_res(){
        EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(1172);
        this->res.P1=EC_POINT_new(ec_group);
        this->res.Q1=EC_POINT_new(ec_group);
        make_p1(this->res.P1);
        make_qe(this->res.Q1,this->res.e);
    }

    void make_sign(A2B res){
        BN_mod_mul(this->r_s.s,this->d1,this->k1,this->n,this->ctx);
        BN_mod_mul(this->r_s.s,this->r_s.s,res.s2,this->n,this->ctx);
        BN_mod_mul(this->r_s.r,this->d1,res.s3,this->n,this->ctx);
        BN_mod_sub(this->r_s.r,this->r_s.r,res.r,this->n,this->ctx);
        BN_mod_add(this->r_s.s,this->r_s.s,this->r_s.r,this->n,this->ctx);
        BN_copy(this->r_s.r,res.r);
    }

}bOb;

struct Alice
{
    private:
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM* d2=BN_new();
    BIGNUM* k2=BN_new();
    BIGNUM* k3=BN_new();
    BIGNUM* n=BN_new();
    public:
    A2B res;
    void make_res(B2A res){
        EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(1172);
        EC_GROUP_get_order(ec_group,this->n,this->ctx);
        do {
		    BN_rand_range(this->d2,this->n);
	    } while (BN_is_zero(this->d2));
         do {
		BN_rand_range(this->k2,this->n);
	    } while (BN_is_zero(this->k2));
         do {
		BN_rand_range(this->k3,this->n);
	    } while (BN_is_zero(this->k3));
        EC_POINT* q2=EC_POINT_new(ec_group);
        EC_POINT* q3=EC_POINT_new(ec_group);
        EC_POINT_mul(ec_group,q2,this->k2,NULL,NULL,this->ctx);
        EC_POINT_mul(ec_group,q2,nullptr,res.Q1,this->k3,this->ctx);
        EC_POINT_add(ec_group,q3,q3,q2,this->ctx);
        BIGNUM* x=BN_new();
        BIGNUM* e=BN_new();
        BN_bin2bn(res.e,32,e);
        EC_POINT_get_affine_coordinates_GFp(ec_group, q2, x, NULL, this->ctx);
        BN_mod_add(this->res.r,x,e,this->n,this->ctx);
        BN_mod_mul(this->res.s2,this->d2,this->k3,this->n,this->ctx);
        BN_mod_add(x,this->res.r,this->k2,this->n,this->ctx);
        BN_mod_mul(this->res.s3,x,this->d2,this->n,this->ctx);
    }
}alice;

// int main(){
//     //bob生成P1，Q1，e
//     bOb.make_res();
//     //alice生成r，s2，s3
//     alice.make_res(bOb.res);
//     //bob生成签名r，s
//     bOb.make_sign(alice.res);
//     unsigned char r[32],s[32];
//     BN_bn2bin(bOb.r_s.r,r);
//     BN_bn2bin(bOb.r_s.s,s); 
//      printf("r: ");
//      for(int i=0;i<32;i++){
//         printf("%02X ",r[i]);
//      }
//      printf("\ns: ");
//      for(int i=0;i<32;i++){
//         printf("%02X ",s[i]);
//      }
// }

