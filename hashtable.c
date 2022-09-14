#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hashtable.h"


/**
 * @brief hash function
 * 
 * sum of all ASCII values of characters in a string ajusted to the size of h
 * integer ajusted to the size of h
 * 
 * @param h hashtable for which the hash value is necessary 
 * @param key_void key to be hashed
 * @return int
 */
int hash(HT* h, int key) {
    return key%(h->size);
}

/**
 * @brief checks if given position in hashtable is free to write on
 * 
 * returns false if h->key_type is not defined
 * 
 * @param h hashtable on whitch to check
 * @param p position to check
 * @return int 
 */
int isfreeHT(HT* h, int p) {
    return h->tbl[p].key == EMPTY || h->tbl[p].key == DELETED;
    return 0;
}


/**
 * @brief frees allocated memory pointed to by h->tbl[p].value
 * 
 * @param h hashtable on whitch memory will be freed
 * @param p position on whitch memory should be freed
 * @return int 
int freeValueHT(HT* h, int p, int flg) {
    //value type check
    if(h->value_type == PEDIDO) {
        if(flg){
            deepFreePedido(h->tbl[p].value);//deep free of pedido
            free(h->tbl[p].value);
        }
    }else if (h->value_type == INT) {
        free(h->tbl[p].value);// simple free for integer
    }else {
        return -1;//error condition if typr not defined
    }
    return 0;
}*/

/**
 * @brief compares two given keys of the same type
 * 
 * returns 0 if keys are equal
 * returns <0 if key1 < key2
 * returns >0 if key1 > key2
 * 
 * @param h hashtable on whitch keys will be compared
 * @param key1 
 * @param key2 
 * @return int 
 */
int keycmp(HT* h, int key1, int key2) {
    //type check
    return key1 - key2;
}

/**
 * @brief inicializa um dicionario
 * 
 * aux_array is alocated as simulated matrix
 * 
 * @param h pointer to hashtable to be created
 * @param size size of new hashtable
 * @param aux_array_flag flag indicating if an auxiliary array is to be created
 * @param key_type type of key
 * @param value_type type of value
 * 
 * @return int
 */
int initHT(HT *h, int size) {

    

    //aux_array_flag check
    if((h->aux_array.array = malloc(2*size*sizeof(int))) == NULL) { //aux_array_flag initialization
        return -1;//error condition if malloc fails
    }
    h->aux_array.last = -1; //initial last position is set initialy as -1

    if((h->tbl = calloc(size, sizeof(struct pair))) == NULL){//memory allocation for hashtable
        return -1;//error condition if calloc fails
    }

    //setting initial parameters
    h->size = size;
    h->entries = 0;
    h->used = 0;

    //if key is set to INT sizeof(pid_t) bytes of memory are allocated
    //for eatch position in hashtable
    for (int i=0; i<size; i++) {
        h->tbl[i].key = EMPTY;
        h->tbl[i].value = NULL;
    }

    return 0;
}

/**
 * @brief auxiliary function to use in writeHT
 * 
 * @param h 
 */
void AuxFree(HT *h) {
    //lucena: testar se o array existe mesmo, vai que
    if(h->aux_array.array) {
        free(h->aux_array.array);
    }
    free(h->tbl);
}

/**
 * @brief frees all memory allocated for a given hashtable 
 * 
 * @param h hashtable to free
 */
void freeHT(HT *h) {
    free(h->aux_array.array);
    if (h->tbl) {
        free(h->tbl);//free hashtable
    }
    free(h);//free pointer to h
}

/**
 * @brief checks if number is prime 
 * 
 * 
 * @param p value to test
 * @return int
 */
