#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


static int NumOfSwaps = 0; 
static int NumOfComparisons = 0;

typedef struct Game Game;
struct Game {
    char name[60]; 
    char platform[20]; 
    int score; 
    char releaseYr[5]; 
};

Game* IntialiseGame ()
{   
    Game *Ret;
    Ret = malloc(sizeof(Game));
    strcpy(Ret->name, ""); 
    strcpy(Ret->platform, "");
    strcpy(Ret->releaseYr, "");
    Ret->score = 0; 

    return Ret; 
}
void CreateGame (Game *g, char *field, int i)
{
    switch (i)
    {
        case 0:
            strcpy(g->name,field);
            break; 
        case 1: 
            strcpy(g->platform,field);
            break;
        case 2: 
            g->score = atoi(field); 
            break; 
        case 3: 
            strcpy(g->releaseYr,field); 
            break;
    }
}

int next_field( FILE *f, char *buf, int max ) 
{
	int i=0, end=0, quoted=0;
	
    for(;;) 
    {
		// fetch the next character from file		
		buf[i] = fgetc(f);
		// if we encounter quotes then flip our state and immediately fetch next char
		if(buf[i]=='"') { quoted=!quoted; buf[i] = fgetc(f); }
		// end of field on comma if we're not inside quotes
		if(buf[i]==',' && !quoted) { break; }
		// end record on newline or end of file
        if(feof(f) || buf[i]=='\n' || buf[i]=='\r') 
        { 
            if (buf[i] == '\r') { fgetc(f); }
            end=1; break;
        } 
		// truncate fields that would overflow the buffer
		if( i<max-1 ) { ++i; } 
	}

	buf[i] = 0; // null terminate the string
	return end; // flag stating whether or not this is end of the line
}

void Swap(Game *A, Game *B)
{
    Game temp;  
    temp = *B;
    *B = *A; 
    *A = temp; 
    NumOfSwaps++;
}


int Partition (Game *array[], int lowB, int highB)
{
    int temp; 
    int pivot = highB;
    int leftInt = lowB; 
    int rightInt = highB - 1;

    while (leftInt <= rightInt)
    {
        while(array[leftInt]->score <= array[pivot]->score && leftInt < highB)   //Check before submitting
        {
            leftInt++;
            NumOfComparisons++; 
        }
        while (array[rightInt]->score > array[pivot]->score && rightInt > -1)
        {
            rightInt--; 
            NumOfComparisons++;
        }  

        if (leftInt >= rightInt)
        {
            Swap(array[rightInt + 1], array[highB]); 
            pivot = rightInt + 1;
        }
        else 
        {
            Swap(array[leftInt], array[rightInt]); 
        }
    }
    
    return pivot; 
}


void QuickSort(Game *array[], int start, int end)
{
    int pivot; 
    if (start < end)
    {
        pivot = Partition(array, start, end);
        QuickSort(array, start, (pivot - 1));
        QuickSort(array, (pivot + 1), end); 
    }
}

void SelectionSort(Game *array[], int start, int end)
{
    int min;
    for (int i = start; i < end - 1; i++)
    {
        min = i + 1; 
        for (int j = i + 1; j < end; j++)
        {
            if (array[min]->score > array[j]->score)
            {
                min = j;
            }
            NumOfComparisons++;
        }
        if (array[i]->score > array[min]->score)
        {
            Swap(array[i], array[min]); 
        }
        
    }
}

void ReadInValues()
{

}

void PrintResults()
{
    printf("Number of Comparisons: %d\n", NumOfComparisons); 
    printf("Number of Swaps: %d\n\n", NumOfSwaps);

    NumOfComparisons = 0; 
    NumOfSwaps = 0; 
}

void PrintArray (Game *a[], int MAX)
{
    for (int i = 0; i < MAX; i++)
    {      
        printf("%60s", a[i]->name);
        printf("%25s", a[i]->platform); 
        printf("%5d", a[i]->score);
        printf("%10s\n", a[i]->releaseYr); 
    }
}

void PrintTopN(Game *a[], int MAX, int N)
{
    for (int i = MAX; i > (MAX - N); i--)
    {
        printf("%60s", a[i]->name);
        printf("%25s", a[i]->platform); 
        printf("%5d", a[i]->score);
        printf("%10s\n", a[i]->releaseYr); 
    }
}

void Create10000Array(int *a, int MAX, int type)
{
    srand(time(NULL));
    int j = 0; 
    switch(type)
    {
        case 0:                                         //Unqiue Random and Random
        for (int i = 0; i < MAX; i++) { a[i] = rand()%(2*MAX); }
        break;

        case 1:                                         //Random
        for (int i = 0; i < MAX; i++) { a[i] = rand()%(MAX); }
        break;
        
        case 2:                                         //Ascending Order 
        for (int i = 0; i < MAX; i++) { a[i] = i; }
        break; 

        case 3:                                         //Decending Order
        for (int i = MAX - 1; i < 0; i--) { a[j] = i; j++; }
        break;

        case 4:                                         //Uniform Order
        for (int i = 0; i < MAX; i++) { a[i] = 5; }
        break;
    }
}

int main (int argc, char*argv[])
{   
    int ARRAY_SIZE = 18625; 
    Game *array[ARRAY_SIZE];
    //array = malloc(sizeof(Game)*ARRAY_SIZE); 

    for (int i = 0; i < ARRAY_SIZE; i++) 
    { 
        array[i] = IntialiseGame(); 
    } 

    int arraynum = 0; 
    int indicator = 0;
    int fieldcount = 0; 
    char buffer[60];
    //buffer = calloc(0, sizeof(char)*60); 
    FILE *example = fopen("ign.csv", "r"); 

    while (indicator != 1)
    {
        indicator = next_field(example, buffer, 60);
    }

    while(feof(example) == 0 && arraynum < ARRAY_SIZE)
    {
        indicator = next_field(example, buffer, 60);
        while(indicator != 1)
        {
            CreateGame(array[arraynum], buffer, fieldcount);
            fieldcount++;   
            indicator = next_field(example, buffer, 60);
        }
        CreateGame(array[arraynum], buffer, fieldcount);            //for last field
        fieldcount = 0;
        arraynum++;
    }

    QuickSort(array, 0, ARRAY_SIZE - 1);
    //SelectionSort(array, 0, ARRAY_SIZE); 
    //PrintArray(array, ARRAY_SIZE);
    PrintTopN(array, ARRAY_SIZE - 1, 10); 
    PrintResults();

    //free(buffer); 
    //for (int i = 0; i < ARRAY_SIZE; i++)
    //{
        //free(array[i]); 
    //}

    fclose(example);

    return 0; 
}