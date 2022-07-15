#include <openssl/evp.h>
#include <openssl/aes.h>
#include<string.h>

bool openssl_sm3_hash(const void* input,unsigned char* output,int len)
{
   bool res;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    // 设置使用SM3
    EVP_DigestInit_ex(ctx, EVP_sm3(), NULL);
    EVP_DigestUpdate(ctx,input,len);
    res=EVP_DigestFinal_ex(ctx,output,NULL);
    EVP_MD_CTX_free(ctx);
    return res;
}

bool openssl_sm3_hash_verity(const void* msg,int len,const unsigned char* message){
    uint8_t buf[32];
    openssl_sm3_hash(msg,buf,len);
    return memcmp(msg,message,32);
}