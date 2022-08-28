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
    int nodes[MAX_NODES];
    int edges[MAX_EDGES];
} typedef G;

struct Edge
{
    int src;
    int dest; // src < dest always
} typedef E;

G *buildGraph(E edges[], int n)
{
    printf("size: %d\n", n);

    G *graph = (G *)malloc(sizeof(G));

    printf("malloced\n");

    int* degrees = calloc(MAX_NODES, sizeof(int));
    for (int i = 0; i < n; i++)
    {
        graph->nodes[edges[i].src] = 0;
        graph->edges[i] = 0;
    }

    printf("cleared lists...\n");

    // getting degrees
    for (int i = 0; i < n; i++)
    {
        degrees[edges[i].src] += 1;
    }

    //  for (int i = 0; i < MAX_NODES; i++)
    // {        
    //     printf("%d ",degrees[i]);
    // }

    int degSoFar = 0;
    for (int i = 0; i < MAX_NODES; i++)
    {        
        graph->nodes[i] = degSoFar;
        degSoFar += degrees[i];
    }

    // printf("<- degrees \n");
    // for (int i = 0; i < MAX_NODES; i++)
    // {        
    //     printf("%d ",graph->nodes[i]);
    // }
    // printf("<- offsets \n");
    free(degrees);
    printf(" set degrees .... \n");

    for(int i = 0; i < n; i++) {
        // degree of the current node
        //int deg = i+1 >= MAX_NODES ? MAX_EDGES - graph->nodes[i] :  graph->nodes[i+1] - graph->nodes[i];

        //printf("inserting %d,%d", edges[i].src, edges[i].dest);
        int j = graph->nodes[edges[i].src];
        int toPlace = edges[i].dest;
        while(graph->edges[j] != 0) {
            if(toPlace < graph->edges[j]) {
                int temp = graph->edges[j];
                graph->edges[j] = toPlace;
                toPlace = graph->edges[i];
            }
            j++;
        }
        graph->edges[j] = toPlace;
    }


    return graph;
}


// Function to print adjacency list representation of a graph
void printGraph(struct Graph *graph)
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        printf("NODE SRC: %d ", i);
        int offset = graph->nodes[i];
        // degree of the current node
        int deg = i+1 >= MAX_NODES ? MAX_EDGES - graph->nodes[i] :  graph->nodes[i+1] - graph->nodes[i];
        for (int j = offset; j < offset + deg; j++) {
            printf("(%d — %d)\t", i, graph->edges[j]);
        }
        printf("\n");
    }

    for (int i = 0; i < MAX_NODES; i++) {
        printf("%d ", graph->nodes[i]);
    }
    printf("\n");

    for (int i = 0; i < MAX_EDGES; i++) {
        printf("%d ", graph->edges[i]);
    }
}





int intersect(G* graph, int i, int j) {
    //printf("interseting node: %d and %d\n",i,j);
    int pointer1 = graph->nodes[i];
    int pointer2 = graph->nodes[j];
    //printf("  start: %d and %d\n",pointer1,pointer2);
    int max1 = i+1 >= MAX_NODES ? MAX_EDGES : graph->nodes[i+1];
    int max2 = j+1 >= MAX_NODES ? MAX_EDGES : graph->nodes[j+1];
    int count = 0;
    while(pointer1 < max1 && pointer2 < max2) {
        //printf("    comparing: %d and %d\n",graph->edges[pointer1], graph->edges[pointer2]);
        if(graph->edges[pointer1] < graph->edges[pointer2]) {
            pointer1++;
        } else if (graph->edges[pointer1] > graph->edges[pointer2]) {
            pointer2 ++;
        } else {
            //printf("    found\n");
            count++;
            pointer1 +=1;
            pointer2 += 1;
        }
    }
    return count;
}

int findTriangles(G* graph, int i) {
    int output = 0;
    int offset = graph->nodes[i];
    int deg = i+1 >= MAX_NODES ? MAX_EDGES - graph->nodes[i] :  graph->nodes[i+1] - graph->nodes[i];
    for(int j = offset; j < deg + offset; j++) {
        output += intersect(graph, i,graph->edges[j]);
    }
    return output;
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

    printf("scanned edges! \n");
    printf("outside n %d\n", n);

    struct Graph *graph = buildGraph(edges, n);
    printf("loaded graph\n");

    //printGraph(graph);
    

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
