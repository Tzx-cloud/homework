#  成员及账号名
  
  
唐战希 Tzx-cloud
[库链接](https://github.com/Tzx-cloud/homework.git )
#  项目简介
  
  
通过C++语言，我实现了SM2,SM3,SM4以及比特币和以太坊的大部分实验
具体项目为：
  
- SM2
    - ECDSA在以太坊上的应用报告(pdf)
    - SM2两方签名(C++)
    - 用RFC6979修改SM2(C++)
    - 谷歌密钥检查(C++)
- SM3
    - 生日碰撞攻击(C++)
    - ρ碰撞(C++)
    - 长度拓展攻击(C++)
    - 加速SM3(C++)
    - merkle tree(C++)
- SM4
    - 加速SM4(C++)
- 比特币
    - 比特币测试(pdf)
    - 伪造签名(C++)
- 以太坊
    - 对MPT的研究(txt)
  
#  项目说明
  
##  运行指导
  
> C++编译器为linux-64下的g++编译器
> 对于同一个文件夹下的文件应使用多文件编译
> 运行某一个project时，应注意把同文件夹下的其他文件中的main函数注释掉
##  SM2
  
---
###  SM2两方签名
  
在这个cpp文件中我构建了两个结构体，一个为Bob，一个为Alice来模拟签名的两方,最后Bob会生成一个签名(r,s).
![截图](pic/SM2.1.png "结果截图")
###  用RFC6979修改SM2
  
首先我利用OpenSSL库中的椭圆曲线实现了基本的SM2算法，然后将生成随机数的方式用RFC6979方式代替，代码如下：
~~~C++
 //生成k RFC6979：k = SHA256(d + HASH(m));
 //这里为了方便，使用SM3代替SHA256
 openssl_sm3_hash(Z,(unsigned char*)M,strlen(M));
 memcpy(Buff+32,Z,32);
 BN_bn2binpad(d_a,Z,32);
 memcpy(Buff,Z,32);
 openssl_sm3_hash(Buff,Z,64);
 BN_bin2bn(Z,32,k);
~~~
最后成功生成了签名
![截图](pic/OMM%25QJCYULDO%5D1K_9NHD/(VV.png "结果截图")
###  谷歌密钥检查
  
通过模拟谷歌的用户名及密钥检查方式，来实现一个简单的密钥泄漏检查方法，同样我构建了一个client结构体和一个server结构体，最终检验是否泄漏由client完成。
![截图](pic/SYA%7DR~IG96LYB4%25EI6Q%5D%5D%5BW.png "结果截图")
##  SM3
  
---
##  SM4
  
---
##  比特币
  
---
##  以太坊
  
---
  