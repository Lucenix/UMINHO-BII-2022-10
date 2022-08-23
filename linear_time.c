#include "stdio.h"
#include "malloc.h"

#define MAX_CLOSE_VERTICES 12326458

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
    GraphEdgeNode *start;
    GraphEdgeNode *end;
} GraphListHead;

/**
 * @brief Estrutura de dados para o grafo.
 * 
 */
typedef struct graph {
    int v;
    GraphListHead *array;
} Graph;

/**
 * @brief Função para inicializar um grafo sem arestas com um dado número de vértices.
 * 
 * Necessita receber o número de vértices máximo.
 * 
 * @param graph 
 * @param nVertices 
 * @return int 
 */
int initGraph(Graph **graph, int nVertices) {
    int r = 0;
    (*graph) = malloc(sizeof(Graph));
    (*graph)->v = nVertices;
    if (((*graph)->array = calloc((*graph)->v, sizeof(GraphListHead))) == NULL) {
        r = -1;
        perror("Error: ");
    }
    printf("Debug:");
    for (int i = 0; i<nVertices; i++) {
        printf(" %d", (*graph)->array[i]);
    }
    putchar('\n');
    return r;
}

/**
 * @brief Função para dealocar a memória de uma estrutura do tipo Grafo.
 * 
 * @param graph 
 * @return int 
 */
int freeGraph(Graph *graph) {
    int r = 0, i;
    GraphEdgeNode *nodo, *tmp;
    for (i = 0; i<graph->v; i++) {
        for (nodo = graph->array[i].start; nodo != NULL; nodo = tmp) {            
            tmp = nodo->next;
            free(nodo);
        }
    }
    free(graph->array);
    free(graph);
    return r;
}

/**
 * @brief Função que remove a entrada de uma aresta do grafo.
 * 
 * Função que corre em tempo constante O(D) para D = 59
 * Basta colocar o antecedente para o antecedente do próximo e vice-versa.
 * Também é preciso corrigir a cabeça se for a primeira aresta.
 * 6 alterações
 * 
 * @param graph 
 * @param nodo 
 * @return int 
 */
int removeEdge(Graph *graph, GraphEdgeNode *nodo) {
    int r = 0;

    if (nodo->prev != NULL) {
        nodo->prev->next = nodo->next;
    } else {
        graph->array[nodo->equiv->vertex].start = nodo->next;
    }
    if (nodo->next != NULL) {
        nodo->next->prev = nodo->prev;
    } else {
        graph->array[nodo->equiv->vertex].end = nodo->prev;
    }

    if (nodo->equiv->prev != NULL) {
        nodo->equiv->prev->next = nodo->equiv->next;
    } else {
        graph->array[nodo->vertex].start = nodo->equiv->next;
    }
    if (nodo->equiv->next != NULL) {
        nodo->equiv->next->prev = nodo->equiv->prev;
    } else {
        graph->array[nodo->vertex].end = nodo->equiv->prev;
    }
    
    free(nodo->equiv);
    free(nodo);
    return r;
}

int addEdgePred(Graph *graph, int one, int two, GraphEdgeNode *pred1, GraphEdgeNode *pred2) {    
    int r = 0;
    GraphEdgeNode *new1, *new2;
    if ((new1 = malloc(sizeof(GraphEdgeNode))) == NULL) {
        perror("Error malloc: ");
        return -1;
    }
    if ((new2 = malloc(sizeof(GraphEdgeNode)))  == NULL) {
        perror("Error malloc: ");
        return -1;
    }
    new1->vertex = two;
    new2->vertex = one;
    new1->prev = pred1;
    new2->prev = pred2;
    new1->equiv = new2;
    new2->equiv = new1;
    if (pred1 != NULL) {
        new1->next = pred1->next;
        if (pred1->next != NULL)
            pred1->next->prev = new1;
        else
            graph->array[one].end = new1;
        pred1->next = new1;
    } else {
        new1->next = NULL;
        graph->array[one].start = new1;
        graph->array[one].end = new1;
    }
    if (pred2 != NULL) {
        new2->next = pred2->next;
        if (pred2->next != NULL)
            pred2->next->prev = new2;
        else
            graph->array[two].end = new2;
        pred2->next = new2;
    } else {
        new2->next = NULL;
        graph->array[two].start = new2;
        graph->array[two].end = new2;
    }
    return r;
}

