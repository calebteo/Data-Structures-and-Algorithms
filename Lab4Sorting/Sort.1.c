#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

static int NumOfSwaps = 0; 
static int NumOfComparisons = 0;

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
		if(feof(f) || buf[i]=='\n' || buf[i]=='\r') { end=1; break; } 
		// truncate fields that would overflow the buffer
		if( i<max-1 ) { ++i; } 
	}

	buf[i] = 0; // null terminate the string
	return end; // flag stating whether or not this is end of the line
}

void Swap(int *A, int *B)
{
    int temp;  
    temp = *B;
    *B = *A; 
    *A = temp; 
    NumOfSwaps++;
}


int Partition (int *array, int lowB, int highB)
{
    int temp; 
    int pivot = highB;
    int leftInt = lowB; 
    int rightInt = highB - 1;

    while (leftInt <= rightInt)
    {
        while(array[leftInt] <= array[pivot] && leftInt < highB)   //Check before submitting
        {
            leftInt++;
            NumOfComparisons++; 
        }
        while (array[rightInt] > array[pivot] && rightInt > -1)
        {
            rightInt--; 
            NumOfComparisons++;
        }  

        if (leftInt >= rightInt)
        {
            Swap(&array[rightInt + 1], &array[highB]); 
            pivot = rightInt + 1;
        }
        else 
        {
            Swap(&array[leftInt], &array[rightInt]); 
        }
    }
    
    return pivot; 
}


void QuickSort(int *array, int start, int end)
{
    int pivot; 
    if (start < end)
    {
        pivot = Partition(array, start, end);
        QuickSort(array, start, (pivot - 1));
        QuickSort(array, (pivot + 1), end); 
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

void PrintArray(int *a, int MAX)
{
    for (int i = 0; i < MAX; i++)
    {      
        printf("%d\n", a[i]);
    }
}

char* Create10000Array(int *a, int MAX, int type)
{
    srand(time(NULL));
    int j = 0; 
    switch(type)
    {
        case 0:                                         //Unqiue Random
        for (int i = 0; i < MAX; i++) 
        {
            a[i] = i; 
        }
        for (int i = 0; i < MAX; i++)
        {
            j = rand()%MAX;
            Swap(&a[i], &a[j]);
        }
        return "Unqiue Random";

        case 1:                                         //Random
        for (int i = 0; i < MAX; i++) { a[i] = rand()%(MAX); }
        return "Random";
        
        case 2:                                         //Ascending Order 
        for (int i = 0; i < MAX; i++) { a[i] = i; }
        return "Ascending Order"; 

        case 3:                                         //Decending Order
        for (int i = 0; i < MAX; i++) { a[i] = MAX - i; }
        return "Decending Order";

        case 4:                                         //Uniform Order
        for (int i = 0; i < MAX; i++) { a[i] = 5; }
        return "Uniform Order";
    }
    return "ERROR";
}

int main (int argc, char*argv[])
{   
    int ARRAY_SIZE; 
    int *array;
    int mode = 0; 
    int num = 0; 
    int indicator = 0;
    char *buffer;
    buffer = (char*)calloc(0, sizeof(char)*30); 
    FILE *example = fopen("example.csv", "r"); 
    printf("\n ****QUICKSORT****\nMode 1: User Enter Values\nMode 2: Example Values\nMode 3: Array of 10000\n\n");
    printf("Please Enter Mode 1 or 2 or 3:");
    scanf("%d", &mode); 

    switch(mode)
    {
        case 1: 
        ARRAY_SIZE = 10; 
        array = (int*)malloc(sizeof(int)*ARRAY_SIZE); 
        printf("Please enter 10 values: \n");
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            scanf("%d", &array[i]); 
        }

        printf("Sorting...\n");

        QuickSort(array, 0, ARRAY_SIZE - 1);
        PrintArray(array, ARRAY_SIZE); 
        PrintResults(); 
        break;
        
        case 2:
        ARRAY_SIZE = 10; 
        array = (int*)malloc(sizeof(int)*ARRAY_SIZE); 
        printf("\n");  
        while(feof(example) == 0)
        {
            indicator = next_field(example, buffer, 30);
            while(indicator != 1)
            {
                array[num] = atoi(buffer); 
                num++;
                indicator = next_field(example, buffer, 30);
            }
            QuickSort(array, 0, ARRAY_SIZE - 1);
            //PrintArray(array, ARRAY_SIZE);
            printf("Test type: %s\n", buffer); 
            PrintResults(); 
            num = 0; 
        }
        break;
        case 3:
        ARRAY_SIZE = 10000; 
        array = (int*)malloc(sizeof(int)*ARRAY_SIZE);
        printf("\n");  
        for (int i = 0; i < 5; i++)
        {
            buffer = Create10000Array(array, ARRAY_SIZE, i);
            //PrintArray(array, ARRAY_SIZE); 
            QuickSort(array, 0, ARRAY_SIZE - 1);
            printf("Test Type: %s\n", buffer); 
            PrintResults(); 
        }
        break;         
    }      
         
    return 0; 
}