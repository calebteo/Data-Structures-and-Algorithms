/*                                                                             
 * ============================================================================
 *                                                                             
 *       Filename:  graph.c                                                    
 *                                                                             
 *    Description:  This file demonstrates an extremely naive, limited method
 *                  of implementing a graph structure in C. It does a few 
 *                  things well which you should pay attention to:
 * 
 *                  + Any dynamic memory allocation (malloc or calloc) is 
 *                    tested for failure immediately after the call is made
 *                  + It follows an object oriented programming style with
 *                    functions that perform operations on structs. It's not
 *                    OOP, but if you turn your head and squint, it could be
 *                  + It uses no globals
 *                  + It has a clear, distinct sequence of initialize, run and
 *                    shutdown stages
 *                  + During shutdown we release any and all dynamically
 *                    allocated memory (you can test for leaks using valgrind)
 *                  + Does a lot of error checking
 * 
 *                  It does a few things extremely badly which you should fix:
 *  
 *                  + Getting a vertex from the graph is slow
 *                  + Graph needs to know how big it is expected to get (needs
 *                    a max_vertices attribute). Shouldn't it grow naturally 
 *                    with the size of the input?
 *                  + Can't handle any sort of complex string labels
 *                  + Vertices can only store a (very) limited number of edges
 *                  + No method of weighting edges
 *                  + Following an edge to get to the next vertex requires a
 *                    call to graph_get_vertex. Couldn't edges just store
 *                    pointers to vertices instead?
 *                  + It has no functionality for removing/deleting edges
 *                  + Anything which fails will do so silently. Hard to track
 *                    down any errors
 *                                                                             
 *        Version:  1.0                                                        
 *        Created:  04/12/17                                          
 *       Revision:  none                                                       
 *       Compiler:  clang                                                      
 *                                                                             
 *         Author:  Gary Munnelly (gm), munnellg@tcd.ie                        
 *        Company:  Adapt Centre, Trinity College Dublin                       
 *                                                                             
 * ============================================================================
 */
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

// Max number of edges a node can have (shouldn't need this)
#define MAX_EDGES    8 

struct vertex {
    char id;
    
    // An array is very rigid. How do we make this better?
    // How do we handle the case where edges have weights?
    // Maybe it's time for another struct?
    int visited;
    struct edges *list; 
    int num_edges;
};

struct edges
{
    struct vertex* v;
    struct edges* next;
    char id;  
    //int weight;
};

struct graph {
    int max_vertices;        // Maximum size of the graph. Do we need a limit?
    int num_vertices;        // current size of the graph
    struct vertex *vertices; // pointer to all nodes in the graph
};

// Functions which manipulate vertices. 

struct vertex* vertex_new      ( char id );
int            vertex_init     ( struct vertex *v, char id );
int            vertex_add_edge ( struct vertex *v, struct vertex* v2, char id );
void           vertex_print    ( struct vertex *v );
void           vertex_free     ( struct vertex *v );

//Functions for edges. 
struct edges*  edges_new (struct vertex *v, char id);
int            edges_init( struct edges *e, struct vertex* v2, char id);
int            edge_add_edge ( struct edges **e, struct vertex *v, char id );
void           edges_free    ( struct edges *e );
void           edges_print   (struct edges *e);


// Functions which manipulate the graph

struct graph*  graph_new        ( int max_vertices );
int            graph_init       ( struct graph *g, int max_vertices );
struct vertex* graph_get_vertex ( struct graph *g, char id );
int            graph_add_vertex ( struct graph *g, char id );
int            graph_add_edge   ( struct graph *g, char id1, char id2 );
void           graph_print      ( struct graph *g );
void           graph_free       ( struct graph *g );

int            DFS           (struct graph* g, char start); 
int            stack_IsEmpty (int top);
struct vertex* stack_pop     (struct vertex* stack[], int *top);
int            stack_push    (struct vertex *stack[], struct vertex *new, int *top);

int            BFS           (struct graph *g, char start);
int            queue_IsEmpty (int startq, int endq);
struct vertex* queue_dequeue (struct vertex* queue[], int *startq);
int            queue_enqueue (struct vertex *queue[], struct vertex *new, int *endq);



int
main(int argc, char *argv[]) {

    struct graph *graph;
    
    // Initialization
    graph = graph_new( 7 );
    if (!graph) { return EXIT_FAILURE; }

    // Could load this from a file, but we'll hard code it for demo purposes
    for ( char c='A'; c<='F'; c++ ) {
        graph_add_vertex( graph, c );
    }

    graph_add_edge ( graph, 'A', 'E' );
    graph_add_edge ( graph, 'A', 'D' );
    graph_add_edge ( graph, 'A', 'B' );
    graph_add_edge ( graph, 'B', 'C' );
    graph_add_edge ( graph, 'B', 'D' );
    graph_add_edge ( graph, 'C', 'D' );
    graph_add_edge ( graph, 'C', 'B' );
    graph_add_edge ( graph, 'D', 'F' );
    graph_add_edge ( graph, 'E', 'D' );
    graph_add_edge ( graph, 'F', 'C' );

    // Do stuff
    graph_print(graph);
    printf("\n"); 
    DFS(graph, 'A'); 
    BFS(graph, 'A'); 
    printf("\n"); 
    // terminate
    graph_free(graph);

    return EXIT_SUCCESS;
}

