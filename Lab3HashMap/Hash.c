#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


typedef struct Element Element;
struct Element{
    char surname[30];
    int personId; 
    char depId[20];
    char firstName[30]; 
    int Age; 
    char pType[20];
    char Gender [10];
    char Nationality[20]; 
    char Religion[20];
    char Occupation [30];
    
    Element* next; 
};

void PrintPerson(Element* Person)
{
    printf("%5d", Person->personId);
    printf("%20s", Person->depId); 
    printf("%15s", Person->surname);
    printf("%15s", Person->firstName); 
    if (Person->Age != -1) { printf("%8d", Person->Age); } 
    else {printf("%3s", "NA");}
    printf("%20s", Person->pType);
    printf("%10s", Person->Gender);
    printf("%20s", Person->Nationality);
    printf("%20s", Person->Religion);
    printf("%20s", Person->Occupation);
    printf("\n");  
}

void PrintHeader()
{
    printf("%s", "Person ID");
    printf("%15s", "Desp ID"); 
    printf("%15s", "Surname");
    printf("%15s", "First Name"); 
    printf("%10s", "Age");  
    printf("%20s", "Person Type");
    printf("%10s", "Gender");
    printf("%20s", "Nationality");
    printf("%20s", "Religion");
    printf("%20s", "Occupation");
    printf("\n");  
}

int PrintList(Element* head)
{
    int numofoccur = 0; 
    Element *curr; 
    curr = head;
    while(curr != NULL)
    {
        PrintPerson(curr);
        numofoccur++;
        curr = curr->next; 
    }
    return numofoccur;
}

void AddNext (Element *head, Element *toAdd)
{
    Element *curr; 
    curr = head; 
    while(curr->next != NULL)
    {
        curr = curr->next; 
    }
    curr->next = toAdd;  
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
    return (37*hash)%99991;
}

int hash2 (int currhash, int col, int max)
{
    int ret = currhash + (col*col);
    int diff = 0;
    int Maxint = pow(2,31) - 1;  
    if (ret < 0)
    {
        diff = abs(Maxint + ret);
        ret = 0 + diff;  
    }
    else if (ret > max)
    {
        diff = ret - max; 
        ret = 0 + diff; 
    }
    return ret; 
}

int AddToMap (Element *Map[], int sizeofmap, Element *Temp, int hash)
{
    int numberofcollisions = 0;
    int CompleteFlag = 0;
    while(CompleteFlag != 1)
    {
        if (hash >= sizeofmap - 1)                                              //Loop around array 
        {
            hash = 0; 
        }
        else if (strcmp(Map[hash]->surname, Temp->surname) == 0)                      //If same name - add to Linked List
        {
            AddNext(Map[hash], Temp);
            break; 
        }
        else if (strcmp(Map[hash]->surname,"") != 0)
        {
            numberofcollisions++;
            hash = hash2(hash, numberofcollisions, sizeofmap);
            //hash++; 
        }
        else 
        {
            Map[hash] = Temp; 
            CompleteFlag = 1;  
        }
    } 


    return numberofcollisions; 
}

void FindName(Element *Map[], int size, char* searchname)
{
    int searchHash;
    int numcoll = 0;
    int numfound = 0;  
    searchHash = hash(searchname);

    while (strcmp(Map[searchHash]->surname,"") != 0)
    {
        if (strcmp(Map[searchHash]->surname, searchname) == 0)
        {
            numfound = PrintList(Map[searchHash]);
            printf("%d people with the surname %s was found.\n", numfound, searchname);
            return; 
        }
        numcoll++;
        searchHash = hash2(searchHash, numcoll, size);
        
    }

    printf("The name '%s' was not found\n", searchname); 
    
}

void CreatePerson (Element *Person, char* field, int fieldindicator)
{ 
    int num;
    switch(fieldindicator)
    {
        case 0: 
            num = atoi(field); 
            Person->personId = num;
            break;
        case 1: 
            strcpy(Person->depId,field); 
            break;
        case 2: 
            strcpy(Person->surname,field); 
            break;

        case 3: 
            strcpy(Person->firstName,field); 
            break;
        case 4:
            num = atoi(field); 
            Person->Age = num;
            break;
        case 5: 
            strcpy(Person->pType,field); 
            break;

        case 6: 
            strcpy(Person->Gender,field); 
            break;
        case 7: 
            strcpy(Person->Nationality,field); 
            break;
        case 8: 
            strcpy(Person->Religion,field); 
            break;
        case 9: 
            strcpy(Person->Occupation,field); 
            break;        

    }

}

Element* CreateElement()
{
    Element *Temp;
    Temp = (Element*) malloc(sizeof(Element)); 
    strcpy(Temp->surname, "");
    //strcpy(Temp->depId, "");
    //strcpy(Temp->personId, ""); 
    //strcpy(Temp->firstName, "");
    //strcpy(Temp->pType, "");
    //Temp->Age = -1;    
    Temp->next = NULL; 

    return Temp;
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
    const int ARRAY_SIZE = 99991;
    Element *Map[ARRAY_SIZE];
    Element *Person = CreateElement(); 
    char *name;
    int MAX_SIZE = 30; 
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        Map[i] = CreateElement(); 
    }

    name = (char*)malloc(sizeof(char)* MAX_SIZE); 
    FILE *namescsv; 
    namescsv = fopen("people.csv","r"); 
    LoadCSV(namescsv); 

    int num = 0; 
    int nthfield = 0; 
    int totalcollisions = 0;

    while (num != 1)
    {
        num = next_field(namescsv, name, MAX_SIZE);
    }

    num = 0; 

    while(!feof(namescsv))
    {
        while(num != 1)
        {
            num = next_field(namescsv, name, MAX_SIZE);
            CreatePerson(Person, name, nthfield);
            nthfield++; 
        }
        nthfield = 0;
        num = 0; 

        totalcollisions += AddToMap(Map, ARRAY_SIZE, Person, hash(Person->surname));
        Person = CreateElement(); 
        //printf("%d\n", totalcollisions);  
    }

    
    //name = "Alcock";
    //totalcollisions += AddToMap(Map, 2000, name, hash(name));
    printf ("Collisions = %d\n", totalcollisions);
     
    //for (int i = 0; i < ARRAY_SIZE; i++)
    //{
        //printf("%d - %s - %s\n", i, Map[i]->surname, Map[i]->personId); 
    //    PrintPerson(Map[i]); 
    //}

    char *input; 
    input = (char*) malloc(sizeof(char)*MAX_SIZE);

    printf("Please enter a name to search: ");
    //scanf("%s30", input);
    fgets(input, MAX_SIZE, stdin); 
    input = Removenewline(input); 

    while (strcmp(input, "quit") != 0)
    {
        PrintHeader();
        FindName(Map, ARRAY_SIZE, input);
        printf("Please enter a name to search: ");
        //scanf("%s30", input);
        fgets(input, MAX_SIZE, stdin); 
        input = Removenewline(input);
    }
    
    free(name); 
    free(input);
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        free(Map[i]);
    }
    free(Person);
    fclose(namescsv);  

    return 0; 
} 