int isprime(int p){

    int i;
    
    for(i = 2 ; i <= p/2 ; i++){
        if(p % i == 0) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief funcao auxiliar do writeHT 
 * 
 * 
 * @param h hashtable on whitch to write
 * @param key key to add to hashtable
 * @param value value to associate with key
 * @return int
 */
int writeHTaux (HT *h, int key, GraphListHead *value) {

    int p = hash(h, key);
    int flag = 1;

    //increments p until free position is found
    for(; !isfreeHT(h, p) && (flag = keycmp(h, key,(h->tbl)[p].key)); p = (p+1)%(h->size));

    //copy value from value to hashtable
    h->tbl[p].value = value;
    
    //if key is new to hashtable write it on to hashtable
    if (flag) {
        
        //update aux_array
        int last = h->aux_array.last;
        if (last != -1) {
            h->aux_array.array[POS(last,1)] = p;
        }
        h->aux_array.array[POS(p,0)] = last;
        //h->aux_array.array[POS(p,1)] = -1;
        h->aux_array.last = p;
        
        if (h->tbl[p].key != DELETED) 
            h->used++;//increment used
        //copy key from key to hashtable
        h->tbl[p].key = key;
        
        h->entries++;//increment entries

    }

    return p;
}

/**
 * @brief writes pair key,value in hashtable 
 * 
 * if charge passes MAX_CHARGE changes size to largest prime bigger than 2*h->size
 * if key already in hashtable value is updated
 * 
 * @param h pointer to hashtable on whitch to write
 * @param key key to put in hashtable
 * @param value value to associate to key
 * @return int position were (key,value) was placed or -1
 */
int writeHT (HT *h, int key, GraphListHead *value) {

    //charge check
    float charge = (h->used + 1.0)/(h->size);

    if(charge >= MAX_CHARGE) {//increasing h size if charge surpasses limit

        HT *new_h = malloc(sizeof(HT));//allocating size for new hashtable

        if(new_h == NULL){//malloc fail condition
            return -1;
        }

        int new_size = (h->size)*2;

        while(!isprime(++new_size));//finding smallest prime larger than double h's size

        if(initHT(new_h, new_size) == -1){//initializig new_h with new size
            return -1;
        }

        for(int i = 0; i < h->size; i++) {//writing entries to new hashtable 
            if (!isfreeHT(h, i)){//only valid entries are writen
                if(writeHTaux(new_h, h->tbl[i].key, h->tbl[i].value) == -1){
                    return -1;
                }
            }
        }

        AuxFree(h);//frees previous hashtable

        h->aux_array.array = new_h->aux_array.array;
        h->aux_array.last = new_h->aux_array.last;
        

        //updates table,size, used and entries
        h->tbl = new_h->tbl;
        h->size = new_h->size;
        h->used = new_h->used;
        h->entries = new_h->entries;
        

        free(new_h);
    }
    //writes value to hashtable
    return writeHTaux(h, key, value);
}

/**
 * @brief reads value associated with key
 * 
 * 
 * @param h pointer to hashtable on whitch to read
 * @param key key to be read
 * @param value pointer were pointer to value is placed
 * @return int position were key was found or -1
 */
int readHT(HT *h, int key, GraphListHead** value){

    int p , r = -1 , c = h->size, flg = 1;
    //tries to find key in hashtable, stops if finds EMPTY key or if all table positions have been checked
    for(p = hash(h, key); keycmp(h, key,(h->tbl)[p].key) && (c > 0) && (flg = keycmp(h, EMPTY,(h->tbl)[p].key)); c--){
        p = (p+1)%(h->size);
    }
    //case key has been found changes return value and saves value
    if(c != 0 && flg){
        r = p;
        *value = (h->tbl)[p].value;
    }
    return r;
}

/**
 * @brief substituted key with DELETED and sets value to NULL
 * 
 * 
 * @param h pointer to hashtable were entrie will be deleted
 * @param key key to eliminate
 * @param free_pedido_flag flag that indicates if Pedido should be freed if it is value to be eliminated
 * @return int position were key was eliminated or -1
 */
int deleteHT (HT *h, int key) {

    int* x;
    int p = readHT(h, key, &x);//finds position of key

    if(p != -1) {//case key exists

        //checks key_type and replaces with respective DELETED
        h->tbl[p].key = DELETED;

        //frees value associated with key and sets it to NULL
        
        //removes position from aux_array

        int last = h->aux_array.last;

        if (p == last) {//check if last is position to be removed
            h->aux_array.last = h->aux_array.array[POS(p,0)];//updates last
        }
        
        //position removal logic
        int i1 = h->aux_array.array[POS(p,0)];
        int i2 = h->aux_array.array[POS(p,1)];
        if (i1!=-1)
            h->aux_array.array[POS(i1,1)] = h->aux_array.array[POS(p,1)];
        if (i2!=-1)
            h->aux_array.array[POS(i2,0)] = h->aux_array.array[POS(p,0)];

        h->entries--;//update entries

    }
    return p;
}