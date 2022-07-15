#include<iostream>
#include<cstring>
#include<time.h>
#include <intrin.h>
using namespace std;

#define sm3_digest 32
#define sm3_block 64

#define rs32(i,n) (((i<<(32-n))&0xffffffff)|((i>>n)&0xffffffff))
#define ls32(i,n) (((i<<n)&0xffffffff)|((i>>(32-n))&0xffffffff))

#define P0(x) (x^ls32(x,9)^ls32(x,17))
#define P1(x) (x^ls32(x,15)^ls32(x,23))
#define T(i) (i<16?0x79cc4519:0x7a879d8a)
#define FF(x,y,z,i) (i<16?x^y^z:(x&y)|(x&z)|(y&z))
#define GG(x,y,z,i) (i<16?x^y^z:(x&y)|((~x)&z))

#define byte_swap32(i) ((rs32(i,8)&0xff00ff00)|(ls32(i,8)&0x00ff00ff))
#define byte_swap64(i) (((uint64_t)byte_swap32((uint32_t)(i&0xffffffff))<<32)|(byte_swap32((uint32_t)(i>>32))))


typedef struct sm3_ctx_t
{
    uint32_t digest[sm3_digest/sizeof(uint32_t)];
    int nblocks;
    uint8_t block[sm3_block*4];
    int num;
}sm3_ctx;

void sm3_init(sm3_ctx *ctx);
void sm3_update(sm3_ctx *ctx,const uint8_t* data,size_t data_len);
void sm3_final(sm3_ctx *ctx,uint8_t *digest);
void sm3_hash(uint8_t* digest,const uint8_t* data,size_t dlen);
void sm3_compress(uint32_t*digest,const uint8_t* data);
bool openssl_sm3_hash(const void* input,unsigned char* output,int len);

#define num_max 10000000
int main(){
     static char a[num_max]={0};
     a[0]='a';
     uint8_t b[32]={0},c[32]={0};
     long long begin;
     begin=clock();
     openssl_sm3_hash(a,b,num_max);
     printf("openssl time:%.3f s\n",(double)(clock()-begin)/CLOCKS_PER_SEC);
     begin=clock();
     sm3_hash(c,(uint8_t*)a,num_max);
     printf("my_sm3 time:%.3f s\n",(double)(clock()-begin)/CLOCKS_PER_SEC);
}



void sm3_init(sm3_ctx *ctx){
    __m256i digest;
    digest=_mm256_set_epi32(0x7380166F,0x4914B2B9,0x172442D7,0xDA8A0600,0xA96F30BC,0x163138AA,0xE38DEE4D,0xB0FB0E4E);
    _mm256_store_epi32(ctx->digest,digest);
    ctx->nblocks=0;
    ctx->num=0;
}

void sm3_update(sm3_ctx *ctx,const uint8_t *data,size_t dlen){
    if(ctx->num){
        unsigned int left=sm3_block-ctx->num;
        if(dlen<left){
            memcpy(ctx->block+ctx->num,data,dlen);
            ctx->num+=dlen;
            return;
        }else{
            memcpy(ctx->block+ctx->num,data,left);
            sm3_compress(ctx->digest,ctx->block);
            ctx->nblocks++;
            data+=left;
            dlen-=left;
        }
    }
    while(dlen>=sm3_block){
         sm3_compress(ctx->digest,data);
         ctx->nblocks++;
         data+=sm3_block;
         dlen-=sm3_block;
    }
    ctx->num=dlen;
    if(dlen){
        memcpy(ctx->block,data,dlen);
    }
}

