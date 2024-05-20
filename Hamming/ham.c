#include <stdio.h>
#include <stdlib.h>

const int len_pack=16+5;
const int am_stop=5;

int is_pow(int a){
    int met=0;
    if(!a){
        return 1;
    }
    while(a){
        if(a&1){
            if(!met){
                met++;
            }
            else{
                return 0;
            }
        }
        a=a>>1;
    }
    return 1;
}

int init_Ham(int val){
        int curr_bit=0;
        int ham_val=0;
        for(int i=0; i<len_pack; i++){
        if(!is_pow(i+1)){
            ham_val|=((val>>curr_bit)&1)<<i;
            curr_bit++;
        }
    }
    return ham_val;
}

int encode_Ham(int val){
    int curr_bit=0;
    for(int i=0; i<am_stop; i++){
        for(int j=1; j<=len_pack; j++){
            if((j>>i)&1){
                curr_bit^=(val>>(j-1))&1;
            }
        }
        val|=curr_bit<<((1<<i)-1);
        curr_bit=0;
    }

    //general parity bit
    for(int i=0; i<len_pack; i++){
        curr_bit^=(val>>i)&1;
    }
    val=val<<1;
    val|=curr_bit;
    printf("val is %x\n",val);
    return val;
}

int check_Ham(int val){
    int res=0;
    int curr_bit=0;
  //  printf("val is %x\n",val);
    for(int i=0; i<am_stop; i++){
        for(int j=1; j<=len_pack; j++){
            if((j>>i)&1){
                
                curr_bit^=(val>>(j))&1;
                printf("bit %x, ind %x\n",(val>>(j))&1,j);
            }
        }
        res|=curr_bit<<(i);
        printf("%x\n",res);
        curr_bit=0;
   }
   //check general parity bit
   for(int i=0; i<=len_pack;i++){
    curr_bit^=(val>>i)&1;
   }
   // 2 mistakes, can only detect
   printf("parity %x\n",curr_bit);
   if(res && !curr_bit){
        return -1;
   }
         //  printf("%x\n",res);
    return res;
}

int decode_Ham(int val){
   int res=0;
   int copy_bit=0;
    for(int i=0; i<=len_pack; i++){
        if(!is_pow(i)){
            res|=((val>>i)&1)<<copy_bit;
            copy_bit++;
        }
    }
    return res;
}

int val;
int main(){
    scanf_s("%x",&val);
    printf("\n");
    int ham_val=init_Ham(val);
    ham_val=encode_Ham(ham_val);
   // ham_val&=0x3FFEEC;
    int res=check_Ham(ham_val);
    printf("syndrome =%x\n",res);
    printf("val =%x\n",ham_val);
    if(res==-1){
        printf("can not correct, 2 mistakes");
        return -1;
    }
    if(res){
        ham_val^=1<<res;
    }
    ham_val=decode_Ham(ham_val);
    printf("init val= %x\n", ham_val);
    printf("%x", ham_val);
}