int printGraph(Graph *graph) {
    int r = 0, i;
    GraphEdgeNode *nodo;
    printf("Impressão de grafo.\n");
    for (i = 0; i < graph->v; i++) {
        printf("Adjacentes do vértice %d:", i);
        for (nodo = graph->array[i].start; nodo != NULL; nodo = nodo->next) {
            printf(" %d", nodo->vertex);
        }
        putchar('\n');
    }
    return r;
}

//O que quer dizer remover um vértice nestas condições? Tenho de pensar no grafo reduzido para aplicar recursividade.
//A chamada recursiva não deve dar cores aos vértices que foram removidos. Reconstruir como o grafo depois das reduções?
//Fazer a lista de adjacência por hashtable funcionaria bem, também se podem marcar os vértices removidos com deleção/manter uma lista dos ativos.

int removeIsolatedVertex(Graph *graph, int v) {
    int r = 1;
    //if (r = (graph[v].array->start == NULL)) graph[v].array->deletedFlag = 1;
    return r;
}

/**
 * @brief Função que verifica se o grau de um vértice é maior que D = 59 ou devolve o grau do vértice.
 * 
 * Com maior gasto de espaço, pode-se atualizar os graus dos vértices em tempo constante sem ser necessário tais restrições e percorrer as listas
 * Se este for o bottleneck do tempo constante, facilmente se resolve.
 * 
 * @param graph 
 * @param vertex 
 * @return int o grau de v se <= D ou -1 se > D
 */
int checkDegree(Graph *graph, int vertex) {
    int r = 0, d = 59;
    GraphEdgeNode *nodo;
    for (nodo = graph->array[vertex].start; nodo != NULL && r <= d; nodo = nodo->next, r++);
    return r <= d ? r : -1;
}

/**
 * @brief Função que verifica se dois vértices são adjacentes, tais que o grau mínimo entre eles é <= D. (ASSUME)
 * 
 * Como saber qual deles tem o menor grau, para percorrer apenas os adjacentes desse? 3*D para saber os graus de ambos (saber se são maiores que D)
 * 
 * @param graph 
 * @param v1 
 * @param v2 
 * @return int 
 */
int checkAdjacent(Graph *graph, int v1, int v2) {
    int r = 0, d = 59, g1 = checkDegree(graph, v1), g2;
    int count = 0;
    GraphEdgeNode *nodo;
    if (g1 == -1) {
        for (nodo = graph->array[v2].start; nodo != NULL && !r; nodo = nodo->next) {
            count++;
            r = (nodo->vertex == v1);
        }
    } else if ((g2 = checkDegree(graph, v2)) == -1 || g1 <= g2){
        for (nodo = graph->array[v1].start; nodo != NULL && !r; nodo = nodo->next) {
            count++;
            r = (nodo->vertex == v2);
        }
    } else {
        for (nodo = graph->array[v2].start; nodo != NULL && !r; nodo = nodo->next) {
            count++;
            r = (nodo->vertex == v1);
        }
    }
    //printf("Operações \"Ver adjacentes\": %d.\n", count);
    return r;
}

/**
 * @brief Função que verifica se a distância entre dois vértices é, no máximo, dois.
 * 
 * Para fazer isto tenho de ver se é igual a dois ou se para em um? Faz o mesmo que o anterior mais um pouco?
 * Será que há alguma maneira de verificar apenas pelas arestas do que tem maior grau?
 * 
 * Como está implementado, a complexidade é, no max, D^2.
 * 
 * @param graph 
 * @param v1 
 * @param v2 
 * @return int 
 */
