#include "stdio.h"
#include "malloc.h"
#include "linear_time.h"

#include "hashtable.h"

#define MAX_CLOSE_VERTICES 12326458

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
    int r = 0, i;
    (*graph) = malloc(sizeof(Graph));
    (*graph)->v = nVertices;
    (*graph)->graph = malloc(sizeof(HT));
    if (initHT((*graph)->graph, (*graph)->v) == -1) {
        r = -1;
        perror("Error: ");
    }
    for (i = 0; i< nVertices; i++) {
        GraphListHead *head = calloc(1, sizeof(GraphListHead));
        head->v = i;
        writeHT((*graph)->graph, i, head);
    }

    return r;
}

/**
 * @brief Função que retorna a aresta seguinte, nos ponteiros do relógio, à aresta dada.
 * 
 * Se a aresta for única, retorna NULL (a própria não é seguinte a si mesma, ajuda quando se remove).
 * 
 * @param graph 
 * @param edge 
 * @return GraphEdgeNode* 
 */
GraphEdgeNode *nextLoop(Graph *graph, GraphEdgeNode *edge) {
    GraphEdgeNode *r;
    if (edge->next == NULL) {
        GraphListHead *head;
        readHT(graph->graph, edge->equiv->vertex, &head);
        if (head->start == edge) r = NULL;
        else r = head->start;
    } else {
        r = edge->next;
    }
    return r;
}

/**
 * @brief Função que retorna a aresta anterior, nos ponteiros do relógio, à aresta dada.
 * 
 * Se a aresta for única, retorna NULL (a própria não é anterior a si mesma, ajuda quando se remove).
 * 
 * @param graph 
 * @param edge 
 * @return GraphEdgeNode* 
 */
GraphEdgeNode *prevLoop(Graph *graph, GraphEdgeNode *edge) {
    GraphEdgeNode *r;
    if (edge->prev == NULL) {
        GraphListHead *head;
        readHT(graph->graph, edge->equiv->vertex, &head);
        if (head->end == edge) r = NULL;
        else r = head->end;
    } else {
        r = edge->prev;
    }
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
    GraphListHead *head;
    for (i = graph->graph->aux_array.last; i != -1; i = graph->graph->aux_array.array[POS(i, 0)]) {
        readHT(graph->graph, i, &head);
        for (nodo = head->start; nodo != NULL; nodo = tmp) {            
            tmp = nodo->next;
            free(nodo);
        }
        free(head);
    }
    freeHT(graph->graph);
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
    GraphListHead *read;
    readHT(graph->graph, nodo->equiv->vertex, &read);
    if (nodo->prev != NULL) {
        nodo->prev->next = nodo->next;
    } else {
        read->start = nodo->next;
    }
    if (nodo->next != NULL) {
        nodo->next->prev = nodo->prev;
    } else {
        read->end = nodo->prev;
    }

    readHT(graph->graph, nodo->vertex, &read);
    if (nodo->equiv->prev != NULL) {
        nodo->equiv->prev->next = nodo->equiv->next;
    } else {
        read->start = nodo->equiv->next;
    }
    if (nodo->equiv->next != NULL) {
        nodo->equiv->next->prev = nodo->equiv->prev;
    } else {
        read->end = nodo->equiv->prev;
    }
    printf("Help: %d, %d    ", nodo->equiv->vertex, nodo->vertex);
    printf("Help: %d, %d\n", nodo->equiv, nodo);
    free(nodo->equiv);
    free(nodo);
    return r;
}

/**
 * @brief Função que adiciona uma aresta entre dois vértices, dadas arestas predecessoras a adicionar.
 * 
 * @param graph 
 * @param one 
 * @param two 
 * @param pred1 
 * @param pred2 
 * @return GraphEdgeNode* Devolve a aresta criada a partir de one, após o pred1.
 */
GraphEdgeNode *addEdgePred(Graph *graph, int one, int two, GraphEdgeNode *pred1, GraphEdgeNode *pred2) {    
    int r = 0;
    GraphEdgeNode *new1, *new2;
    GraphListHead *head;
    if ((new1 = malloc(sizeof(GraphEdgeNode))) == NULL) {
        perror("Error malloc: ");
        return NULL;
    }
    if ((new2 = malloc(sizeof(GraphEdgeNode))) == NULL) {
        perror("Error malloc: ");
        return NULL;
    }
    new1->vertex = two;
    new2->vertex = one;
    new1->prev = pred1;
    new2->prev = pred2;
    new1->equiv = new2;
    new2->equiv = new1;
    if (readHT(graph->graph, one, &head) == -1) {
        head = calloc(1, sizeof(GraphListHead));
        writeHT(graph->graph, one, head);
    }
    if (pred1 != NULL) {
        new1->next = pred1->next;
        if (pred1->next != NULL)
            pred1->next->prev = new1;
        else
            head->end = new1;
        pred1->next = new1;
    } else {
        new1->next = NULL;
        head->start = new1;
        head->end = new1;
    }

    if (readHT(graph->graph, two, &head) == -1) {
        head = calloc(1, sizeof(GraphListHead));
        writeHT(graph->graph, two, head);
    }
    if (pred2 != NULL) {
        new2->next = pred2->next;
        if (pred2->next != NULL)
            pred2->next->prev = new2;
        else
            head->end = new2;
        pred2->next = new2;
    } else {
        new2->next = NULL;
        head->start = new2;
        head->end = new2;
    }

    return new1;
}

