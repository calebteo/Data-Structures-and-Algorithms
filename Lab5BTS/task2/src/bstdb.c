#include "bstdb.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

// Write your submission in this file
//
// A main function and some profiling tools have already been set up to test
// your code in the task2.c file. All you need to do is fill out this file
// with an appropriate Binary Search Tree implementation.
//
// The input data will be books. A book is comprised of a title and a word
// count. You should store both these values in the tree along with a unique
// integer ID which you will generate.
//
// We are aiming for speed here. A BST based database should be orders of
// magnitude faster than a linked list implementation if the BST is written
// correctly.
//
// We have provided an example implementation of what a linked list based
// solution to this problem might look like in the db/listdb.c file. If you
// are struggling to understand the problem or what one of the functions
// below ought to do, consider looking at that file to see if it helps your
// understanding.
//
// There are 6 functions you need to look at. Each is provided with a comment
// which explains how it should behave. The functions are:
//
//  + bstdb_init
//  + bstdb_add
//  + bstdb_get_word_count
//  + bstdb_get_name
//  + bstdb_stat
//  + bstdb_quit
//
// Do not rename these functions or change their arguments/return types.
// Otherwise the profiler will not be able to find them. If you think you
// need more functionality than what is provided by these 6 functions, you
// may write additional functions in this file.

typedef struct book_node book_node;
struct book_node {
	char *title; 
	int wordCount; 
	int document_id; 
	book_node* lChild; 
	book_node* rChild;
};

book_node* book_db;
int Num_book; 
int numberofnodevisited; 
int averagenumofnodevisited;
int numofsearches;
int deepest; 
int nextDeepest; 
FILE *output; 
int NumofInsertion; 

int
bstdb_init ( void ) {
	// This function will run once (and only once) when the database first
	// starts. Use it to allocate any memory you want to use or initialize 
	// some globals if you need to. Function should return 1 if initialization
	// was successful and 0 if something went wrong.
	book_db = NULL; 
	Num_book = 0;
	numberofnodevisited = 0; 
	averagenumofnodevisited = 0;
	numofsearches = 0; 
	deepest = 0; 
	nextDeepest = 0; 
	output = fopen("output.txt", "w");
	NumofInsertion = 0; 

	return 1;
}

int GenerateBookId1 (char* title, int words)
{
	int Id = 0; 
	char *temp; 
	temp = title; 
	while(*temp)
	{
		Id = Id + *temp;
		temp++;
	}
	words = words%511;
	return 31*Id;
}

int GenerateBookId2 (char* title, int words)
{
	int Id = 0; 
	char *temp; 
	temp = title;

	char initials[12];
	int inicount = 0;
	initials[inicount] = *temp;
	inicount++;
	temp++;  
	while(*temp)
	{
		if (*temp == ' ')
		{
			initials[inicount] = *(++temp);			//use *(--temp) for last letter of word
			inicount++; 
			//temp = temp + 2;
			//initials[inicount] = *(temp);
			//inicount++; 
		}
		temp++;
	}
	for (int i = inicount; i > 0; i--)
	{
		Id = Id + initials[i];
	}
	Id = Id*(pow(10,4));
	Id = Id + words;
	return 7*Id;
}

book_node* CreateBook(char* title, int words)
{
	book_node* New = malloc(sizeof(book_node));
	size_t len_str = strlen(title) + 1; 
	New->title = calloc(len_str, sizeof(char)) ;
	strcpy(New->title, title);

	New->wordCount = words; 
	New->document_id = GenerateBookId2(title, words); 
	New->lChild = NULL; 
	New->rChild = NULL;

	Num_book++; 
	return New; 
}

/*book_node* TreetoArray(book_node *root, book_node* array, int i)
{
	book_node* add;

	if (!root)
	{
		return root;
	}
	array[i] = *add;
	i++; 
	if (root->lChild != NULL)
		add = TreetoArray(root->lChild, array,i); 
	
	if (root->rChild != NULL)
		add = TreetoArray(root->rChild, array, i);

	return root; 
}

void ReBalanceTree(book_node **root)
{
	book_node* array; 
	array = TreetoArray(book_db,array, 0);

}*/

int
bstdb_add ( char *name, int word_count ) {
	// This function should create a new node in the binary search tree, 
	// populate it with the name and word_count of the arguments and store
	// the result in the tree.
	//
	// This function should also generate and return an identifier that is
	// unique to this document. A user can find the stored data by passing
	// this ID to one of the two search functions below.
	//
	// How you generate this ID is up to you, but it must be an integer. Note
	// that this ID should also form the keys of the nodes in your BST, so
	// try to generate them in a way that will result in a balanced tree.
	//
	// If something goes wrong and the data cannot be stored, this function
	// should return -1. Otherwise it should return the ID of the new node
	book_node* curr = book_db;
	book_node* AddNode = CreateBook(name, word_count);
	if (curr == NULL)
	{
		book_db = AddNode; 
		return AddNode->document_id;
	}
	else 
	{
		book_node* prev; 
		while(curr != NULL)
		{
			prev = curr;
			if (curr->document_id <= AddNode->document_id)
			{
				if (curr->document_id == AddNode->document_id) 
				{ 
					AddNode->document_id++;
				}
				curr = curr->rChild;	

			}
			else
			{
				curr = curr->lChild; 
			}
		}
		
		if (prev->document_id < AddNode->document_id) { prev->rChild = AddNode; }
		else { prev->lChild = AddNode; }
		return AddNode->document_id; 
	}
	return -1;
}