int checkDistanceAtMostTwo(Graph *graph, int v1, int v2) {
    int r = 0;
    int count = 0;
    GraphEdgeNode *nodo1, *nodo2;
    for (nodo1 = graph->array[v1].start; nodo1 != NULL; nodo1 = nodo1->next) {
        count++;
        if (nodo1->vertex == v2) return 1;
        for (nodo2 = graph->array[v2].start; nodo2 != NULL; nodo2 = nodo2->next) {
            count++;
            if (nodo1->vertex == nodo2->vertex || nodo2->vertex == v1) return 1;            
        }
    }
    printf("Nº Operações de Ver Distância Pelo menos 2: %d\n", count);
    return r;
}

/**
 * @brief Função que lista as arestas numa dada facial walk.
 * 
 * Assume-se que a aresta dada é a clockwise mais tardia incidente tanto em vi como na face em questão.
 * Por enquanto, recebe-se o tamanho expectável do facial walk. Provavelmente terá de se alterar com as necessidades do algoritmo.
 * 
 * @param graph 
 * @param vi 
 * @param edge 
 * @param k 
 * @return GraphEdgeNode* 
 */
GraphEdgeNode **edgesInFacialWalk(Graph *graph, int vi, GraphEdgeNode *edge, int k) {
    GraphEdgeNode **list = calloc(k, sizeof(GraphEdgeNode*));
    list[0] = edge;
    int i;
    for (i = 1; i < k && edge->vertex != vi; i++) {
        if (edge->equiv->next == NULL) {
            edge = graph->array[edge->vertex].start;
        } else {
            edge = edge->equiv->next;
        }
        list[i] = edge;
    }
    return list;
}

/**
 * @brief Função que descreve os vértices no máximo a 2 de distância de uma dada aresta, percorrendo uma dada face. Devolve se o tamanho da componente é <=6.
 * 
 * A face subentende-se sobre o vértice final da aresta dada seguindo os ponteiros de relógio para a aresta seguinte.
 * A aresta dada deve ser a mais tardia nos ponteiros de relógio incidente na face pretendida sobre o vértice de origem.
 * 
 * Recua duas arestas para depois descrever os vértices, faz 6 acessos.
 * Devolve 1 se o tamanho da componente é <=6.
 * 
 * @param graph 
 * @param edge 
 * @return int* 
 */
int verticesDistanceAtMostTwoFacialWalk(Graph *graph, GraphEdgeNode *edge, int **vertexList) {
    int i;
    (*vertexList) = malloc(sizeof(int)*7);
    for (i = 0; i<7; i++) {
        (*vertexList)[i] = -1;
    }
    GraphEdgeNode *tmp = edge;
    if (tmp->prev == NULL) tmp = graph->array[tmp->equiv->vertex].end->equiv;
    else tmp = tmp->prev->equiv;
    if (tmp->prev == NULL) tmp = graph->array[tmp->equiv->vertex].end->equiv;
    else tmp = tmp->prev->equiv;
    (*vertexList)[0] = tmp->equiv->vertex;
    for (i = 1; i < 7 && tmp->vertex != (*vertexList)[0]; i++) {
        if (tmp->equiv->next == NULL) {
            tmp = graph->array[tmp->vertex].start;
        } else {
            tmp = tmp->equiv->next;
        }
        (*vertexList)[i] = tmp->equiv->vertex;
    }
    return i<7 && (*vertexList)[i] == -1;
}

Graph *subgraphClose(Graph *graph, int v) {
    Graph *subgraph;
    initGraph(&subgraph, MAX_CLOSE_VERTICES);
    
}