struct vertex*
vertex_new ( char id ) {
    struct vertex *v;

    // I did not test malloc here because I know that vertex_init will do it
    // for me. If there is an issue with the allocation then vertex_init will
    // return 0
    v = malloc ( sizeof(struct vertex) );
    if(!vertex_init(v, id)) {
        // On failure, just to be safe, free whatever got allocated
        vertex_free(v);
        v = NULL;
    }
    return v;
}

int
vertex_init ( struct vertex *v, char id ) {
    // Check that we have a vertex before we do anything
    if ( v == NULL ) { return 0; }

    // Initialize everything
    v->id = id;
    v->num_edges = 0;
    v->list = NULL; 

    // return successful initialization
    return 1;
}

void
vertex_print ( struct vertex *v ) {
    // Check that we have a vertex before we do anything
    if (!v) { return; }

    // Print vertex label
    printf("%c", v->id);
   
    // Print vertex edges
    edges_print(v->list); 

    printf("\n");
}

void
vertex_free ( struct vertex *v ) {
    if (v) { free(v); }
}

int
vertex_add_edge ( struct vertex *v, struct vertex* v2, char id ) {
    if ( !v ) { return 0; }

    // Check to see that we haven't reached max capacity of edges
    if ( v->num_edges >= MAX_EDGES ) { return 0; }

    // If we have room, store the edge and update this vertex's edge count
    //v->edges[v->num_edges]->id = id;
    v->num_edges++;
    if (!edge_add_edge (&v->list, v2, id)){ return 0; }

    return 1;
}

struct edges* edges_new (struct vertex *v, char id) 
{
    struct edges *e;

    // I did not test malloc here because I know that edges_init will do it
    // for me. If there is an issue with the allocation then edges_init will
    // return 0
    e = malloc ( sizeof(struct edges) );
    if(!edges_init(e, v, id)) {
        // On failure, just to be safe, free whatever got allocated
        edges_free(e);
        e = NULL;
    }
    return e;
}


int edges_init( struct edges *e, struct vertex* v2, char id)
{
    if (!e) { return 0; }
    e->next = NULL;
    e->id = id;
    e->v = v2; 
    //e->weight = 0; 
    return 1; 
} 

int edge_add_edge ( struct edges **e, struct vertex *v, char id )
{
    if (!*e) 
    {
        struct edges* New = edges_new(v, id);
        *e = New;   
        return 1; 
    }
    
    struct edges* curr = *e;
    while (curr->next != NULL)
    {
         curr = curr->next;  
    }
    struct edges* New = edges_new(v, id);
    curr->next = New; 

    return 1;
}

void
edges_free ( struct edges *e ) {
    if (e) { free(e); }
}

void edges_print (struct edges *e)
{
    struct edges *curr = e; 
    while (curr != NULL)
    {
        printf(" -> %c", curr->id);
        curr = curr->next; 
    }
}

struct graph *
graph_new ( int max_vertices ) {
    struct graph *g;

    // I did not test malloc here because I know that graph_init will do it
    // for me. If there is an issue with the allocation then graph_init will
    // return 0
    g = malloc(sizeof(struct graph));
    if (!graph_init ( g, max_vertices )) {
        // On failure, just to be safe, free whatever got allocated
        graph_free(g);
        g = NULL;
    }

    // g will point to a new graph on success or NULL on failure
    return g;
}

int
graph_init ( struct graph *g, int max_vertices ) {
    // Good idea to make sure that g actually points to something
    if (!g) { return 0; };

    // Set everything up
    g->max_vertices = max_vertices;
    g->num_vertices = 0;
    // Try to malloc out some space for the vertices
    g->vertices = malloc(sizeof(struct vertex) * g->max_vertices);
    // if malloc fails then this function should report failure
    if(g->vertices == NULL) { return 0; }

    // return success
    return 1;
}

struct vertex *
graph_get_vertex ( struct graph *g, char id ) {
    // Good idea to make sure that g actually points to something
    if (!g) { return NULL; }

    // This linear search to find a vertex is horrible!
    // What datastructures can we use to make it faster?
    struct vertex *v = NULL;
    for ( int i=0; i<g->num_vertices; i++ ) {
        if ( g->vertices[i].id == id ) {
            v = &g->vertices[i];
            break;
        }
    }

    // v will point to the vertex if we found it. Else it will be NULL
    return v;
}