void sm3_final(sm3_ctx *ctx,uint8_t *digest){
    size_t i;
    uint32_t *pdgiset=(uint32_t*)(digest);
    uint64_t *count=(uint64_t*)(ctx->block+sm3_block-8);
    ctx->block[ctx->num]=0x80;
    if(ctx->num+9<=sm3_block){
        memset(ctx->block+ctx->num+1,0,sm3_block-ctx->num-9);
    }else{
        memset(ctx->block+ctx->num+1,0,sm3_block-ctx->num-1);
        sm3_compress(ctx->digest,ctx->block);
        memset(ctx->block,0,sm3_block-8);
    }
    count[0]=(uint64_t)(ctx->nblocks<<9)+(ctx->num<<3);
    count[0]=byte_swap64(count[0]);
    sm3_compress(ctx->digest,ctx->block);
     for(i=0;i<sizeof(ctx->digest)/sizeof(ctx->digest[0]);i++){
        pdgiset[i]=byte_swap32(ctx->digest[i]);
     }
}

void sm3_hash(uint8_t* digest,const uint8_t* data,size_t dlen){
    sm3_ctx ctx;
    sm3_init(&ctx);
    sm3_update(&ctx,data,dlen);
    sm3_final(&ctx,digest);
}

void sm3_compress(uint32_t*digest,const uint8_t* data){
    uint32_t W68[68];
    uint32_t W64[64];
    int j=0;
    
    __m256i w=_mm256_loadu_epi32(data);
    _mm256_storeu_epi32(W68,w);
    w=_mm256_loadu_epi32(data+32);
    _mm256_storeu_epi32(W68+8,w);
    // for (j = 0; j < 16; j++)
	// {
    //   memcpy(W68+j,data+4*j,4);
    //   W68[j]=byte_swap32(W68[j]);
	// }
   
	for (j = 16; j < 68; j++)
	{
		W68[j] = P1((W68[j - 16] ^ W68[j - 9] ^ (ls32(W68[j - 3], 15)))) ^((ls32(W68[j - 13], 7)) ^ W68[j - 6]);
	}
 
	for (j = 0; j < 64; j+=16)
	{
        __m256i a=_mm256_load_epi32(W68+j);
        __m256i b=_mm256_load_epi32(W68+j+4);
        __m256i e=_mm256_load_epi32(W68+j+8);
        __m256i f=_mm256_load_epi32(W68+j+12);
        __m256i c=_mm256_xor_epi32(a,b);
        __m256i g=_mm256_xor_epi32(e,f);
        _mm256_store_epi32(W64+j,c);
        _mm256_store_epi32(W64+j+8,g);

		// W64[j] = W68[j] ^ W68[j + 4];
		// W64[j+1] = W68[j+1] ^ W68[j + 5];
		// W64[j+2] = W68[j+2] ^ W68[j + 6];
		// W64[j+3] = W68[j+3] ^ W68[j + 7];
        // W64[j+4] = W68[j+4] ^ W68[j + 8];
		// W64[j+5] = W68[j+5] ^ W68[j + 9];
		// W64[j+6] = W68[j+6] ^ W68[j + 10];
		// W64[j+7] = W68[j+7] ^ W68[j + 11];
	}
	
	//iterative process
    unsigned int A,B,C,D,E,F,G,H;
    A=digest[0];
	B=digest[1];
    C=digest[2];
    D=digest[3];
    E=digest[4];
    F=digest[5];
    G=digest[6];
    H=digest[7]; 
	//tempporary variable
	unsigned int SS1 = 0, SS2 = 0, TT1 = 0, TT2 = 0;
 
	for (j = 0; j < 64; j++)
	{
		SS1 = ls32((ls32(A, 12) + E + ls32(T(j), (j % 32))), 7);
		SS2 = SS1 ^ (ls32(A, 12));
		TT1 = (FF(A, B, C, j) + D) + (SS2 + W64[j]);
		TT2 = (GG(E, F, G, j) + H) + (SS1 + W68[j]);
		D =C;
		C = ls32(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = ls32(F, 19);
		F = E;
		E = P0(TT2);
	}
 	digest[0] = A ^ digest[0];
 	digest[1] = B ^ digest[1];
 	digest[2] = C ^ digest[2];
 	digest[3] = D ^ digest[3];
 	digest[4] = E ^ digest[4];
 	digest[5] = F ^ digest[5];
 	digest[6] = G ^ digest[6];
 	digest[7] = H ^ digest[7];
}