/**
 * @brief Função que devolve o subgrafo correspondente ao caminho de vértices dado.
 * 
 * Como lidar com subgrafos? Inventar arestas novas (espaço) para depois pintar?
 * Tempo D^2, percorrer todas as D arestas de cada vértice para D vértices. NOPE
 * 
 * Saber o array de vértices é necessário para saber qual corresponde a qual no subgrafo.
 * 
 * Como lidar com números iguais usando o array? Fazer um subgrafo com o mesmo número de vértices e depois anular alguns?
 * Caminho não repete vértices!!! HOORAY
 * 
 * @param graph 
 * @param vertices 
 * @param n
 * @return Graph* 
 */
Graph *subgraphFromPath(Graph *graph, int *vertices, int n) {
    int i;
    Graph *subgraph;
    GraphEdgeNode *edge, *pred1, *pred2, *tmp;
    initGraph(&subgraph, n);
    for (i = 0; i<n-1; i++) {
        //procuro apenas enquanto tiver de "encaixar" em algum lado. Há problemas quando as ordens cíclicas não estão coerentes
        pred1 = NULL;
        if ((tmp = subgraph->array[i].start) != NULL) {
            for (edge = graph->array[vertices[i]].start; edge != NULL && edge->vertex != vertices[i+1]; edge = edge->next) {
                if (vertices[tmp->vertex] == edge->vertex) {
                    pred1 = tmp;
                    if (tmp->next == NULL) {
                        tmp = subgraph->array[i].start;
                    } else {
                        tmp = tmp->next;
                    }
                }
            }
        }
        pred2 = NULL;
        if ((tmp = subgraph->array[i+1].start) != NULL) {
            for (edge = graph->array[vertices[i+1]].start; edge != NULL && edge->vertex != vertices[i]; edge = edge->next) {
                if (vertices[tmp->vertex] == edge->vertex) {
                    pred2 = tmp;
                    if (tmp->next == NULL) {
                        tmp = subgraph->array[i+1].start;
                    } else {
                        tmp = tmp->next;
                    }
                }
            }
        }
        addEdgePred(subgraph, i, i+1, pred1, pred2);
    }
    return subgraph;
}

int main(int argc, char const *argv[])
{
    //quanto cuidado ter para manter a descrição clockwise
    Graph *g1;
    printf("%d\n", initGraph(&g1, 5));    
    addEdgePred(g1, 0, 1, g1->array[0].start, g1->array[1].start);
    addEdgePred(g1, 1, 2, g1->array[1].start, g1->array[2].start);
    addEdgePred(g1, 4, 2, g1->array[4].start, g1->array[2].start);
    addEdgePred(g1, 3, 2, g1->array[3].start, g1->array[2].start);
    addEdgePred(g1, 0, 3, g1->array[0].start, g1->array[3].start);
    printGraph(g1);
    //removeEdge(g1, g1->array[2].start->next);
    printGraph(g1);
    printf("Aresta final do vértice 2: %d\n", g1->array[2].end->vertex);
    printf("Adjacente: %d\n", checkAdjacent(g1, 1, 2));
    printf("Distância no max 2: %d, %d\n",checkDistanceAtMostTwo(g1, 4, 0), checkDistanceAtMostTwo(g1, 2, 0));

    GraphEdgeNode **faceWalk = edgesInFacialWalk(g1, 0, g1->array[0].start, 4);
    printf("Arestas na face contida incidente em 0 (espera-se 1 2 3 0):");
    for (int i = 0; i<4; i++) {
        printf(" %d", faceWalk[i]->vertex);
    }
    putchar('\n');
    free(faceWalk);

    int *vertexList;
    printf("Será que a componente tem tamanho at most 6? %d\n", verticesDistanceAtMostTwoFacialWalk(g1, g1->array[1].end, &vertexList));
    printf("Vértices a 2 de distância (espera-se 3 0 1 2):");
    for (int i = 0; i<4; i++) {
        printf(" %d", vertexList[i]);
    }
    putchar('\n');
    free(vertexList);

    int list[] = {0,1,2,3};
    Graph *subg1 = subgraphFromPath(g1, list, 4);
    printGraph(subg1);
    freeGraph(subg1);

    printf("Gottem.\n");
    return 0;
}
