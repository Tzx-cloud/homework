#include <iostream>
#include <openssl/ec.h>
#include <cstring>


bool openssl_sm3_hash(const void *input, unsigned char *output, int len);
struct res{
    unsigned char r[32];
    long rlen=0;
    unsigned char s[32];
    long slen=0;
}response;

res sign(const BIGNUM *d_a, BIGNUM *e);

static const char *group_p = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF";
static const char *group_a = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC";
static const char *group_b = "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93";
static const char *group_Gx = "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7";
static const char *group_Gy = "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0";
static const char *group_n = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123";
static const char *ENTL_ID = "008031323334353637383132333435363738";

// int main()
// {

//     char pa[65]; //公钥
//     unsigned char Z[32];
//     char M[] = "SM2-signature";

//     //生成密钥
//     BN_CTX *ctx = BN_CTX_new();
//     EC_KEY *ec_key = EC_KEY_new_by_curve_name(1172);
//     EC_KEY_generate_key(ec_key); //生成公私钥对
//     const BIGNUM *d_a = BN_new();
//     d_a = EC_KEY_get0_private_key(ec_key);
//     const EC_POINT *P = EC_KEY_get0_public_key(ec_key);
//     const EC_GROUP *ec_group = EC_KEY_get0_group(ec_key);
//     EC_POINT_point2oct(ec_group, P, POINT_CONVERSION_UNCOMPRESSED, (unsigned char *)pa, 65, ctx);
//     //预结算
//     static char Buff[256];
//     strcpy(Buff, ENTL_ID);
//     strcat(Buff, group_a);
//     strcat(Buff, group_b);
//     strcat(Buff, group_Gx);
//     strcat(Buff, group_Gy);
//     strcat(Buff, pa);
//     openssl_sm3_hash(Buff, Z, 230);

//     //计算e
//     memset(Buff, 0, 256);
//     memcpy(Buff, Z, 32);
//     memcpy(Buff + 32, M, 14);
//     openssl_sm3_hash(Buff, Z, 46);
//     BIGNUM *e = BN_new();
//     BN_bin2bn(Z, 32, e);

//     //椭圆曲线乘法
//     const EC_POINT *G = EC_GROUP_get0_generator(ec_group);
//     BIGNUM *n, *k;
//     n = BN_new();
//      EC_GROUP_get_order(ec_group, n, ctx);
//     k = BN_new();
//     //生成k RFC6979：k = SHA256(d + HASH(m));
//     //这里为了方便，使用SM3代替SHA256
//     openssl_sm3_hash(Z,(unsigned char*)M,strlen(M));
//     memcpy(Buff+32,Z,32);
//     BN_bn2binpad(d_a,Z,32);
//     memcpy(Buff,Z,32);
//     openssl_sm3_hash(Buff,Z,64);
//     BN_bin2bn(Z,32,k);

//     EC_POINT *k_G = EC_POINT_new(ec_group);
//     EC_POINT_mul(ec_group, k_G, NULL, G, k, ctx);
//     BIGNUM *x1 = BN_new();
//     EC_POINT_get_affine_coordinates_GFp(ec_group, k_G, x1, NULL, ctx);
//     BIGNUM *r, *a, *b, *s;
//     r = BN_new();
//     a = BN_new();
//     b = BN_new();
//     s = BN_new();
//     BN_one(a);

//     //计算r
//     BN_mod_add(r, x1, e, n, ctx);
//     BN_add(a, d_a, a);
//     BN_mod_inverse(a, a, n, ctx);
//     BN_mul(b, r, d_a, ctx);
//     BN_mod_sub(b, k, b, n, ctx);

//     //计算s
//     BN_mod_mul(s, a, b, n, ctx);
//     response.rlen = BN_bn2bin(r, response.r);
//     response.slen = BN_bn2bin(s, response.s);
//     printf("r: ");
//     for(int i=0;i<32;i++){
//          printf("%02X ",response.r[i]);
//     }
//     printf("\ns: ");
//     for(int i=0;i<32;i++){
//          printf("%02X ",response.s[i]);
//     }
// }
