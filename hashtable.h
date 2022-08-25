#define INIT_DICT_SIZE 13
#define MAX_CHARGE 0.75
#define MAX_TRANSF_SIZE 32

#define EMPTY         -1
#define DELETED       -2

#define POS(i,j) 2*i +j


/**
 * @brief Estrutura de dados para os nodos da lista duplamente ligada.
 * 
 */
typedef struct graphlistnode {
    
    /*será que a última aresta devia ter como seguinte o nodo à cabeça? para saber que 
    se terminou de percorrer todas as arestas seria necessário comparar com o elemento no array, MELHOR NAO*/
    struct graphlistnode *next;
    //supõe-se necessário o apontador para o nodo anterior para poder remover e poder percorrer faces
    struct graphlistnode *prev;
    struct graphlistnode *equiv;
    int vertex;
} GraphEdgeNode;

typedef struct graphlisthead {
    int v;
    GraphEdgeNode *start;
    GraphEdgeNode *end;
} GraphListHead;

/**
 * @brief 
 * 
 * Responsabilidade do algoritmo de libertar o espaço da cabeça
 */
struct pair {
    int key;
    GraphListHead *value;
};

struct aux_array {
    int *array;
    int last;
};

typedef struct hashTable {
    struct aux_array aux_array;
    int size;
    int used;
    int entries;
    struct pair *tbl;
} HT;

/**
 * @brief Estrutura de dados para o grafo.
 * 
 */
typedef struct graph {
    int v;
    HT *graph;
} Graph;

int hash(HT* h, int key);

int isfreeHT(HT* h, int p);

int keycmp(HT* h, int key1, int key2);

void freeHT(HT *h);

int initHT(HT *h, int size);

int writeHT (HT *h, int key, GraphListHead *value);

int readHT (HT *h, int key, GraphListHead **value);

int deleteHT (HT *h, int key);

//int printHT(HT *h);