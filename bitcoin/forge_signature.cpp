#include <openssl/ec.h>
#include<iostream>
#include <cstring>

static char e[]="4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b";

struct sign{
    BIGNUM *e=BN_new();
    BIGNUM *r=BN_new();
    BIGNUM *s=BN_new();
    EC_POINT *P;
};

sign make_sign(){
        sign res;
        BN_CTX *ctx=BN_CTX_new();
        BIGNUM *n=BN_new();
        BIGNUM *k=BN_new();
        BIGNUM *x1=BN_new();
        BIGNUM *d=BN_new();
        EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(715);
        EC_GROUP_get_order(ec_group,n,ctx);
        do {
		BN_rand_range(d,n);
		BN_rand_range(k,n);
	    } while (BN_is_zero(k)||BN_is_zero(d));
        EC_POINT *R=EC_POINT_new(ec_group);
        res.P=EC_POINT_new(ec_group);
        EC_POINT_mul(ec_group,R,k,NULL,NULL,ctx);
        EC_POINT_mul(ec_group,res.P,d,NULL,NULL,ctx);
        EC_POINT_get_affine_coordinates_GFp(ec_group, R, x1, NULL, ctx);
        BN_copy(res.r,x1);
        BN_hex2bn(&res.e,e);
        BIGNUM*t=BN_new();
        BN_mod_inverse(k,k,n,ctx);
        BN_mod_mul(x1,d,x1,n,ctx);
        BN_mod_add(x1,res.e,x1,n,ctx);
        BN_mod_mul(res.s,k,x1,n,ctx);
        return res;
}

sign forge(sign res){
    sign forg;
    BN_CTX *ctx=BN_CTX_new();
    BIGNUM *n=BN_new();
    BIGNUM *w=BN_new();
    EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(715);
    EC_GROUP_get_order(ec_group,n,ctx);
    BN_mod_inverse(w,res.s,n,ctx);
    EC_POINT *P=EC_POINT_new(ec_group);
    EC_POINT_mul(ec_group,P,res.e,res.P,res.r,ctx);
    EC_POINT_mul(ec_group,P,NULL,P,w,ctx);
    EC_POINT_get_affine_coordinates_GFp(ec_group, P,forg.r, NULL, ctx);
    BIGNUM *u=BN_new();
    BIGNUM *v=BN_new();
    EC_POINT *p=EC_POINT_new(ec_group);
    do {
		BN_rand_range(u,n);
		BN_rand_range(v,n);
        EC_POINT_mul(ec_group,p,u,res.P,v,ctx);
	} while (EC_POINT_cmp(ec_group,p,P,ctx));
    BN_mod_inverse(v,v,n,ctx);
    BN_mod_mul(forg.s,forg.r,v,n,ctx);
    BN_mod_mul(forg.e,forg.s,u,n,ctx);
    EC_POINT_copy(forg.P,res.P);
    return forg;
}

int verify(sign res){
    BN_CTX *ctx=BN_CTX_new();
    BIGNUM *n=BN_new();
    BIGNUM *w=BN_new();
    BIGNUM *x1=BN_new();
    EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(715);
    EC_GROUP_get_order(ec_group,n,ctx);
    BN_mod_inverse(w,res.s,n,ctx);
    BN_mod_mul(x1,w,res.e,n,ctx);
    BN_mod_mul(w,w,res.r,n,ctx);
    EC_POINT *P=EC_POINT_new(ec_group);
    EC_POINT_mul(ec_group,P,x1,res.P,w,ctx);
    EC_POINT_get_affine_coordinates_GFp(ec_group, P, x1, NULL, ctx);
    return BN_cmp(res.r,x1);
}


int main(){
    sign res=make_sign();
    int ret=verify(res);//ret==0,correct sig
    sign forg=forge(res);
    ret=verify(forg);//ret==0,correct forge
}