int
graph_add_vertex ( struct graph *g, char id ) {
    // First make sure that we've been given a graph to operate on
    if (!g) { return 0; }
    // Make sure the graph is not full
    if (g->num_vertices >= g->max_vertices) { return 0; }

    // Check to see if the vertex is already in the graph    
    struct vertex *v = graph_get_vertex(g, id);
    // If it is, report failure
    if (v) { return 0; }

    // Now try to add the vertex to the graph.    
    if (!vertex_init( &g->vertices[g->num_vertices], id )) { 
        return 0;
    }

    // If init was successful then increase the vertices count
    g->num_vertices++;

    // return success
    return 1;
}

int
graph_add_edge ( struct graph *g, char id1, char id2 ) {
    // First make sure that we've been given a graph to operate on
    if (!g) { return 0; }

    // Ensure both vertices are actually in the graph
    struct vertex *v1 = graph_get_vertex(g, id1);
    struct vertex *v2 = graph_get_vertex(g, id2);
    if (!v1 || !v2) { return 0; }

    // return the result of the vertex_add_edge function (1 if edge added, 0
    // if there was a problem)
    return vertex_add_edge( v1, v2, v2->id );;
}

void
graph_print ( struct graph *g ) {
    if (!g) { return; }

    // Iterate over all vertices in the graph and print them
    for ( int i=0; i<g->num_vertices; i++ ) {
        vertex_print( &g->vertices[i] );
    }
}

void
graph_free ( struct graph *g ) {
    // Do some checks and free whetever needs to be freed
    if (g) {
        if (g->vertices) { free(g->vertices); }
        free(g);
    }
}

int graph_mark_all_visited (struct graph **g)
{
    for (int i = 0; i < (*g)->max_vertices; i++)
    {
        (*g)->vertices[i].visited = 0; 
    }
    return 1;
}

int DFS (struct graph* g, char start)
{
    printf("DFS: "); 
    int MAX_STACK = 20; 
    struct vertex *stack[MAX_STACK];
    int top = -1; 
    struct vertex *curr;
    struct edges *curredge;

    graph_mark_all_visited(&g); 

    top++; 
    stack[top] = graph_get_vertex(g, start);
    stack[top]->visited = 1;

    while (stack_IsEmpty(top))
    {
        curr = stack_pop(stack, &top);
        printf("%c ", curr->id);
        curredge = curr->list; 
        while (curredge != NULL)
        {
            if (curredge->v->visited == 0)
            {
                curredge->v->visited = 1; 
                stack_push(stack, curredge->v, &top); 
            }
            curredge = curredge->next; 
        }   
    }
    printf("\n"); 
    return 1; 
}

int stack_IsEmpty(int top)
{
    //return 0 = empty OR 1 = not empty
    int empty = 0; 
/*    for (int i = 0; i < size; i++)
    {
        if (stack[i] != NULL)
        {
            empty = 1;
            break; 
        }
    }
*/
    if (top != -1) { empty = 1; }
    return empty; 
}

struct vertex* stack_pop(struct vertex* stack[], int *top)
{
    struct vertex* pop = stack[*top]; 
    stack[*top] = NULL; 
    (*top)--; 
    return pop; 

}

int stack_push (struct vertex *stack[], struct vertex *new, int *top)
{
    (*top)++; 
    stack[*top] = new; 
    return 1; 
}

int BFS (struct graph *g, char start)
{
    printf("BFS: "); 
    int MAX_QUEUE = 20; 
    struct vertex *Q[MAX_QUEUE];
    struct vertex *curr;
    struct edges *curredge; 
    int startq = 0;
    int endq = 0; 

    graph_mark_all_visited(&g); 

    queue_enqueue(Q, graph_get_vertex(g, start), &endq);  
    Q[startq]->visited = 1; 

    while(queue_IsEmpty(startq, endq))
    {
        curr = queue_dequeue(Q, &startq);
        printf("%c ", curr->id); 
        curredge = curr->list; 
        while (curredge != NULL)
        {
            if (curredge->v->visited == 0)
            {
                curredge->v->visited = 1; 
                queue_enqueue(Q, curredge->v, &endq); 
            }
            curredge = curredge->next; 
        } 
    }
    printf("\n"); 
    return 1;
}

int queue_IsEmpty (int startq, int endq)
{
    // 0 = empty, 1 = not empty
    int empty = 0; 
    if (startq != endq) { empty = 1; }
    return empty; 
}
struct vertex* queue_dequeue (struct vertex* queue[], int *startq)
{
    struct vertex *deq = queue[*startq];
    (*startq)++;
    return deq;  

}
int queue_enqueue (struct vertex *queue[], struct vertex *new, int *endq)
{
    queue[*endq] = new; 
    (*endq)++;
    return 1; 
}
