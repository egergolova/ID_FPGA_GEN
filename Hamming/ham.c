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
    // int check_bits=0;
    for(int i=0; i<am_stop; i++){
        for(int j=1; j<=len_pack; j++){
            if((j>>i)&1){
                curr_bit^=(val>>(j-1))&1;
            }
        }
        val|=curr_bit<<((1<<i)-1);
        // check_bits|=curr_bit<<(i);
        curr_bit=0;
    }

    //general parity bit
    for(int i=0; i<len_pack; i++){
        curr_bit^=(val>>i)&1;
    }
    // printf("val is %x\n",check_bits);
    val=val<<1;
    val|=curr_bit;
    // check_bits=check_bits<<1;
    // check_bits|=curr_bit;
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
                // printf("bit %x, ind %x\n",(val>>(j))&1,j);
            }
        }
        res|=curr_bit<<(i);
        // printf("%x\n",res);
        curr_bit=0;
   }
   //check general parity bit
   for(int i=0; i<=len_pack;i++){
    curr_bit^=(val>>i)&1;
   }
   // 2 mistakes, can only detect
//    printf("parity %x\n",curr_bit);
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
int IDs[]={
0b0101000110010000,
0b0001000100010000,
0b0001100100010000,
0b0001111100010111,
0b0001000000010101,
0b0000100100010010,
0b0001101100010101,
0b0001100100010111,
0b0101100100011101,
0b0000011010010101
};
int Ham_codes[10]={};
int Mist_synd[10]={};
int Ham_dec[10]={};
int main(){
    // initialize and ENCODE the Hamming
    for(int i=0; i<10; i++){
         int ham=IDs[i];
         ham=init_Ham(ham);
         Ham_codes[i]=encode_Ham(ham);
    }
    // checking, decoding and correcting mistakes if possible.
    for(int i=0; i<10; i++){
        Mist_synd[i]=check_Ham(Ham_codes[i]);
        Ham_dec[i]=decode_Ham(Ham_codes[i]);
        // printf("%x\n",Mist_synd[i]);
        // if Syndrome is not equal to zero
        if(Mist_synd[i]){
            // ret val in case if is not possible to correct. Indicates 2 mistakes
            if(Mist_synd[i]==-1){
                //incorrectable mistake. insert code with restarting the IP
            }
            // correcting one mistake by flipping the incorrect bit.
            else{
                Ham_dec[i]^=1<<Mist_synd[i];
            }
        }
    }



}