int
bstdb_get_word_count ( int doc_id ) {
	// This is a search function. It should traverse the binary search tree
	// and return the word_count of the node with the corresponding doc_id.
	//
	// If the required node is not found, this function should return -1
	numofsearches++; 
	numberofnodevisited = 0; 
	book_node* curr = book_db;
	while (curr != NULL)
	{
		if (curr->document_id == doc_id)
		{
			averagenumofnodevisited += numberofnodevisited;
			return curr->wordCount;
		}
		else
		{
			if (curr->document_id < doc_id) { curr = curr->rChild; numberofnodevisited++; }
			else { curr = curr->lChild; numberofnodevisited++; }
		}	
	}
	return -1;
}

char*
bstdb_get_name ( int doc_id ) {
	// This is a search function. It should traverse the binary search tree
	// and return the name of the node with the corresponding doc_id.
	//
	// If the required node is not found, this function should return NULL or 0
	numofsearches++; 
	numberofnodevisited = 0; 
	book_node* curr = book_db;
	while (curr != NULL)
	{
		if (curr->document_id == doc_id)
		{
			averagenumofnodevisited += numberofnodevisited;
			return curr->title;
		}
		else
		{
			if (curr->document_id < doc_id) { curr = curr->rChild; numberofnodevisited++; }
			else { curr = curr->lChild; numberofnodevisited++; }
		}
		
	}
	return 0;
}

void CheckDeepest(int num)
{
	if (num > deepest)
	{
		deepest = num; 
	}
	else if (num < deepest && (num > nextDeepest))
	{
		nextDeepest = num; 
	}
}

int heighttree(book_node *root)
{
	if (root == NULL)
	{
		return 0; 
	}
	int ldepth = heighttree(root->lChild); 
	int rdepth = heighttree(root->rChild);

	CheckDeepest(ldepth);
	CheckDeepest(rdepth);

	if (ldepth > rdepth) 
	{
		return ldepth + 1; 
	}
	else
	{
		return rdepth + 1; 
	}

}

int BalancedTree (book_node* root)
{
	//1 = balanced, 0 = unbalanced
	if (root == NULL)
	{
		return 1; 
	}
	int ldepth = heighttree(root->lChild); 
	int rdepth = heighttree(root->rChild);

	if (abs(ldepth - rdepth) <= 1 && BalancedTree(root->lChild) && BalancedTree(root->rChild)) 
	{
		return 1; 
	}
	else
	{
		return 0; 
	}
}

void PrintBook(book_node* book)
{
	fprintf(output, "%40s", book->title);
	fprintf(output, "%12d\n", book->document_id); 

}

void PrintTree(book_node* root)
{
	if(root)
	{
		PrintTree(root->lChild);
		PrintBook(root); 
		PrintTree(root->rChild); 	
	}
}

void
bstdb_stat ( void ) {
	// Use this function to show off! It will be called once after the 
	// profiler ends. The profiler checks for execution time and simple errors,
	// but you should use this function to demonstrate your own innovation.
	//
	// Suggestions for things you might want to demonstrate are given below,
	// but in general what you choose to show here is up to you. This function
	// counts for marks so make sure it does something interesting or useful.
	//
	//  + Check if your tree is balanced and print the result
	//
	//  + Does the number of nodes in the tree match the number you expect
	//    based on the number of insertions you performed?
	//
	//  + How many nodes on average did you need to traverse in order to find
	//    a search result? 
	//
	//  + Can you prove that there are no accidental duplicate document IDs
	//    in the tree?
	printf("===============STATS===============\n"); 
	printf("TOTAL NUMBER OF NODES VISITED: %d\n", averagenumofnodevisited);
	printf("TOTAL NUMBER OF SEARCHES: %d\n", numofsearches);  
	averagenumofnodevisited = averagenumofnodevisited/numofsearches;
	printf("AVERAGE NUMBER OF NODES VISITED: %d\n\n", averagenumofnodevisited);

	int heightoftree = heighttree(book_db); 
	printf("HEIGHT OF TREE: %d\n", heightoftree); 
	printf("DEEPEST NODE: %d\n", deepest + 1); 
	printf("SECOND DEEPEST NODE: %d\n", nextDeepest);

	int Balanced = BalancedTree(book_db); 
	if (Balanced == 1)
	{
		printf("BALANCED TREE: Y\n");
	}
	else 
	{
		printf("BALANCED TREE: N\n");
	}

	PrintTree(book_db); 

}

void tree_delete (book_node* root)
{
    if (root != NULL)
    {
        tree_delete(root->lChild); 
        tree_delete(root->rChild); 
        free(root); 
    }
}

void
bstdb_quit ( void ) {
	// This function will run once (and only once) when the program ends. Use
	// it to free any memory you allocated in the course of operating the
	// database.
	tree_delete(book_db);
}
