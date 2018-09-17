//==== NOTE ==================================================================
// You do not need to change anything in this file, but feel free to read it
// if it is of interest.
//
// You only need to update bstdb.c
//============================================================================

#include "listdb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==== STRUCT ================================================================
// Simple implementation of a singly linked list which will store documents
// in our database.
//============================================================================
struct list_node {
    int   doc_id;           // unique identifier for the document
    char *name;             // file name of the document    
    int   word_count;       // number of words in the document      
    struct list_node *next; // pointer to the next node in the list
};

int               g_next_id;     // ID of the next document to be added
struct list_node *g_linked_list; // database storage

int g_num_comps;
int g_num_searches;

static void
free_list_node ( struct list_node *node ) {
    struct list_node *p;
    p = node;
    while (p) {
        p = node->next;
        if (node->name) free(node->name);
        free(node);
        node = p;
    }
}

static struct list_node *
find_document ( int doc_id ) {
    struct list_node *p = g_linked_list;
    
    // Count how many times this function was called so we can get 
    // average number of nodes traversed for each query
    g_num_searches++;
    
    while (p) {
        // count the number of traversals
        g_num_comps++;
        // If we found the node, return it. Otherwise keep searching.
        if (p->doc_id == doc_id) { break; }
        p = p->next;
    }

    return p;
}

static int
len_list ( struct list_node *ln ) {
    int count = 0;
    while (ln) {
        count++;
        ln = ln->next;
    }
    return count;
}

static struct list_node*
create_list_node ( int doc_id, char *name, int word_count, 
        struct list_node *next ) {
    struct list_node *ln;
    size_t len_name;

    // Allocate memory for the new list node
    ln = malloc( sizeof(struct list_node) );
    
    // malloc can fail, so make sure ln is pointing to something before we try
    // writing to it.
    if (ln) {       
        // Store data in the list node
        ln->doc_id     = doc_id;
        ln->word_count = word_count;
        ln->next       = next;

        // Allocate memory to store the file name and copy the string into the
        // new list node
        len_name = strlen(name)+1;
        ln->name = calloc(len_name, sizeof(char));
        if (ln->name) {
            // if calloc was successful, copy the filename into the node
            strcpy( ln->name, name );
        } else {
            // if calloc failed, release any memory that was allocated and 
            // report an error by returning NULL
            ln->next = NULL;
            free_list_node(ln);
            ln = NULL;
        }
    }

    return ln;
}

int
listdb_init ( void ) {
    g_linked_list = NULL;
    g_next_id = 0;
    g_num_comps = 0;
    g_num_searches = 0;
    return 1;
}

int
listdb_add ( char *name, int word_count ) {
    struct list_node *tmp;
    
    // Create a new list node and push it to the start of the linked list
    // If something goes wrong this function should return a negative number
    // in order to report the error 
    tmp = create_list_node(g_next_id, name, word_count, g_linked_list);
    if(!tmp) { return -1; }
    g_linked_list = tmp;

    // Use g_next_id as the id of the new document. Results in incrementing
    // integer document ID assignment
    return g_next_id++; 
}

int
listdb_get_word_count ( int doc_id ) {
    // search list of document and return its length - slow...
    struct list_node *p = find_document(doc_id);
    return (p)? p->word_count : -1;
}

char*
listdb_get_name ( int doc_id ) {
    // search list of document and return its name - slow...
    struct list_node *p = find_document(doc_id);
    return (p)? p->name : NULL;
}

void
listdb_stat ( void ) {
    printf("STAT\n");
    printf("Avg comparisons per search  -> %lf\n",
        (double)g_num_comps/g_num_searches);
    printf("List size matches expected? -> %c\n",
        ((g_next_id == len_list(g_linked_list))? 'Y' : 'N') );
}

void
listdb_quit ( void ) {  
    free_list_node(g_linked_list);
}