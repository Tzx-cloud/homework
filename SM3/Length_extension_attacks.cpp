#include<iostream>
#include<cstring>

bool openssl_sm3_hash(const void* input,unsigned char* output,int len);
void sm3_compress(uint32_t*digest,const uint8_t* data);

// int main(){
//     unsigned char buff[512];
//     char M[]="abcd";//原始输入
//     char add[]="def";//长度拓展
//     unsigned char block[64];//原始填充后的块
//     int len=4;//原始长度
//     int alen=3;//填充长度
//     unsigned char Hash_M[32];//原始hash
//     uint8_t output1[32];
//     openssl_sm3_hash(M,Hash_M,len);
//     memcpy(block,M,len);
//     block[len]=0x80;
//     memset(block+len+1,0,64-len-9);
//     uint64_t *count=(uint64_t*)(block+64-8);
//     count[0]=(uint64_t)len<<3;
//     count[0]=_byteswap_uint64(count[0]);
//     memcpy(buff,block,64);
//     memcpy(buff+64,add,alen);
//     openssl_sm3_hash(buff,output1,alen+64);//填充后的hash值
//     memcpy(block,add,alen);
//     block[alen]=0x80;
//     memset(block+alen+1,0,64-alen-9);
//     count=(uint64_t*)(block+64-8);
//     count[0]=(uint64_t)(alen+64)<<3;
//     count[0]=_byteswap_uint64(count[0]);
//     uint32_t *pdgiset=(uint32_t*)(Hash_M);
//      for(int i=0;i<8;i++){
//          pdgiset[i]=_byteswap_ulong(pdgiset[i]);
//     }
//     sm3_compress(pdgiset,block);//拓展攻击解出的hash值
//     for(int i=0;i<8;i++){
//          pdgiset[i]=_byteswap_ulong(pdgiset[i]);
//     }
//     if(!memcmp(output1,Hash_M,32)){//两者相同则攻击成功
//         printf("success!\n");
//     }
// }