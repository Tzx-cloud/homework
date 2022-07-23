#include<iostream>
#include<cstring>
#include<queue>
using namespace std;
//用sm2代替sha256
bool openssl_sm3_hash(const void* input,unsigned char* output,int len); 
// Merkle Tree 结构体定义 
typedef struct MerkleTreeNode{
	struct MerkleTreeNode *left;		// 左子节点 
	struct MerkleTreeNode *right;		// 右子节点 
	struct MerkleTreeNode *parent;		// 父节点 
	uint8_t data[32];					// 当前节点的值 
}MerkleTree;

MerkleTree * Creat_Merkle_Tree(MerkleTree *arr, int nums)
{
    MerkleTree *p=arr;
    MerkleTree *less=NULL;
	while(nums!=1||less!=NULL){
        if((nums&1)==0){
            MerkleTree *node=new MerkleTree[nums>>1];
            for(int i=0;i<nums;i+=2){
                int j=i>>1;
                node[j].left=(p+i);
                node[j].right=(p+i+1);
                (p+i)->parent=node+j;
                (p+i+1)->parent=node+j;
                uint8_t buff[65];
                buff[0]=0x01;
                memcpy(buff+1,node[j].left->data,32);
                memcpy(buff+33,node[j].right->data,32);
                openssl_sm3_hash(buff,node[j].data,65);
            }
            p=node;
            nums/=2;
        }else{
            if(less==NULL){
                MerkleTree *node=new MerkleTree[nums>>1];
                for(int i=0;i<nums-1;i+=2){
                    int j=i>>1;
                    node[j].left=p+i;
                    node[j].right=p+i+1;
                    (p+i)->parent=node+j;
                    (p+i+1)->parent=node+j;
                    uint8_t buff[65];
                    buff[0]=0x01;
                    memcpy(buff+1,node[j].left->data,32);
                    memcpy(buff+33,node[j].right->data,32);
                    openssl_sm3_hash(buff,node[j].data,65);
                }
                less=p+nums-1;
                p=node;
                nums/=2;
            }
           else{
                MerkleTree *node=new MerkleTree[(nums+1)>>1];
                for(int i=0;i<nums-1;i+=2){
                    int j=i>>1;
                    node[j].left=p+i;
                    node[j].right=p+i+1;
                    (p+i)->parent=node+j;
                    (p+i+1)->parent=node+j;
                    uint8_t buff[65];
                    buff[0]=0x01;
                    memcpy(buff+1,node[j].left->data,32);
                    memcpy(buff+33,node[j].right->data,32);
                    openssl_sm3_hash(buff,node[j].data,65);
                }
                node[nums/2].left=p+nums-1;
                node[nums/2].right=less;
                (p+nums-1)->parent=node+nums/2;
                less->parent=node+nums/2;
                uint8_t buff[65];
                buff[0]=0x01;
                memcpy(buff+1,node[nums/2].left->data,32);
                memcpy(buff+33,node[nums/2].right->data,32);
                openssl_sm3_hash(buff,node[nums/2].data,65);
                less=NULL;
                p=node;
                nums=nums/2+1;
           }
        }
    };
    p->parent=NULL;
    return p;
} 

void print_tree(MerkleTree *mt){
    if(mt==NULL)
        return;
    queue<MerkleTree*> q;
    q.push(mt);
    while(!q.empty()){
        int i=q.size();
        while (i--)
        {
            MerkleTree* p=q.front();
            q.pop();
            printf("%d ",i);
            if(p->left!=NULL)
                q.push(p->left);
            if(p->right!=NULL)
                q.push(p->right);
        }printf("\n");
    }
}

int inclusion_proof(MerkleTree *node){//存在性证明
    MerkleTree* p=node->parent;
    uint8_t buff[65];
    while(p->parent!=NULL){
        if(p->left==node){
            buff[0]=0x01;
            memcpy(buff+1,node->data,32);
            memcpy(buff+33,p->right->data,32);
        }
        else{
            buff[0]=0x01;
            memcpy(buff+1,p->left->data,32);
            memcpy(buff+33,node->data,32);           
        }
        openssl_sm3_hash(buff,buff,65);
        node=p;
        p=p->parent;
        memcpy(node->data,buff,32);
    }
    if(p->left==node){
            buff[0]=0x01;
            memcpy(buff+1,node->data,32);
            memcpy(buff+33,p->right->data,32);
        }
        else{
            buff[0]=0x01;
            memcpy(buff+1,p->left->data,32);
            memcpy(buff+33,node->data,32);           
        }openssl_sm3_hash(buff,buff,65);
    return memcmp(p->data,buff,32);
};

int exclusion_proof(float a,MerkleTree *node){//不存在性证明
    int b=a;
    int r1=inclusion_proof(node+b);//对b的存在性证明
    int r2=inclusion_proof(node+b+1);//对b相邻节点的存在性证明
    if(r1||r2){
        return 1;
    }else{
        return 0;
    }
};

int main(){
    int num=100000;//十万个节点
    MerkleTree *node=new MerkleTree[num];
    int i(0),j;
    for(j=0;j<num;j++){//生成叶子节点
        node[j].left=node[j].right=NULL;
        i+=1;//底层节点最小间距为1
        uint8_t buff[5];
        buff[0]=0x00;
        memcpy(buff+1,&i,4);
        openssl_sm3_hash(buff,node[j].data,5);
    }
    MerkleTree *root=NULL;
    root=Creat_Merkle_Tree(node,num);
    //print_tree(root);
    int ret=inclusion_proof(node+3);//对第四个叶子节点的存在性证明,存在则返回0
    ret=exclusion_proof(3.2,node);//对3.2的不存在性证明，不存在返回0
}

