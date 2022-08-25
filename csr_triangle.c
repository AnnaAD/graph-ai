#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NODES 10
#define MAX_EDGES 10

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

struct Graph
{
    int nodes[MAX_NODES];
    int edges[MAX_EDGES]
} typedef G;

struct Edge
{
    int src;
    int dest; // src < dest always
} typedef E;

G *buildGraph(E edges[], int n)
{
    G *graph = (G *)malloc(sizeof(G));

    for (int i = 0; i < n; i++)
    {
        graph->nodes[edges[i].src] = 0;
    }

    for (int i = 0; i < n; i++)
    {
        graph->nodes[edges[i].src] += 1;
        int offset = graph->nodes[edges[i].src+1] - graph->nodes[edges[i].src];
        graph->edges[offset] = edges[i].dest;
    }

    return graph;
}


// Function to print adjacency list representation of a graph
void printGraph(struct Graph *graph)
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        // print current vertex and all its neighbors
        int index = graph->nodes[i];
        printf("index: %d ", index);
        int deg = 0;
        if(i+1 >= MAX_NODES) {
            deg = MAX_EDGES - graph->nodes[i];
        } else {
            deg = graph->nodes[i+1] - graph->nodes[i];
        }
        for (int j = 0; j < deg; j++) {
            printf("(%d — %d)\t", i, graph->edges[j]);
        }
        printf("\n");
    }
}

// int seqTriangles(struct Graph* graph) {
//     int count = 0;
//     for (int i = 0; i < MAX_NODES; i++)
//     {
//         count += findTriangles(graph, i);
//     }
//     return count;
// }

// int parallelTriangles(struct Graph* graph) {
//     int count = 0;
//     int num_threads = 1;
//     #pragma omp parallel
//     {
//         num_threads = omp_get_num_threads();
//     }
//     printf("threads: %d \n",num_threads);
//     #pragma omp parallel for reduction(+ : count) //schedule(dynamic, 1)
//     for (int i = 0; i < MAX_NODES; i++)
//     {
//         count += findTriangles(graph, i);
//     }
//     return count;
// }

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

    printGraph(graph);
    

    // time_t start, end;
    // start = clock(); 
    // int count1 = seqTriangles(graph);
    // end = clock();
    // printf("SEQ TIME: %lf\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    // printf("COUNT = %d\n",
    //        count1);


    // start = clock(); 
    // int count = parallelTriangles(graph);
    // end = clock();
    // printf("PARALLEL TIME: %lf\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    // printf("COUNT = %d\n",
    //        count);

}
