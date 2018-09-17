#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Element Element;
struct Element{
    char name[30];
    int count;
};

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

void LoadCSV(FILE *csv)
{
    if (csv) printf("CSV Loaded\n"); 
    else printf("Error in loading CSV\n"); 
}

int hash(char*s)
{
    int hash = 0;
    char *temp; 
    temp = s; 
    while(*temp)
    {
        hash = hash + *temp;
        temp++;
    }
    //printf("%d\n", hash); 
    return hash%104;
}

int AddToMap (Element Map[], int sizeofmap, char *name, int hash)
{
    int numberofcollisions = 0;
    int CompleteFlag = 0;
    while(CompleteFlag != 1)
    {
        if (hash >= sizeofmap - 1)
        {
            hash = 0; 
        }
        else if (strcmp(Map[hash].name, name) == 0)
        {
            Map[hash].count++;
            break; 
        }
        else if (strcmp(Map[hash].name,"") != 0)
        {
            numberofcollisions++;
            //hash = hash + (numberofcollisions*numberofcollisions);  
            hash++; 
        }
        else 
        {
            strcpy(Map[hash].name,name);
            Map[hash].count++;  
            CompleteFlag = 1;  
        }
    } 


    return numberofcollisions; 
}
void FindName(Element Map[], int size, char* searchname)
{
    int searchHash;
    searchHash = hash(searchname);

    while (strcmp(Map[searchHash].name,"") != 0)
    {
        if (strcmp(Map[searchHash].name, searchname) == 0)
        {
            printf("The name %s was found %d\n", searchname, Map[searchHash].count);
            return; 
        }
        searchHash++; 
    }

    printf("The name '%s' was not found\n", searchname); 
    
}

char* Removenewline(char* word)
{
    if (word[strlen(word) - 1] == '\n')
    {
        word[strlen(word) - 1] = '\0';
    }
    return word; 
}

int main (int agrc, char *agrv[])
{
    Element Map[104]; 
    char *name;
    int MAX_SIZE = 30; 
    for (int i = 0; i < 104; i++)
    {
        Map[i] = *(Element*)calloc(0,sizeof(Element)); 
    }

    name = (char*)malloc(sizeof(char)* MAX_SIZE); 
    FILE *namescsv; 
    namescsv = fopen("names.csv","r"); 
    LoadCSV(namescsv); 

    int num; 
    int totalcollisions = 0;

    while(!feof(namescsv))
    {
        num = next_field(namescsv, name, MAX_SIZE); 
        totalcollisions += AddToMap(Map, 104, name, hash(name));
        //printf("%d\n", totalcollisions);  
    }

    
    //name = "Alcock";
    //totalcollisions += AddToMap(Map, 2000, name, hash(name));
    printf ("Collisions = %d\n", totalcollisions);
     
    //for (int i = 0; i < 104; i++)
    //{
    //    printf("%d - %s\n", i, Map[i].name); 
    //}
    
    char *input; 
    input = (char*) malloc(sizeof(char)*MAX_SIZE);

    printf("Please enter a name to search: ");
    //scanf("%s30", input);
    fgets(input, MAX_SIZE, stdin); 
    input = Removenewline(input); 

    while (strcmp(input, "quit") != 0)
    {
        FindName(Map, 104, input);
        printf("Please enter a name to search: ");
        //scanf("%s30", input);
        fgets(input, MAX_SIZE, stdin); 
        input = Removenewline(input); 
    }
    
    free(name); 
    free(input); 

    return 0; 
} 