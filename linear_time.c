#include "stdio.h"
#include "malloc.h"

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
    
    free(nodo->equiv);
    free(nodo);
    return r;
}

int addEdgePred(Graph *graph, int one, int two, GraphEdgeNode *pred1, GraphEdgeNode *pred2) {    
    int r = 0;
    GraphEdgeNode *new1, *new2;
    GraphListHead *head;
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

    return r;
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
        if (edge->equiv->next == NULL) {
            readHT(graph->graph, edge->vertex, &head);
            edge = head->start;
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
 * Necessita de um vertexList com 7 espaços
 * 
 * Recua duas arestas para depois descrever os vértices, faz 6 acessos.
 * Devolve o tamanho da componente se <=6 ou 0 otherwise.
 * 
 * @param graph 
 * @param edge 
 * @return int* 
 */
int verticesDistanceAtMostTwoFacialWalk(Graph *graph, GraphEdgeNode *edge, int *vertexList) {
    int i;
    GraphEdgeNode *tmp = edge;
    GraphListHead *head;
    for (i = 0; i<7; i++) {
        (vertexList)[i] = -1;
    }
    readHT(graph->graph, tmp->equiv->vertex, &head);
    if (tmp->prev == NULL) tmp = head->end->equiv;
    else tmp = tmp->prev->equiv;
    readHT(graph->graph, tmp->equiv->vertex, &head);
    if (tmp->prev == NULL) tmp = head->end->equiv;
    else tmp = tmp->prev->equiv;
    (vertexList)[0] = tmp->equiv->vertex;
    for (i = 1; i < 7 && tmp->vertex != (vertexList)[0]; i++) {
        if (tmp->equiv->next == NULL) {
            readHT(graph->graph, tmp->vertex, &head);
            tmp = head->start;
        } else {
            tmp = tmp->equiv->next;
        }
        (vertexList)[i] = tmp->equiv->vertex;
    }
    return i <= 6 ? i : 0;
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
        return 0;       
    }
    if (checkDegree(graph, vertexList[0]) == 60) {
        readHT(graph->graph, x, &head);
        node = head->start;
        if (node->next != NULL && node->prev != NULL && node->next->vertex != vertexList[1] && node->next->vertex != vertexList[3] &&
            node->prev->vertex != vertexList[1] && node->prev->vertex != vertexList[3]) {
            for (; node != NULL; node = node->next) {
                if (checkDegree(graph, node->vertex) == 60) return 0;
                else if (checkAdjacent(graph, node->vertex, vertexList[0]) == 0) return 0; //min grau <= D: node->vertex <=> w (safety)
            }
        }
    } else {
        return checkDistanceAtMostTwo(graph, x, vertexList[0]) ? 0 : 4; //max grau <= D: x e v3 (safety)
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
        checkAdjacent(graph, x2, x3) || checkAdjacent(graph, x2, x4) || checkAdjacent(graph, x3, x4)) return 0;
    
    int flag;
    GraphListHead *head;
    GraphEdgeNode *node;
    readHT(graph->graph, vertexList[1], &head);
    for (node = head->start, flag = 1; node != NULL && flag; node = node->next) {
        if (node->vertex != vertexList[0] && node->vertex != vertexList[2] && 
            (flag = (checkDegree(graph, node->vertex) < 60)) && checkDistanceAtMostTwo(graph, node->vertex, x2)) return 0;
    }
    //se flag == 0, v5 tem C-forbidden.
    readHT(graph->graph, x2, &head);
    for (node = head->start; node != NULL && flag == 0; node = node->next) {
        if (node->vertex != vertexList[3] && (flag = (checkDegree(graph, node->vertex) < 60 - 1)) && 
            checkDistanceAtMostTwo(graph, node->vertex, vertexList[1])) return 0;
    }
    //se flag == -1, x2 tem C-forbidden.
    if (flag == -1) return 0;

    int y = -1;
    int *trash = malloc(sizeof(int) * 7);
    readHT(graph->graph, x3, &head);
    for (node = head->start, flag = 1; node != NULL && flag; node = node->next) {
        if (node->vertex != vertexList[4] && (flag = (checkDegree(graph, node->vertex) < 60)) && 
            checkDistanceAtMostTwo(graph, node->vertex, x4)) {
                if (checkAdjacent(graph, node->vertex, x4) && verticesDistanceAtMostTwoFacialWalk(graph, node, trash) == 5) 
                    y = node->vertex;
                else {
                    return 0;
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
                    return 0;
            }
    }
    //se flag == -1, x2 tem C-forbidden.
    if (y != -1) vertexList[5] = y;
    if (flag == -1) return 0;
    return 5; //tenho de devolver o y? para dar delete às arestas paralelas eventualmente
}

int checkHexagramSecurity(Graph *graph, int vertex, int *vertexList) { //v5 v6 v1 v2 v3 v4
    int r = 6;
    /*checkDegree(graph, vertex) != 3 ||*/
    if (checkDegree(graph, vertexList[1]) == 60 || checkDegree(graph, vertexList[5]) == 60) r = 0;
    else {
        int x = findThirdAdjacent(graph, vertex, vertexList[1], vertexList[3]);
        if (checkDistanceAtMostTwo(graph, x, vertexList[4])) r = 0;
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
    int r = 0, i;
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
            checkDegree(graph, vertexList[3]) != 3 || checkDegree(graph, vertexList[4]) != 3) return 0;

        int x1 = findThirdAdjacent(graph, vertexList[2], vertexList[1], vertexList[3]), 
            x3 = findThirdAdjacent(graph, vertexList[4], vertexList[0], vertexList[3]);

        if (x1 == x3 || (i = checkDegree(graph, vertexList[1])) == 60 || checkDegree(graph, x1) == 60 || 
            checkDegree(graph, x3) == 60 || checkAdjacent(graph, x1, x3)) return 0;
        
        if (i == 3) {
            return checkDistanceAtMostTwo(graph, x1, x3) ? 0 : 10;
        } else {
            return checkPentagramSecurity(graph, vertex, vertexList, x1, x3);
        }
    case 6: //v5 v6 v1 v2 v3 v4
        return checkHexagramSecurity(graph, vertex, vertexList);
    default:
        return 0;
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
    int r = 0;
    GraphEdgeNode *node;
    GraphListHead *head;
    readHT(graph->graph, vertex, &head);
    if (checkDegree(graph, vertex) == 3) {
        for (node = head->start; r == 0 && node != NULL; node = node->next) {
            if ((r = verticesDistanceAtMostTwoFacialWalk(graph, node, vertexList)) > 0) {
                r = checkMultigramSecurity(graph, vertex, vertexList, r); //o vértice pode não ser o primeiro...
            }
        }
    } else if (checkDegree(graph, vertex) == 2) r = 2;
    return r;
}

int main(int argc, char const *argv[])
{
    int i;
    //quanto cuidado ter para manter a descrição clockwise
    Graph *g1;
    printf("%d\n", initGraph(&g1, 10));   
    GraphListHead *head1, *head2;
    readHT(g1->graph, 0, &head1);
    //addEdgePred(g1, 0, 6, head1->start, NULL);
    addEdgePred(g1, 0, 1, head1->start, NULL);
    readHT(g1->graph, 6, &head1);
    readHT(g1->graph, 5, &head2);
    addEdgePred(g1, 6, 5, head1->start, head2->start);
    readHT(g1->graph, 4, &head1);
    addEdgePred(g1, 4, 5, head1->start, head2->start);
    readHT(g1->graph, 1, &head1);
    addEdgePred(g1, 1, 5, head1->start, head2->start->next);
    readHT(g1->graph, 4, &head1);
    readHT(g1->graph, 3, &head2);
    addEdgePred(g1, 4, 3, head1->start, head2->start);
    readHT(g1->graph, 2, &head1);
    addEdgePred(g1, 2, 3, head1->start, head2->start);
    readHT(g1->graph, 1, &head2);
    addEdgePred(g1, 2, 1, head1->start, head2->start->next);
    addEdgePred(g1, 7, 9, NULL, NULL);
    readHT(g1->graph, 7, &head2);
    addEdgePred(g1, 2, 7, head1->start, head2->start);
    readHT(g1->graph, 0, &head1);
    addEdgePred(g1, 0, 10, head1->start, NULL);
    readHT(g1->graph, 10, &head1);
    addEdgePred(g1, 10, 7, head1->start, head2->start);
    readHT(g1->graph, 4, &head1);
    readHT(g1->graph, 8, &head2);
    addEdgePred(g1, 4, 8, head1->start, head2->start);
    readHT(g1->graph, 9, &head1);
    addEdgePred(g1, 9, 8, head1->start, head2->start);
    readHT(g1->graph, 3, &head2);
    addEdgePred(g1, 9, 3, head1->start, head2->start);
    //(g1->graph, 6, &head1);
    //addEdgePred(g1, 6, 8, head1->start, head2->start);
    printGraph(g1);
    //removeEdge(g1, g1->array[2].start->next);
    //printGraph(g1);
    //printf("Aresta final do vértice 2: %d\n", g1->graph->tbl[hash(g1->graph, 2)].value->start->next->vertex);
    printf("Aresta final do vértice 2: %d\n", g1->graph->tbl[hash(g1->graph, 2)].value->end->vertex);
    printf("Adjacente: %d\n", checkAdjacent(g1, 0, 2));
    printf("Distância no max 2: %d, %d\n",checkDistanceAtMostTwo(g1, 4, 0), checkDistanceAtMostTwo(g1, 2, 0));

    readHT(g1->graph, 0, &head1);
    GraphEdgeNode **faceWalk = edgesInFacialWalk(g1, 0, head1->end, 4);
    printf("Arestas na face contida incidente em 0 (espera-se 1 2 3 0):");
    for (i = 0; i<4; i++) {
        if (faceWalk[i] != NULL)
            printf(" %d", faceWalk[i]->vertex);
    }
    putchar('\n');
    free(faceWalk);

    int *vertexList = malloc(sizeof(int)*7);
    printf("Será que a componente tem tamanho at most 6? %d\n", 
        verticesDistanceAtMostTwoFacialWalk(g1, g1->graph->tbl[hash(g1->graph, 0)].value->end, vertexList));
    printf("Vértices a 2 de distância (espera-se 5 6 0 1):");
    for (int i = 0; i<7; i++) {
        printf(" %d", vertexList[i]);
    }
    putchar('\n');

    //int list[] = {0,2};
    //Graph *subg1 = subgraphFromPath(g1, list, 2);
    //printGraph(subg1);
    //freeGraph(subg1);

    printf("Got %d: ", findSecureMultigramFromVertex(g1, 5, vertexList));
    for (i = 0; i<7; i++) printf(" %d", vertexList[i]);
    putchar('\n');

    printf("Got %d: ", findSecureMultigramFromVertex(g1, 4, vertexList));
    for (i = 0; i<7; i++) printf(" %d", vertexList[i]);
    putchar('\n');

    printf("Got %d: ", findSecureMultigramFromVertex(g1, 3, vertexList));
    for (i = 0; i<7; i++) printf(" %d", vertexList[i]);
    putchar('\n');
    //printf("Got %d\n", findSecureMultigramFromVertex(g1, 1, vertexList));
    //printf("Got %d\n", findSecureMultigramFromVertex(g1, 2, vertexList));
    //printf("Got %d\n", findSecureMultigramFromVertex(g1, 3, vertexList));
    free(vertexList);
    
    printf("Gottem.\n");

    return 0;
}
