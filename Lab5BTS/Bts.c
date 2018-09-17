#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct Tree_Node Tree_Node; 
struct Tree_Node {
    char value; 
    Tree_Node* lChild;
    Tree_Node* rChild; 
};

Tree_Node* CreateNode(char data)
{
    Tree_Node* Ret = (struct Tree_Node*)malloc(sizeof(struct Tree_Node)*1); 
    Ret->value = data; 
    Ret->lChild = NULL; 
    Ret->rChild = NULL; 

    return Ret; 
}

void tree_insert(Tree_Node** root, char data)
{
    if (*root == NULL)                           //Empty Tree
    {
        Tree_Node* AddNode = CreateNode(data);
        *root = AddNode; 
    }
    else                                        //Not Empty
    {
        Tree_Node* curr = *root;
        if (curr->value < data)
        {
            tree_insert(&curr->rChild, data);
        }
        else
        {
            tree_insert(&curr->lChild, data);
        }
    }
}

Tree_Node* tree_search(Tree_Node* root, char data)
{ 
    Tree_Node* curr = root; 

    while (curr != NULL)
    {
        if (curr->value == data)
        {
            return curr;
        }
        else
        {
            if (curr->value < data)
            {
                curr = curr->rChild; 
            }
            else if (curr->value >= data)
            {
                curr = curr->lChild; 
            }
        }
    }

    return NULL; 
}

void tree_print_sorted ( Tree_Node* root)
{
    if (root != NULL)
    {
        tree_print_sorted(root->lChild);
        printf("%c ",root->value);
        tree_print_sorted(root->rChild); 
    }
}

void tree_delete (Tree_Node* root)
{
    if (root != NULL)
    {
        tree_delete(root->lChild); 
        tree_delete(root->rChild); 
        free(root); 
    }
}

char* ReadIn()
{
    char *word;
    scanf("%s", word);
    if (word[strlen(word) - 1] == '\n')
    {
        word[strlen(word) - 1] = '\0';
    }

    return word; 
}

int main (int agrc, char* argv[])
{
    char* Test = "FLOCCINAUCINHILIPILIFICATION";
    char* Alphabet = "ABCDEFGHIJKLMNOPQRSTUVQXYZ";
    int randomnumber; 
    srand(time(NULL));
    int stringSize = 30;
    Tree_Node *root = NULL;
    char search;
    Tree_Node *searchNode = NULL; 

    for (int i = 0; i < stringSize; i++)
    {
        tree_insert(&root, Test[i]); 
    }

    tree_print_sorted(root); 
    printf("\n"); 

    //printf("Please Search a Letter (0 to quit): "); 
    //search = ReadIn();
    //scanf(" %c", &search);
    
    for (int i = 0; i < 2; i++)
    {
        randomnumber = rand() %26;
        search = Alphabet[randomnumber]; 
        searchNode = tree_search(root, search);
        if (searchNode == NULL)
        {
            printf("The letter '%c' was not found\n", search);
        }
        else 
        {
            printf("The letter '%c' was found\n", search);
        }

        //printf("Please Search a Letter (0 to quit): "); 
        //scanf(" %c", &search);
    }

    tree_delete(root); 
    printf("\nTree deleted\n"); 
    return 0; 
} 