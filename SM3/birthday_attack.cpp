#include<unordered_map>
#include<iostream>
#include<cstring>
using namespace std;
bool openssl_sm3_hash(const void* input,unsigned char* output,int len);
unordered_map<string,long long> hash_map;//利用哈希表加快查表
//  int main(){
//       long long input[1]={0};
//       unsigned char output[32];
//       while(input[0]<LONG_LONG_MAX){
//           openssl_sm3_hash(input,output,8);
//           string s="";
//           for(int i=0;i<6;i++){//取前8byte
//                s+=output[i];
//            }
//           auto it=hash_map.find(s);
//           if(it!=hash_map.end()){
//               printf("%lld,%lld,%s",input[0],it->second,s.c_str());
//               return 0;
//           }
//           hash_map[s]=input[0]++;
//       }
//   }