int printGraph(Graph *graph) {
    int r = 0, i;
    GraphEdgeNode *nodo;
    GraphListHead *head;
    printf("Impressão de grafo.\n");
    for (i = graph->graph->aux_array.last; i != -1; i = graph->graph->aux_array.array[POS(i,0)]) {
        readHT(graph->graph, i, &head);
        printf("Adjacentes do vértice %d:", i);
        for (nodo = head->start; nodo != NULL; nodo = nodo->next) {
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
    GraphListHead *head;
    readHT(graph->graph, v, &head);
    if (head->end == NULL && head->start == NULL) {
        deleteHT(graph->graph, v);
        free(head);
    }
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
 * @return int o grau de v se <= D ou 60 se > D, -1 se o vértice não existir
 */
int checkDegree(Graph *graph, int vertex) {
    int r = 0, d = 59;
    GraphEdgeNode *nodo;
    GraphListHead *head;
    if (readHT(graph->graph, vertex, &head) == -1) {
        return -1;
    }
    for (nodo = head->start; nodo != NULL && r <= d; nodo = nodo->next, r++);
    return r;
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
    GraphListHead *head;
    if (g1 == -1) {
        readHT(graph->graph, v2, &head);
        for (nodo = head->start; nodo != NULL && !r; nodo = nodo->next) {
            count++;
            r = (nodo->vertex == v1);
        }
    } else if ((g2 = checkDegree(graph, v2)) == -1 || g1 <= g2){
        readHT(graph->graph, v1, &head);
        for (nodo = head->start; nodo != NULL && !r; nodo = nodo->next) {
            count++;
            r = (nodo->vertex == v2);
        }
    } else {
        readHT(graph->graph, v2, &head);
        for (nodo = head->start; nodo != NULL && !r; nodo = nodo->next) {
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
 * @return int Devolve 1 se for no max 2
 */
int checkDistanceAtMostTwo(Graph *graph, int v1, int v2) {
    int r = 0;
    int count = 0;
    GraphEdgeNode *nodo1, *nodo2;
    GraphListHead *head1, *head2;
    readHT(graph->graph, v1, &head1);
    readHT(graph->graph, v2, &head2);
    for (nodo1 = head1->start; nodo1 != NULL && r == 0; nodo1 = nodo1->next) {
        count++;
        if (nodo1->vertex == v2) r = 1;
        for (nodo2 = head2->start; nodo2 != NULL && r == 0; nodo2 = nodo2->next) {
            count++;
            if (nodo1->vertex == nodo2->vertex || nodo2->vertex == v1) r = 1;            
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
    GraphListHead *head;
    for (i = 1; i < k && edge->vertex != vi; i++) {
        edge = nextLoop(graph, edge->equiv);
        //if (edge->equiv->next == NULL) {
        //    readHT(graph->graph, edge->vertex, &head);
        //    edge = head->start;
        //} else {
        //    edge = edge->equiv->next;
        //}
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
 * Necessita de um vertexList com 7 espaços
 * 
 * Recua duas arestas para depois descrever os vértices, faz 6 acessos.
 * Devolve o tamanho da componente se <=6 ou -1 otherwise.
 * 
 * @param graph 
 * @param edge 
 * @return int* 
 */
int verticesDistanceAtMostTwoFacialWalk(Graph *graph, GraphEdgeNode *edge, int *vertexList) {
    int i, j;
    GraphEdgeNode *tmp = edge;
    GraphListHead *head;
    for (i = 0; i<7; i++) {
        (vertexList)[i] = -1;
    }
    if ((tmp = prevLoop(graph, tmp)) == NULL) return -1;
    if ((tmp = prevLoop(graph, tmp->equiv)) == NULL) return -1;
    tmp = tmp->equiv;
    (vertexList)[0] = tmp->equiv->vertex;
    for (i = 1; i < 7 && tmp->vertex != (vertexList)[0]; i++) {
        if ((tmp = nextLoop(graph, tmp->equiv)) == NULL) i = 7;
        else (vertexList)[i] = tmp->equiv->vertex;
    }
    if (tmp != NULL) tmp = nextLoop(graph, tmp->equiv);
    return i <= 6 && tmp && tmp->vertex == vertexList[1] ? i : -1;
}

Graph *subgraphClose(Graph *graph, int v) {
    Graph *subgraph;
    initGraph(&subgraph, MAX_CLOSE_VERTICES);
    return subgraph;
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
    //rever este com as hashtables para fazer subgrafos decentes
    int i;
    Graph *subgraph;
    GraphEdgeNode *edge, *pred1, *pred2, *tmp;
    GraphListHead *head, *subhead;
    initGraph(&subgraph, n);
    for (i = 0; i<n-1; i++) {
        //procuro apenas enquanto tiver de "encaixar" em algum lado. Há problemas quando as ordens cíclicas não estão coerentes
        pred1 = NULL;
        readHT(graph->graph, vertices[i], &head);
        readHT(subgraph->graph, i, &subhead);
        if ((tmp = subhead->start) != NULL) {
            for (edge = head->start; edge != NULL && edge->vertex != vertices[i+1]; edge = edge->next) {
                if (vertices[tmp->vertex] == edge->vertex) {
                    pred1 = tmp;
                    if (tmp->next == NULL) {
                        tmp = subhead->start;
                    } else {
                        tmp = tmp->next;
                    }
                }
            }
        }
        readHT(graph->graph, vertices[i+1], &head);
        readHT(subgraph->graph, i+1, &subhead);
        if ((tmp = subhead->start) != NULL) {
            for (edge = head->start; edge != NULL && edge->vertex != vertices[i]; edge = edge->next) {
                if (vertices[tmp->vertex] == edge->vertex) {
                    pred2 = tmp;
                    if (tmp->next == NULL) {
                        tmp = subhead->start;
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

int checkTetagramSecurity(Graph *graph, int vertex, int *vertexList) { //v3 v4 v1 v2
    //if (checkDegree(graph, vertex) != 3) {
    //    return 0;
    //} 
    GraphListHead *head;
    GraphEdgeNode *node;
    int x = -1;
    readHT(graph->graph, vertex, &head);
    //só há 3 vértices, ver o prev pode dar NULL e uma dor de cabeça
    for (node = head->start; node != NULL && x == -1; node = node->next) {
        if (node->vertex != vertexList[1] && node->vertex != vertexList[3]) x = node->vertex;
    }
    if (checkDegree(graph, x) == 60) {
        return -1;       
    }
    if (checkDegree(graph, vertexList[0]) == 60) {
        readHT(graph->graph, x, &head);
        node = head->start;
        if (node->next != NULL && node->prev != NULL && node->next->vertex != vertexList[1] && node->next->vertex != vertexList[3] &&
            node->prev->vertex != vertexList[1] && node->prev->vertex != vertexList[3]) {
            for (; node != NULL; node = node->next) {
                if (checkDegree(graph, node->vertex) == 60) return -1;
                else if (checkAdjacent(graph, node->vertex, vertexList[0]) == 0) return -1; //min grau <= D: node->vertex <=> w (safety)
            }
        }
    } else {
        return checkDistanceAtMostTwo(graph, x, vertexList[0]) ? -1 : 4; //max grau <= D: x e v3 (safety)
    }
}

int checkPentagramSecurity(Graph *graph, int vertex, int *vertexList, int x1, int x3) { //v4 v5 v1 v2 v3
    //falta ver: x2 e x4 são C-admissable
    //x2 e x4 distintos não adjacentes de x1 e x3
    //v5 ou x2 não tem vizinho C-forbidden
    //x3 ou x4 não tem vizinho C-forbidden
    //não existe caminho <=3 de x2 para v5
    //caminho de x3 para x4 se existe é de comprimento 2 (existe um x') e faz parte de um facial cycle x3v3v4x4x'
    int x2 = findThirdAdjacent(graph, vertexList[3], vertexList[2], vertexList[4]), 
        x4 = findThirdAdjacent(graph, vertexList[0], vertexList[1], vertexList[4]);
    if (x1 == x2 || x1 == x4 || x2 == x3 || x2 == x4 || x3 == x4 || 
        checkDegree(graph, x2) == 60 || checkDegree(graph, x4) == 60 || checkAdjacent(graph, x1, x2) || checkAdjacent(graph, x1, x4) ||
        checkAdjacent(graph, x2, x3) || checkAdjacent(graph, x2, x4) || checkAdjacent(graph, x3, x4)) return -1;
    
    int flag;
    GraphListHead *head;
    GraphEdgeNode *node;
    readHT(graph->graph, vertexList[1], &head);
    for (node = head->start, flag = 1; node != NULL && flag; node = node->next) {
        if (node->vertex != vertexList[0] && node->vertex != vertexList[2] && 
            (flag = (checkDegree(graph, node->vertex) < 60)) && checkDistanceAtMostTwo(graph, node->vertex, x2)) return -1;
    }
    //se flag == 0, v5 tem C-forbidden.
    readHT(graph->graph, x2, &head);
    for (node = head->start; node != NULL && flag == 0; node = node->next) {
        if (node->vertex != vertexList[3] && (flag = (checkDegree(graph, node->vertex) < 60 - 1)) && 
            checkDistanceAtMostTwo(graph, node->vertex, vertexList[1])) return -1;
    }
    //se flag == -1, x2 tem C-forbidden.
    if (flag == -1) return -1;

    int y = -1;
    int *trash = malloc(sizeof(int) * 7);
    readHT(graph->graph, x3, &head);
    for (node = head->start, flag = 1; node != NULL && flag; node = node->next) {
        if (node->vertex != vertexList[4] && (flag = (checkDegree(graph, node->vertex) < 60)) && 
            checkDistanceAtMostTwo(graph, node->vertex, x4)) {
                if (checkAdjacent(graph, node->vertex, x4) && verticesDistanceAtMostTwoFacialWalk(graph, node, trash) == 5) 
                    y = node->vertex;
                else {
                    return -1;
                }
            }
    }
    //se flag == 0, v5 tem C-forbidden.
    readHT(graph->graph, x4, &head);
    for (node = head->start; node != NULL && flag == 0; node = node->next) {
        if (node->vertex != vertexList[0] && (flag = (checkDegree(graph, node->vertex) < 60 - 1)) && 
            checkDistanceAtMostTwo(graph, node->vertex, x3)) {
                if (checkAdjacent(graph, node->vertex, x3) && verticesDistanceAtMostTwoFacialWalk(graph, node->equiv, trash) == 5) 
                    y = node->vertex;
                else 
                    return -1;
            }
    }
    free(trash);
    //se flag == -1, x2 tem C-forbidden.
    if (y != -1) vertexList[5] = y;
    if (flag == -1) return -1;
    return 5; //tenho de devolver o y? para dar delete às arestas paralelas eventualmente
}

int checkHexagramSecurity(Graph *graph, int vertex, int *vertexList) { //v5 v6 v1 v2 v3 v4
    int r = 6;
    /*checkDegree(graph, vertex) != 3 ||*/
    if (checkDegree(graph, vertexList[1]) == 60 || checkDegree(graph, vertexList[5]) == 60) r = -1;
    else {
        int x = findThirdAdjacent(graph, vertex, vertexList[1], vertexList[3]);
        if (checkDistanceAtMostTwo(graph, x, vertexList[4])) r = -1;
    }
    return r;
}

int findThirdAdjacent(Graph *graph, int v, int x, int y) {
    int r = -1;
    GraphListHead *head;
    GraphEdgeNode *node;
    readHT(graph->graph, v, &head);
    for (node = head->start; node != NULL && r == -1; node = node->next) {
        if (node->vertex != x && node->vertex != y) r = node->vertex;
    }
    return r;
}

/**
 * @brief 
 * 
 * @param graph 
 * @param vertex 
 * @param vertexList 
 * @return int 
 */
int checkMultigramSecurity(Graph *graph, int vertex, int *vertexList, int n) {
    int r = -1, i;
    switch (n)
    {
    case 4: //v3 v4 v1 v2
        //if (checkDegree(graph, vertex) != 3) return 0;
        if (checkDegree(graph, vertexList[0]) != 3 || checkDegree(graph, vertexList[1]) != 3 || checkDegree(graph, vertexList[3]) != 3) {
            return checkTetagramSecurity(graph, vertex, vertexList);
        }
        return 8;
    case 5: //v4 v5 v1 v2 v3
        /*checkDegree(graph, vertexList[2]) != 3 ||*/
        if (checkDegree(graph, vertexList[0]) != 3 || 
            checkDegree(graph, vertexList[3]) != 3 || checkDegree(graph, vertexList[4]) != 3) return -1;

        int x1 = findThirdAdjacent(graph, vertexList[2], vertexList[1], vertexList[3]), 
            x3 = findThirdAdjacent(graph, vertexList[4], vertexList[0], vertexList[3]);

        if (x1 == x3 || (i = checkDegree(graph, vertexList[1])) == 60 || checkDegree(graph, x1) == 60 || 
            checkDegree(graph, x3) == 60 || checkAdjacent(graph, x1, x3)) return -1;
        
        if (i == 3) {
            return checkDistanceAtMostTwo(graph, x1, x3) ? -1 : 10;
        } else {
            return checkPentagramSecurity(graph, vertex, vertexList, x1, x3);
        }
    case 6: //v5 v6 v1 v2 v3 v4
        return checkHexagramSecurity(graph, vertex, vertexList);
    default:
        return -1;
    }
}

/**
 * @brief 
 * 
 * @param graph 
 * @param vertex 
 * @param vertexList 
 * @return int Número do multigrama
 */
int findSecureMultigramFromVertex(Graph *graph, int vertex, int *vertexList) {
    int r = -1, t = checkDegree(graph, vertex);
    GraphEdgeNode *node;
    GraphListHead *head;
    readHT(graph->graph, vertex, &head);
    if (t == 3) {
        for (node = head->start; r == -1 && node != NULL; node = node->next) {
            if ((r = verticesDistanceAtMostTwoFacialWalk(graph, node, vertexList)) > 0) {
                r = checkMultigramSecurity(graph, vertex, vertexList, r); //o vértice pode não ser o primeiro...
            }
        }
    } else if (t == 2 || t == 1 || t == 0) r = t;
    return r;
}

int addCloseToEdge(Graph *graph, VertexQueue *L, int *n, GraphEdgeNode *edge) {
    int vertexList[7];
    int i, r = 0;
    for (i = 0; i<7; i++) {
        vertexList[i] = -1;
    }

    if (checkDegree(graph, edge->vertex) < 60 && L->array[edge->vertex] == 0) {
        L->array[edge->vertex] = 1;
        L->queue[(*n)++] = edge->vertex;
        r++;
    }
    if (checkDegree(graph, edge->equiv->vertex) < 60 && L->array[edge->equiv->vertex] == 0) {
        L->array[edge->equiv->vertex] = 1;
        L->queue[(*n)++] = edge->equiv->vertex;
        r++;
    }
    
    for (i = verticesDistanceAtMostTwoFacialWalk(graph,edge,vertexList); i>0; i--) {
        if (checkDegree(graph, vertexList[i-1]) < 60 && L->array[vertexList[i-1]] == 0) {
            L->array[vertexList[i-1]] = 1;
            L->queue[(*n)++] = vertexList[i-1];
            r++;
        }
    }
    
    return r;
}

int addCloseEdge(Graph *graph, VertexQueue *L, int *n, GraphEdgeNode *edge) {
    int r = 0;
    r += addCloseToEdge(graph, L, n, edge);
    //r += addCloseToEdge(graph, L, n, edge->equiv);
    return r;
}

int addCloseVertice(Graph *graph, VertexQueue *L, int *n, int vertice) {
    int r = 0;
    if (checkDegree(graph, vertice) == 60) return 0;
    GraphListHead *head;
    GraphEdgeNode *node, *edge1, *edge2, *edge3;
    //MAX 1+4*59+59^2+59^3+59^4
    readHT(graph->graph, vertice, &head);
    for (node = head->start; node != NULL; node = node->next) {
        r += addCloseEdge(graph, L, n, node);
        if (checkDegree(graph, node->vertex) < 60) {
            if (checkDegree(graph, node->vertex) == 3 && L->array[node->vertex] == 0) {
                L->array[node->vertex] = 1;
                L->queue[(*n)++] = node->vertex;
                r++;
            }

            readHT(graph->graph, node->vertex, &head);
            for (edge1 = head->start; edge1 != NULL; edge1 = edge1->next) {
                if (checkDegree(graph, edge1->vertex) < 60) {
                    if (L->array[edge1->vertex] == 0) {
                        L->array[edge1->vertex] = 1;
                        L->queue[(*n)++] = edge1->vertex;
                        r++;
                    }

                    readHT(graph->graph, edge1->vertex, &head);
                    for (edge2 = head->start; edge2 != NULL; edge2 = edge2->next) {
                        if (checkDegree(graph, edge2->vertex) < 60) {
                            if (L->array[edge2->vertex] == 0) {
                                L->array[edge2->vertex] = 1;
                                L->queue[(*n)++] = edge2->vertex;
                                r++;
                            }

                            readHT(graph->graph, edge2->vertex, &head);
                            for (edge3 = head->start; edge3 != NULL; edge3 = edge3->next) {
                                if (checkDegree(graph, edge2->vertex) < 60) {
                                    if (L->array[edge3->vertex] == 0) {
                                        L->array[edge3->vertex] = 1;
                                        L->queue[(*n)++] = edge3->vertex;
                                        r++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return r;
}

int colorMonogram1(Graph *graph, VertexQueue *L, int n, int *coloring, int v, int *vertexList) {
    int a, i;
    GraphListHead *head;
    readHT(graph->graph, v, &head);
    a = head->start->vertex;
    addCloseEdge(graph, L, &n, head->start);
    removeEdge(graph, head->start);
    addCloseVertice(graph, L, &n, a);
    addCloseVertice(graph, L, &n, v);
    printf("Color Monogram 1\n");
    coloringRecursiveStep(graph, L, n, coloring, vertexList);
    for (i = 0; i<3; i++) {
        if (coloring[a] != i) coloring[v] = i;
    }
    return 0;
}

int colorMonogram2(Graph *graph, VertexQueue *L, int n, int *coloring, int v, int *vertexList) {
    int a1, a2, i;
    GraphListHead *head;
    //GraphEdgeNode *node;
    readHT(graph->graph, v, &head);
    a1 = head->start->vertex;
    a2 = head->end->vertex;
    addCloseEdge(graph, L, &n, head->start);
    addCloseEdge(graph, L, &n, head->end);
    addCloseVertice(graph, L, &n, v);
    removeEdge(graph, head->end);
    removeEdge(graph, head->start);
    removeIsolatedVertex(graph, v);
    //add to L all close to head->end and head->start and a1, a2
    addCloseVertice(graph, L, &n, a1);
    addCloseVertice(graph, L, &n, a2);
    printf("Color Monogram 2\n");
    coloringRecursiveStep(graph, L, n, coloring, vertexList);
    for (i = 0; i<3; i++) {
        if (coloring[a1] != i && coloring[a2] != i) coloring[v] = i;
    }
    return 0;
}

int colorTetagram(Graph *graph, VertexQueue *L, int n, int *coloring, int *vertexList) { //v3 v4 v1 v2
    int v1 = vertexList[2], v3 = vertexList[0], x, a;
    GraphListHead *head;
    GraphEdgeNode *edge, *pred3, *predx, *tmp;
    readHT(graph->graph, v1, &head);
    addCloseVertice(graph, L, &n, v1);
    for (edge = head->start; edge != NULL; edge = tmp) {
        a = edge->vertex;
        if (a == vertexList[3]) {
            pred3 = nextLoop(graph, edge->equiv)->equiv;
        } else if (a != vertexList[1]) {
            x = a;
            predx = prevLoop(graph, edge->equiv);
        }
        tmp = edge->next;
        addCloseEdge(graph, L, &n, edge);
        removeEdge(graph, edge);
        addCloseVertice(graph, L, &n, v1);
        addCloseVertice(graph, L, &n, a);
    }
    removeIsolatedVertex(graph, v1);
    addCloseVertice(graph, L, &n, v3);
    addCloseVertice(graph, L, &n, x);
    addEdgePred(graph, v3, x, pred3, predx);
    predx = nextLoop(graph, pred3); //v3-x
    addCloseEdge(graph, L, &n, predx);

    addCloseVertice(graph, L, &n, vertexList[3]); //pelas arestas que não adicionei
    addCloseVertice(graph, L, &n, vertexList[1]);
    addCloseEdge(graph, L, &n, pred3); //v3-v2 //pelas arestas que não adicionei 
    addCloseEdge(graph, L, &n, nextLoop(graph, predx)); //v3-v4 //pelas arestas que não adicionei
    //addCloseVertice(graph, L, &n, vertexList[3]); //pelas arestas que não removi
    //addCloseVertice(graph, L, &n, vertexList[1]);

    coloringRecursiveStep(graph, L, n, coloring, vertexList); //altera o vertexList porque utiliza sempre o mesmo espaço
    coloring[v1] = coloring[v3];
    printf("Color Tetagram %d %d %d %d\n", vertexList[2], vertexList[3], vertexList[0], vertexList[1]);
    return 0;
}

int colorOctogram(Graph *graph, VertexQueue *L, int n, int *coloring, int *vertexList) { //v3 v4 v1 v2
    int x1, x2, x3, x4, i, a1, a2, j, v1, v2, v3, v4;
    GraphListHead *head;
    GraphEdgeNode *node, *tmp;
    v1 = vertexList[2]; v2 = vertexList[3]; v3 = vertexList[0]; v4 = vertexList[1];
    x1 = findThirdAdjacent(graph, v1, v4, v2);
    x2 = findThirdAdjacent(graph, v2, v1, v3);
    x3 = findThirdAdjacent(graph, v3, v2, v4);
    x4 = findThirdAdjacent(graph, v4, v3, v1);
    for (i = 0; i<4; i++) {
        readHT(graph->graph, vertexList[i], &head);
        for (node = head->start; node != NULL; node = tmp) {
            a1 = node->equiv->vertex;
            a2 = node->vertex;
            tmp = node->next;
            addCloseEdge(graph, L, &n, node);
            removeEdge(graph, node);
            addCloseVertice(graph, L, &n, a1);
            addCloseVertice(graph, L, &n, a2);
        }
        removeIsolatedVertex(graph, vertexList[i]);
    }
    printf("Color Octagram\n");
    coloringRecursiveStep(graph, L, n, coloring, vertexList);

    for (i = 0; i<3 && (coloring[x1] == i || coloring[x3] == i); i++) ;
    coloring[v1] = i;
    coloring[v3] = i;
    for (j = 0; j<3; j++) {
        if (j != coloring[x2] && j != i) coloring[v2] = j;
        if (j != coloring[x4] && j != i) coloring[v4] = j;
    }
    return 0;
}

int colorHexagram(Graph *graph, VertexQueue *L, int n, int *coloring, int *vertexList) { //v5 v6 v1 v2 v3 v4
    int v1 = vertexList[2], v3 = vertexList[4], x, a;
    GraphListHead *head;
    GraphEdgeNode *edge, *pred3, *predx, *tmp;
    readHT(graph->graph, v1, &head);
    addCloseVertice(graph, L, &n, v1);
    for (edge = head->start; edge != NULL; edge = tmp) {
        a = edge->vertex;
        if (a == vertexList[3]) {
            pred3 = nextLoop(graph, edge->equiv)->equiv;
        } else if (a != vertexList[1]) {
            x = a;
            predx = prevLoop(graph, edge->equiv);
        }
        tmp = edge->next;
        addCloseEdge(graph, L, &n, edge);
        removeEdge(graph, edge);
        addCloseVertice(graph, L, &n, v1);
        addCloseVertice(graph, L, &n, a);
    }
    removeIsolatedVertex(graph, v1);

    addCloseVertice(graph, L, &n, v3);
    addCloseVertice(graph, L, &n, x);
    predx = addEdgePred(graph, v3, x, pred3, predx);
    addCloseEdge(graph, L, &n, predx);

    readHT(graph->graph, vertexList[1], &head);
    for (edge = head->start; edge != NULL && edge->vertex != vertexList[0]; edge = edge->next) ;

    addCloseVertice(graph, L, &n, v3);
    addCloseVertice(graph, L, &n, vertexList[1]);
    predx = addEdgePred(graph, v3, vertexList[1], predx, edge);
    addCloseEdge(graph, L, &n, predx);

    addCloseVertice(graph, L, &n, vertexList[3]); //pelas arestas que não adicionei
    addCloseEdge(graph, L, &n, pred3); //v3-v2

    printf("Color Hexagram\n");
    coloringRecursiveStep(graph, L, n, coloring, vertexList); //altera o vertexList porque utiliza sempre o mesmo espaço
    coloring[v1] = coloring[v3];
    return 0;
}

int colorDecagram(Graph *graph, VertexQueue *L, int n, int *coloring, int *vertexList) { //v4 v5 v1 v2 v3
    int v1, v2, v3, v4, v5, x1, x2, x3, x4, x5, a1, a2, i, j;
    GraphListHead *head;
    GraphEdgeNode *node, *tmp, *predx1, *predx3;
    v1 = vertexList[2]; v2 = vertexList[3]; v3 = vertexList[4]; v4 = vertexList[0], v5 = vertexList[1];
    x1 = findThirdAdjacent(graph, v1, v5, v2);
    x2 = findThirdAdjacent(graph, v2, v1, v3);
    x3 = findThirdAdjacent(graph, v3, v2, v4);
    x4 = findThirdAdjacent(graph, v4, v3, v5);
    x5 = findThirdAdjacent(graph, v5, v4, v1);
    for (i = 0; i<5; i++) {
        readHT(graph->graph, vertexList[i], &head);
        for (node = head->start; node != NULL; node = tmp) {
            //a1 = head->start->equiv->vertex;
            a2 = node->vertex;
            if (a2 == x1) {
                predx1 = prevLoop(graph, head->start->equiv);
            } else if (a2 == x3) {
                predx3 = nextLoop(graph, node->equiv);
            }
            tmp = node->next;
            addCloseEdge(graph, L, &n, node);
            printf("Adjacente: %d (queremos 4)\n", node->vertex);
            removeEdge(graph, node);
            addCloseVertice(graph, L, &n, vertexList[i]);
            addCloseVertice(graph, L, &n, a2);
        }
        removeIsolatedVertex(graph, vertexList[i]);
    }

    addCloseVertice(graph, L, &n, x1);
    addCloseVertice(graph, L, &n, x3);
    node = addEdgePred(graph, x1, x3, predx1, predx3);
    addCloseEdge(graph, L, &n, node);
    printf("Color Decagram %d %d %d %d %d\n", v1,v2,v3,v4,v5);

    coloringRecursiveStep(graph, L, n, coloring, vertexList);

    a1 = coloring[x1];
    a2 = coloring[x3];
    for (i = 0; i<3 && (i == a1 || i == a2); i++) ;
    if (coloring[x2] == a2) {
        coloring[v2] = a1;
        coloring[v3] = i;
        if (coloring[x4] == a2) {
            coloring[v4] = a1;
            if (coloring[x5] == a2) {
                coloring[v5] = i;
                coloring[v1] = a2;
            } else {
                coloring[v5] = a2;
                coloring[v1] = i;
            }
        } else {
            coloring[v1] = a2;
            coloring[v4] = a2;
            for (i = 0; i<3 && (coloring[x5] == i || a2 == i); i++) ;
            coloring[v5] = i;
        }
    } else {
        coloring[v2] = a2;
        coloring[v1] = i;
        if (coloring[x5] == a1) {
            coloring[v5] = a2;
            if (coloring[x4] == a1) {
                coloring[v4] = i;
                coloring[v3] = a1;
            } else {
                coloring[v4] = a1;
                coloring[v3] = i;
            }
        } else {
            coloring[v3] = a1;
            coloring[v5] = a1;
            for (i = 0; i<3 && (coloring[x4] == i || a1 == i); i++) ;
            coloring[v4] = i;
        }
    }
    return 0;
}

int colorPentagram(Graph *graph, VertexQueue *L, int n, int *coloring, int *vertexList) {//v4 v5 v1 v2 v3 y -1
    int v1, v2, v3, v4, v5, x1, x2, x3, x4, y, a1, a2, i, j;
    GraphListHead *head;
    GraphEdgeNode *node, *tmp, *predx2, *startv5, *predx4, *startx3;
    v4 = vertexList[0], v5 = vertexList[1], v1 = vertexList[2], v2 = vertexList[3], v3 = vertexList[4], y = vertexList[5];
    x1 = findThirdAdjacent(graph, v1, v5, v2);
    x2 = findThirdAdjacent(graph, v2, v1, v3);
    x3 = findThirdAdjacent(graph, v3, v2, v4);
    x4 = findThirdAdjacent(graph, v4, v3, v5);
    //remover v1, v2, v3, v4 lembrando das arestas que são antecessoras aos vértices que vão ser identificados
    for (i = 0; i<5; i++) {
        if (i != 1) {
            readHT(graph->graph, vertexList[i], &head);
            for (node = head->start; node != NULL; node = tmp) {
                //a1 = head->start->equiv->vertex;
                a2 = node->vertex;
                if (a2 == x2) {
                    predx2 = prevLoop(graph, node->equiv);
                } else if (a2 == v5 && vertexList[i] == v1) {
                    startv5 = nextLoop(graph, node->equiv);
                } else if (a2 == x4) {
                    predx4 = prevLoop(graph, node->equiv);
                } else if (a2 == x3) {
                    startx3 = nextLoop(graph, node->equiv);
                    //if (y != -1 && startx3->vertex == y) startx3 = nextLoop(graph, startx3);
                }
                tmp = node->next;
                addCloseEdge(graph, L, &n, node);
                removeEdge(graph, node);
                addCloseVertice(graph, L, &n, vertexList[i]);
                addCloseVertice(graph, L, &n, a2);
            }
        removeIsolatedVertex(graph, vertexList[i]);
        }
    }

    //identificar v5 e x2
    readHT(graph->graph, v5, &head);
    for (startv5; head->start != NULL; startv5 = node) {
        a2 = startv5->vertex;
        
        addCloseVertice(graph, L, &n, x2);
        addCloseVertice(graph, L, &n, a2);
        node = addEdgePred(graph, x2, a2, predx2, startv5->equiv);
        addCloseEdge(graph, L, &n, node);
        predx2 = nextLoop(graph, predx2);

        node = nextLoop(graph, startv5);
        addCloseEdge(graph, L, &n, startv5);
        removeEdge(graph, startv5);
        addCloseVertice(graph, L, &n, v5);
        addCloseVertice(graph, L, &n, a2);
    }
    removeIsolatedVertex(graph, v5);

    //identificar v5 e x2
    readHT(graph->graph, x3, &head);
    for (startx3; head->start != NULL; startx3 = node) {
        a2 = startx3->vertex;
        addCloseVertice(graph, L, &n, x4);
        addCloseVertice(graph, L, &n, a2);
        //se for adicionar uma nova tem de se adicionar os close dessa e avançar o predx4
        if (a2 != y) {
            node = addEdgePred(graph, x4, a2, predx4, startx3->equiv);
            addCloseEdge(graph, L, &n, node);
            predx4 = node; //nextLoop(graph, predx4);, devolve sempre o new1
        } else {
            addCloseEdge(graph, L, &n, nextLoop(graph, startx3->equiv));
        }

        node = nextLoop(graph, startx3);
        addCloseEdge(graph, L, &n, startx3);
        removeEdge(graph, startx3);
        addCloseVertice(graph, L, &n, x3);
        addCloseVertice(graph, L, &n, a2);
    }
    removeIsolatedVertex(graph, x3);
    printf("Color Pentagram %d %d %d %d %d\n", v1, v2, v3, v4, v5);
    coloringRecursiveStep(graph, L, n, coloring, vertexList);

    coloring[x3] = coloring[x4];
    coloring[v5] = coloring[x2];
    if (coloring[x1] == coloring[x2]) {
        for (i = 0; i<3 && (i == coloring[x4] || i == coloring[v5]); i++) ;
        coloring[v4] = i;
        for (i = 0; i<3 && (i == coloring[x3] || i == coloring[v4]); i++) ;
        coloring[v3] = i;
        for (i = 0; i<3 && (i == coloring[x2] || i == coloring[v3]); i++) ;
        coloring[v2] = i;
        for (i = 0; i<3 && (i == coloring[x1] || i == coloring[v2]); i++) ;
        coloring[v1] = i;
    } else if (coloring[x2] == coloring[x4]) {
        for (i = 0; i<3 && (i == coloring[x1] || i == coloring[v5]); i++) ;
        coloring[v1] = i;
        for (i = 0; i<3 && (i == coloring[x2] || i == coloring[v1]); i++) ;
        coloring[v2] = i;
        for (i = 0; i<3 && (i == coloring[x3] || i == coloring[v2]); i++) ;
        coloring[v3] = i;
        for (i = 0; i<3 && (i == coloring[x4] || i == coloring[v3]); i++) ;
        coloring[v4] = i;
    } else if (coloring[x1] == coloring[x3]) {
        for (i = 0; i<3 && (i == coloring[x1] || i == coloring[v5]); i++) ;
        coloring[v1] = i;
        for (i = 0; i<3 && (i == coloring[x2] || i == coloring[v1]); i++) ;
        coloring[v2] = i;
        for (i = 0; i<3 && (i == coloring[x4] || i == coloring[v5]); i++) ;
        coloring[v4] = i;
        for (i = 0; i<3 && (i == coloring[x3] || i == coloring[v4]); i++) ;
        coloring[v3] = i;
    } else {
        coloring[v2] = coloring[x1];
        coloring[v3] = coloring[x2];
        for (i = 0; i<3 && (i == coloring[x1] || i == coloring[x2]); i++) ;
        coloring[v1] = i;
        for (i = 0; i<3 && (i == coloring[x2] || i == coloring[x3]); i++) ;
        coloring[v4] = i;
    }
    return 0;
}

int coloringRecursiveStep(Graph *graph, VertexQueue *L, int n, int *coloring, int *vertexList) {
    if (n == 0) 
        return 0;
    if (n == 1) {
        coloring[L->queue[0]] = 0;
        return 1;
    }

    int v, result;
    for (v = L->queue[--n], L->array[v] = 0; 
        n>0 && ((result = isfreeHT(graph->graph, v)) || ((result = findSecureMultigramFromVertex(graph, v, vertexList)) == -1) || result == 0);
        v = L->queue[--n], L->array[v] = 0) {
            if (result == 0) coloring[v] = 0;
        }
    if (n == 0) {
        coloring[L->queue[0]] = 0;
    } else if (n>0) {
        switch (result)
        {
        //case 0:
        //    coloring[v] = 0;
        //    coloringRecursiveStep(graph, L, n, coloring, vertexList);
        //    break;
        case 1:
            colorMonogram1(graph, L, n, coloring, v, vertexList);
            break;
        case 2:
            colorMonogram2(graph, L, n, coloring, v, vertexList);
            break;
        case 4:
            colorTetagram(graph, L, n, coloring, vertexList);
            break;
        case 8:
            colorOctogram(graph, L, n, coloring, vertexList);
            break;
        case 5:
            colorPentagram(graph, L, n, coloring, vertexList);
            break;
        case 10:
            colorDecagram(graph, L, n, coloring, vertexList);
            break;
        case 6:
            colorHexagram(graph, L, n, coloring, vertexList);
            break;
        default:
            break;
        }
    }
    return result;
}

int *linearTimeColoring(Graph *graph) {
    int *coloring = malloc(sizeof(int) * graph->graph->entries);
    VertexQueue L;
    L.array = calloc(graph->graph->entries, sizeof(int));
    L.queue = malloc(graph->graph->entries * sizeof(int));
    int *vertexList = malloc(sizeof(int) * 7);
    int i, w;
    for (i = graph->graph->aux_array.last, w = 0; i != -1; i = graph->graph->aux_array.array[POS(i, 0)]) {
        if (checkDegree(graph, i) <= 3) {L.array[i] = 1; L.queue[w++] = i;}
    }

    coloringRecursiveStep(graph, &L, w, coloring, vertexList);
    free(L.array);
    free(L.queue);
    free(vertexList);
    return coloring;
}

int checkColoring(Graph *graph, int *coloring) {
    int i;
    GraphListHead *head;
    GraphEdgeNode *node;
    for (i = 0; i < graph->graph->entries; i++) {
        readHT(graph->graph, i, &head);
        for (node = head->start; node != NULL; node = node->next) {
            if (coloring[i] == coloring[node->vertex]) return 0;
        }
    }
    return 1;
}

void generateGraph1(Graph **graph) {
    printf("%d\n", initGraph(graph, 10));
    Graph *g1 = (*graph);
    GraphListHead *head1, *head2;
    readHT(g1->graph, 0, &head1);
    readHT(g1->graph, 6, &head2);
    addEdgePred(g1, 0, 6, head1->start, head2->start);
    readHT(g1->graph, 1, &head2);
    addEdgePred(g1, 0, 1, head1->start, head2->start);
    readHT(g1->graph, 6, &head1);
    readHT(g1->graph, 5, &head2);
    //addEdgePred(g1, 6, 5, head1->start, head2->start);
    readHT(g1->graph, 4, &head1);
    addEdgePred(g1, 4, 5, head1->start, head2->start);
    readHT(g1->graph, 1, &head1);
    addEdgePred(g1, 1, 5, head1->start, head2->end);
    readHT(g1->graph, 4, &head1);
    readHT(g1->graph, 3, &head2);
    addEdgePred(g1, 4, 3, head1->start, head2->start);
    readHT(g1->graph, 2, &head1);
    addEdgePred(g1, 2, 3, head1->start, head2->start);
    addEdgePred(g1, 2, 10, head1->start, NULL);
    readHT(g1->graph, 1, &head2);
    //addEdgePred(g1, 2, 1, head1->start, head2->start->next);
    //addEdgePred(g1, 7, 9, NULL, NULL);
    readHT(g1->graph, 7, &head2);
    addEdgePred(g1, 2, 7, head1->start, head2->start);
    readHT(g1->graph, 1, &head1);
    readHT(g1->graph, 10, &head2);
    addEdgePred(g1, 1, 10, head1->end, head2->start);
    readHT(g1->graph, 10, &head1);
    //addEdgePred(g1, 10, 7, head1->start, head2->start);
    readHT(g1->graph, 4, &head1);
    readHT(g1->graph, 8, &head2);
    //addEdgePred(g1, 4, 8, head1->start, head2->start);
    readHT(g1->graph, 9, &head1);
    //addEdgePred(g1, 9, 8, head1->start, head2->start);
    readHT(g1->graph, 10, &head1);
    //readHT(g1->graph, 12, &head2);
    addEdgePred(g1, 10, 12, head1->end, NULL);
    readHT(g1->graph, 12, &head1);
    addEdgePred(g1, 12, 11, head1->start, NULL);
    readHT(g1->graph, 11, &head1);
    readHT(g1->graph, 4, &head2);
    addEdgePred(g1, 11, 4, head1->start, head2->start);

    readHT(g1->graph, 9, &head1);
    readHT(g1->graph, 3, &head2);
    addEdgePred(g1, 9, 3, head1->start, head2->start);
    //readHT(g1->graph, 6, &head1);
    //addEdgePred(g1, 6, 8, head1->start, head2->start);
}

void generateGraph2(Graph **graph) {
    printf("%d\n", initGraph(graph, 12));
    Graph *g2 = (*graph);
    GraphListHead *head1, *head2;
    readHT(g2->graph, 0, &head1);
    addEdgePred(g2, 0, 1, head1->start, NULL);
    addEdgePred(g2, 0, 2, head1->start, NULL);
    addEdgePred(g2, 0, 3, head1->start, NULL);
    readHT(g2->graph, 1, &head1);
    addEdgePred(g2, 1, 11, head1->start, NULL);
    addEdgePred(g2, 1, 10, head1->start, NULL);
    addEdgePred(g2, 1, 4, head1->start, NULL);
    readHT(g2->graph, 2, &head1);
    addEdgePred(g2, 2, 5, head1->start, NULL);
    addEdgePred(g2, 2, 6, head1->start, NULL);
    readHT(g2->graph, 3, &head1);
    readHT(g2->graph, 10, &head2);
    addEdgePred(g2, 3, 7, head1->start, NULL);
    addEdgePred(g2, 3, 10, head1->start, head2->start);
    readHT(g2->graph, 4, &head1);
    addEdgePred(g2, 4, 8, head1->start, NULL);
    readHT(g2->graph, 8, &head1);
    readHT(g2->graph, 5, &head2);
    addEdgePred(g2, 8, 5, head1->start, head2->start);
    readHT(g2->graph, 9, &head1);
    readHT(g2->graph, 6, &head2);
    addEdgePred(g2, 9, 6, head1->start, head2->start);
    readHT(g2->graph, 7, &head2);
    addEdgePred(g2, 9, 7, head1->start, head2->start);
}


void generateGraph3(Graph **graph) {
    printf("%d\n", initGraph(graph, 17));
    Graph *g2 = (*graph);
    GraphListHead *head1, *head2;
    readHT(g2->graph, 1, &head1);
    addEdgePred(g2, 1, 4, head1->start, NULL);
    addEdgePred(g2, 1, 2, head1->start, NULL);
    readHT(g2->graph, 4, &head1);
    addEdgePred(g2, 4, 3, head1->start, NULL);
    readHT(g2->graph, 2, &head1);
    addEdgePred(g2, 2, 0, head1->start, NULL);
    addEdgePred(g2, 2, 5, head1->start, NULL);
    addEdgePred(g2, 2, 6, head1->start, NULL);
    readHT(g2->graph, 3, &head1);
    readHT(g2->graph, 0, &head2);
    addEdgePred(g2, 3, 7, head1->start, NULL);
    addEdgePred(g2, 3, 0, head1->start, head2->start);
    readHT(g2->graph, 0, &head1);
    addEdgePred(g2, 0, 8, head1->start, NULL);
    readHT(g2->graph, 5, &head1);
    readHT(g2->graph, 8, &head2);
    addEdgePred(g2, 5, 8, head1->start, head2->start);
    addEdgePred(g2, 5, 9, head1->start, NULL);
    readHT(g2->graph, 6, &head1);
    addEdgePred(g2, 6, 10, head1->start, NULL);
    readHT(g2->graph, 7, &head1);
    addEdgePred(g2, 7, 11, head1->start, NULL);
    readHT(g2->graph, 8, &head1);
    addEdgePred(g2, 8, 12, head1->start, NULL);
    addEdgePred(g2, 8, 13, head1->start, NULL);
    readHT(g2->graph, 9, &head1);
    readHT(g2->graph, 10, &head2);
    addEdgePred(g2, 9, 14, head1->start, NULL);
    addEdgePred(g2, 9, 10, head1->start, head2->start);
    readHT(g2->graph, 11, &head1);
    readHT(g2->graph, 12, &head2);
    addEdgePred(g2, 11, 12, head1->start, head2->start);
    readHT(g2->graph, 13, &head1);
    addEdgePred(g2, 13, 15, head1->start, NULL);
    addEdgePred(g2, 13, 14, head1->start, NULL);
    readHT(g2->graph, 14, &head1);
    addEdgePred(g2, 14, 16, head1->start, NULL);
}

void generateGraph4(Graph **graph) {
    initGraph(graph, 2);
    addEdgePred((*graph), 0, 1, NULL, NULL);
}

void generateGraph5(Graph **graph) {
    initGraph(graph, 12);
    Graph *g1 = (*graph);
    GraphListHead *head1, *head2;
    readHT(g1->graph, 0, &head1);
    addEdgePred(g1, 0, 1, head1->start, NULL);
    addEdgePred(g1, 0, 11, head1->start, NULL);
    addEdgePred(g1, 0, 4, head1->start, NULL);
    readHT(g1->graph, 1, &head1);
    addEdgePred(g1, 1, 2, head1->start, NULL);
    readHT(g1->graph, 2, &head1);
    addEdgePred(g1, 2, 3, head1->start, NULL);
    addEdgePred(g1, 2, 8, head1->start, NULL);
    readHT(g1->graph, 3, &head1);
    readHT(g1->graph, 4, &head2);
    addEdgePred(g1, 12, 4, NULL, head2->start);
    addEdgePred(g1, 3, 4, head1->start, head2->start);
    addEdgePred(g1, 3, 10, head1->start, NULL);
    readHT(g1->graph, 5, &head1);
    readHT(g1->graph, 11, &head2);
    addEdgePred(g1, 5, 11, head1->start, head2->start);
    addEdgePred(g1, 5, 6, head1->start, NULL);
    readHT(g1->graph, 6, &head1);
    addEdgePred(g1, 6, 7, head1->start, NULL);
    readHT(g1->graph, 8, &head1);
    readHT(g1->graph, 7, &head2);
    addEdgePred(g1, 8, 9, head1->start, NULL);
    addEdgePred(g1, 8, 7, head1->start, head2->start);
    readHT(g1->graph, 9, &head1);
    readHT(g1->graph, 10, &head2);
    addEdgePred(g1, 9, 10, head1->start, head2->start);
}

void generateGraph6(Graph **graph) {
    initGraph(graph, 20);
    Graph *g1 = (*graph);
    GraphListHead *head1, *head2;
    readHT(g1->graph, 0, &head1);
    addEdgePred(g1, 0, 4, head1->start, NULL);
    addEdgePred(g1, 0, 1, head1->start, NULL);
    addEdgePred(g1, 0, 5, head1->start, NULL);
    readHT(g1->graph, 1, &head1);
    addEdgePred(g1, 1, 2, head1->start, NULL);
    addEdgePred(g1, 1, 7, head1->start, NULL);
    readHT(g1->graph, 2, &head1);
    addEdgePred(g1, 2, 3, head1->start, NULL);
    addEdgePred(g1, 2, 9, head1->start, NULL);
    readHT(g1->graph, 3, &head1);
    readHT(g1->graph, 4, &head2);
    addEdgePred(g1, 3, 4, head1->start, head2->start);
    addEdgePred(g1, 3, 11, head1->start, NULL);
    readHT(g1->graph, 4, &head1);
    addEdgePred(g1, 4, 13, head1->start, NULL);
    readHT(g1->graph, 5, &head1);
    addEdgePred(g1, 5, 6, head1->start, NULL);
    addEdgePred(g1, 5, 14, head1->start, NULL);
    readHT(g1->graph, 14, &head1);
    readHT(g1->graph, 13, &head2);
    addEdgePred(g1, 14, 15, head1->start, NULL);
    addEdgePred(g1, 14, 13, head1->start, head2->start);
    readHT(g1->graph, 12, &head1);
    addEdgePred(g1, 12, 13, head1->start, head2->start);
    addEdgePred(g1, 12, 19, head1->start, NULL);
    readHT(g1->graph, 11, &head2);
    addEdgePred(g1, 12, 11, head1->start, head2->start);
    readHT(g1->graph, 10, &head1);
    addEdgePred(g1, 10, 11, head1->start, head2->start);
    addEdgePred(g1, 10, 18, head1->start, NULL);
    readHT(g1->graph, 9, &head2);
    addEdgePred(g1, 10, 9, head1->start, head2->start);
    readHT(g1->graph, 8, &head1);
    addEdgePred(g1, 8, 9, head1->start, head2->start);
    addEdgePred(g1, 8, 17, head1->start, NULL);
    readHT(g1->graph, 7, &head2);
    addEdgePred(g1, 8, 7, head1->start, head2->start);
    readHT(g1->graph, 6, &head1);
    addEdgePred(g1, 6, 7, head1->start, head2->start);
    addEdgePred(g1, 6, 16, head1->start, NULL);
    readHT(g1->graph, 15, &head1);
    readHT(g1->graph, 16, &head2);
    addEdgePred(g1, 15, 16, head1->start, head2->start);
    readHT(g1->graph, 19, &head2);
    addEdgePred(g1, 15, 19, head1->start, head2->start);
    readHT(g1->graph, 18, &head1);
    addEdgePred(g1, 18, 19, head1->start, head2->start);
    readHT(g1->graph, 17, &head2);
    addEdgePred(g1, 18, 17, head1->start, head2->start);
    readHT(g1->graph, 16, &head1);
    addEdgePred(g1, 16, 17, head1->start, head2->start);
}

void generateGraph7(Graph **graph) {
    initGraph(graph, 6);
    Graph *g1 = (*graph);
    GraphListHead *head1, *head2;
    readHT(g1->graph, 0, &head1);
    addEdgePred(g1, 0, 1, head1->start, NULL);
    addEdgePred(g1, 0, 2, head1->start, NULL);
    addEdgePred(g1, 0, 3, head1->start, NULL);
    readHT(g1->graph, 1, &head1);
    addEdgePred(g1, 1, 4, head1->start, NULL);
    readHT(g1->graph, 2, &head1);
    readHT(g1->graph, 4, &head2);
    addEdgePred(g1, 2, 4, head1->start, head2->start);
    readHT(g1->graph, 3, &head1);
    addEdgePred(g1, 3, 5, head1->start, NULL);
}

void generateGraph8(Graph **graph) {
    initGraph(graph, 6);
    Graph *g1 = (*graph);
    GraphListHead *head1, *head2;
    readHT(g1->graph, 0, &head1);
    addEdgePred(g1, 0, 1, head1->start, NULL);
    addEdgePred(g1, 0, 3, head1->start, NULL);
    addEdgePred(g1, 0, 2, head1->start, NULL);
    readHT(g1->graph, 1, &head1);
    addEdgePred(g1, 1, 4, head1->start, NULL);
    readHT(g1->graph, 2, &head1);
    readHT(g1->graph, 4, &head2);
    addEdgePred(g1, 2, 4, head1->start, head2->start);
    readHT(g1->graph, 3, &head1);
    addEdgePred(g1, 3, 5, head1->start, NULL);
}

int main(int argc, char const *argv[])
{
    int i;
    //quanto cuidado ter para manter a descrição clockwise
    Graph *g1;
    Graph *g2;
    generateGraph8(&g1);
    generateGraph8(&g2);
    GraphListHead *head1;
    printGraph(g1);

    int n = g1->graph->entries;
    int *coloring = linearTimeColoring(g1);
    printf("Coloração de g1:\n");
    for (i = 0; i < n; i++) printf("Cor de %d: %d\n", i, coloring[i]);

    printf("Gottem. %d\n", checkColoring(g2, coloring));
    //printf("Gottem.\n");
    free(coloring);
    freeGraph(g1);
    freeGraph(g2);
    return 0;
}
