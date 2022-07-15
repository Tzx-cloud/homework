#include<iostream>
#include<cstring>

using namespace std;
bool openssl_sm3_hash(const void* input,unsigned char* output,int len);
// int main(){
//     uint8_t input1[32]={1},input2[32]={1};
//     uint8_t output1[32],output2[32];
//     while(1){
//         openssl_sm3_hash(input1,output1,32);
//         openssl_sm3_hash(input2,output2,32);
//         memcpy(input2,output2,32);
//         openssl_sm3_hash(input2,output2,32);
//         if(!memcmp(output1,output2,4)){
//             for(int i=0;i<32;i++){
//                 printf("%02x,",input1[i]);
//             }
//             printf("\n");
//             for(int i=0;i<32;i++){
//                 printf("%02x,",input2[i]);
//             }
//             return 0;
//         }
//         memcpy(input1,output1,32);
//         memcpy(input2,output2,32);
//     }
// }