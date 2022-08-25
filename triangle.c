#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NODES 11348900
#define MAX_EDGES 29876240

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

struct Graph
{
    struct Node *head[MAX_NODES];
} typedef G;

struct Node
{
    int index;
    struct Node *next; // points to nodes with index less
} typedef N;

struct Edge
{
    int src;
    int dest; // src < dest always
} typedef E;

G *buildGraph(E edges[], int n)
{
    G *graph = (G *)malloc(sizeof(G));
    for (int i = 0; i < MAX_NODES; i++)
    {
        graph->head[i] = NULL;
    }

    for (int i = 0; i < n; i++)
    {
        int src = edges[i].src;
        int dest = edges[i].dest;

        N *newNode = (N *)malloc(sizeof(N));
        newNode->index = dest;

        struct Node *ptr = graph->head[src];
        struct Node *last = NULL;
        while (ptr != NULL && dest > ptr->index)
        {
            last = ptr;
            ptr = ptr->next;
        }

        newNode->next = ptr;
        if (last == NULL)
        {
            graph->head[src] = newNode;
        }
        else
        {
            last->next = newNode;
        }
    }
    return graph;
}

int checkEdge(G* graph, int n1, int n2) {
    //assert n1 < n2
    N* ptr = graph->head[n1];
    while (ptr != NULL)
    {
        if (ptr->index == n2)
        {
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

int findTriangles(G* graph, int src)
{
    int triangles = 0;
    N *ptr = graph->head[src];
    while (ptr != NULL)
    {
        N *next = ptr->next;
        while(next != NULL) {
            if(checkEdge(graph, ptr->index,next->index)) {
                triangles += 1;
            }
            next = next->next;
        }
        ptr = ptr->next;
    }
    return triangles;
}


// Function to print adjacency list representation of a graph
void printGraph(struct Graph *graph)
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        // print current vertex and all its neighbors
        struct Node *ptr = graph->head[i];
        while (ptr != NULL)
        {
            printf("(%d — %d)\t", i, ptr->index);
            if (i > ptr->index)
            {
                printf("FAIL\n");
                return;
            }
            ptr = ptr->next;
        }
    }
}

int seqTriangles(struct Graph* graph) {
    int count = 0;
    for (int i = 0; i < MAX_NODES; i++)
    {
        count += findTriangles(graph, i);
    }
    return count;
}

int parallelTriangles(struct Graph* graph) {
    int count = 0;
    int num_threads = 1;
    #pragma omp parallel
    {
        num_threads = omp_get_num_threads();
    }
    printf("threads: %d \n",num_threads);
    #pragma omp parallel for reduction(+ : count) //schedule(dynamic, 1)
    for (int i = 0; i < MAX_NODES; i++)
    {
        count += findTriangles(graph, i);
    }
    return count;
}

int main(int argc, char *argv[])
{
    int nthreads, tid;

    struct Edge *edges = (struct Edge *)malloc(sizeof(struct Edge) * MAX_EDGES);

    FILE *in_file = fopen(argv[1], "r"); // read only
    printf("%s\n", argv[1]);

    if (!in_file) // equivalent to saying if ( in_file == NULL )
    {
        printf("oops, file can't be read\n");
        exit(-1);
    }

    int num1,num2;
    int n = 0;

    for(int i = 0; i < 4; i++) {
        fscanf(in_file, "%*[^\n]\n");
        printf("skip\n");
    }

    while (fscanf(in_file, "%d\t%d", &num1, &num2) == 2 && n < MAX_EDGES)
    {
        edges[n] = (struct Edge){MIN(num1, num2), MAX(num1, num2)};
        n += 1;
    }

    struct Graph *graph = buildGraph(edges, n);
    printf("loaded graph\n");
    

    time_t start, end;
    start = clock(); 
    int count1 = seqTriangles(graph);
    end = clock();
    printf("SEQ TIME: %lf\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    printf("COUNT = %d\n",
           count1);


    start = clock(); 
    int count = parallelTriangles(graph);
    end = clock();
    printf("PARALLEL TIME: %lf\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    printf("COUNT = %d\n",
           count);

}
