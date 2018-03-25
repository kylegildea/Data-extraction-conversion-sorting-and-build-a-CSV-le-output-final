/*------------------------------------------------------------------
// University of Central Florida
// COP3502C - Fall 2016
// Program Author: Kyle Gildea

“The program is entirely my own work and I have neither developed my code together with any another person, nor copied program code from any
other person, nor permitted my code to be copied or otherwise used by any other person, nor have I
copied, modified, or otherwise used program code that you have found in any external source, including but
not limited to, online sources”.

//------------------------------------------------------------------*/

#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define BUFFERSIZE 500

typedef struct airPdata
{
  int seqNumber; //The process output’s sequence number
  char *LocID; //Airport’s ‘‘Short Name’’, ie MCO
  char *fieldName; //Airport Name
  char *city; //Associated City
  float longitude; //Longitude
  float latitude; //Latitude
} airPdata;

typedef struct lListAirPdata
{
  airPdata *curAirPdataP; //Pointer to the Airport Data
  struct lListAirPdata *nextAirPdataList; // pointer to next
} lListAirPdata;

typedef struct lListAirPdata LListAirPdata; 
typedef LListAirPdata *LListAirPdataPtr;

typedef struct treeNode
{
  airPdata *curAirPdataP; //Pointer to the Airport Data
  struct treeNode *leftPtr;
  struct treeNode *rightPtr; 
} TreeAirPdata;

typedef struct treeNode TreeNode; // synonym for struct treeNode
typedef TreeNode *TreeNodePtr; // synonym for TreeNode*


//Function declarations
void errorPrint(char *error);
void print(airPdata *airP);
void freeLineBuffer(char *line);
void freeAirport(airPdata *airP);
lListAirPdata *readFile(char *fileName);
void storeLocID(airPdata *airP, char *input);
void storeFieldName(airPdata *airP, char *input);
void storeCity(airPdata *airP, char *input);
void storeLatitude(airPdata *airP, char *input);
void storeLongitude(airPdata *airP, char *input);
float sexag2decimal(char *degreeString);
int isEmpty(lListAirPdata *headPtr) ;
void addToList(LListAirPdataPtr *headPtr, airPdata *airport);
void sortByLocID(lListAirPdata *airports);
int validAirport(char *token);
void sortByLatitude(lListAirPdata *airports);
void insertNode(TreeNodePtr *treePtr, airPdata *value);
void inOrder(TreeNodePtr treePtr, int *seq);
int parameterCheck(char *param);

//Main Function
int main(int argc, char **argv) 
{
  lListAirPdata *head;
  lListAirPdata *alphaSort;
  TreeNodePtr rootPtr = NULL;
  
  if(argc < 3 || argc > 3)
  {
    errorPrint("ERROR: Invalid Parameters Given!\n");
    return -1;
  }
  
  else if(!(parameterCheck(argv[2])))
  {
    errorPrint("ERROR: Invalid Parameters Given!\n");
    return -1;
  }
  
  
  
  if(argv[2][0] == 'a' || argv[2][0] == 'A')
  {
    head = readFile(argv[1]); 
    sortByLocID(head);
  }
  
  else if(argv[2][0] == 'n' || argv[2][0] == 'N')
  {
    head = readFile(argv[1]); 
    sortByLatitude(head);
  }
  
  
  
  /*while(head->nextAirPdataList != NULL)
  {
    print(head->curAirPdataP);
    head = head->nextAirPdataList;
  }
  print(head->curAirPdataP);*/
  return 0;
}

//Takes argv[2] as a string, checks to see if it's a valid parameter. Called from main for error checking
int parameterCheck(char *param)
{
  if(param[0] == 'n' || param[0] == 'N' || param[0] == 'a' || param[0] == 'A')
    return 1;
  else
    return 0;
}


//Prints error string
void errorPrint(char *error) 
{ 
  fprintf(stderr, "%s", error);
}

//Builds a binary search tree sorted by latitude of airport
void sortByLatitude(lListAirPdata *airports)
{
  TreeNodePtr root = NULL;
  int seq = 1; 
   while(airports != NULL)
   {
     insertNode(&root, airports->curAirPdataP);
     airports = airports->nextAirPdataList;
   }
   
   inOrder(root, &seq);
}

//Inserts airPdata into search tree recursively by calling over and over again until it finds node pointing to Null
void insertNode(TreeNodePtr *treePtr, airPdata *value)
{ 
   // if tree is empty
   if (*treePtr == NULL) 
   {   
      *treePtr = malloc(sizeof(TreeNode));

      // if memory was allocated, then assign data
      if (*treePtr != NULL) 
      { 
         (*treePtr)->curAirPdataP = value;
         (*treePtr)->leftPtr = NULL;
         (*treePtr)->rightPtr = NULL;
      } 
      else 
      {
         printf("Not inserted. No memory available.\n");
      } 
   } 
   else 
   { // tree is not empty
      // data to insert is less than data in current node
      if (value->latitude < (*treePtr)->curAirPdataP->latitude) 
      {                   
         insertNode(&((*treePtr)->leftPtr), value);   
      }                                         

      // data to insert is greater than data in current node
      else if (value->latitude > (*treePtr)->curAirPdataP->latitude) 
      {                 
         insertNode(&((*treePtr)->rightPtr), value);     
      }                                        
   }
} 

// prints inorder traversal of tree. also has int pointer to counter to update sequence number
void inOrder(TreeNodePtr treePtr, int *seq)
{ 
   // if tree is not empty, then traverse
   if (treePtr != NULL) 
   {                
      inOrder(treePtr->leftPtr, seq);
      treePtr->curAirPdataP->seqNumber = *seq;
      (*seq)++;         
      print(treePtr->curAirPdataP);      
      inOrder(treePtr->rightPtr, seq);        
   }                           
}

//sorts by location ID, saving at most 1 per letter. Goes thru list and compares 2 strings using strcmp. If LocID of current airport comes before one saved in array for that letter it replaces it
//Prints Once the list has been fully exhausted, skips letters that aren't used
void sortByLocID(lListAirPdata *airports)
{
  airPdata *sort[26] = {NULL};
  int index;
  
  while(airports !=NULL)
  {
    index = (int)(airports->curAirPdataP->LocID[0]) - 65;
    if(sort[index] == NULL || strcmp(airports->curAirPdataP->LocID, sort[index]->LocID) < 0)
    {
      sort[index] = airports->curAirPdataP;
    }
    airports = airports->nextAirPdataList;
  }
  
  index = 1;
  
  for(int i =0; i <26; i++)
  {
    if(sort[i] != NULL)
    {
      sort[i]->seqNumber = index;
      print(sort[i]);
      index++;
    }
  }
}


/*Reads in file with name given by command line
If the file cannot be opened, prints an error
Otherwise opens file, prints column names and a divider
Then reads file line by line, using fgets
Once a line has been read, strtok() is used to
tokenize the first fields from the string, then
goes into a while loop, tokenizing the rest of the line.
The first 7 fields are always in the same location, and so a counter.
fieldRead is used to determine if the token acquired is one we're
looking for. If it is, switch statement is used to store the value
in a struct. The final field controlTower is not always in the same place but is
always the 13th field or later. so once the first 7 values have been found, 
the counter continues to go up until 13, but any text parsed is discarded.
Upon ariving at the 13th field, the counter stops and text is processed until
a "Y" or "N" is found. Once the struct is filled out, the print function
is called to print the line and the process repeats until EOF.
*/

int validAirport(char *token)
{
  return (strlen(token) == 3 && (isupper(token[0]) && isupper(token[1]) && isupper(token[2])));
}

lListAirPdata *readFile(char *fileName)
{
  FILE *inputFile = fopen(fileName, "r");
  airPdata *airport;
  LListAirPdataPtr headPtr = NULL;
  LListAirPdataPtr temp;
  LListAirPdataPtr newPtr;
  char *comma = ",";
  char *line;
  char *token;
  char *error;
  char *dashPtr;
  int counter = 1;
  int fieldRead = 0;
  
  if (inputFile == NULL)
  {
    error = malloc(BUFFERSIZE * sizeof(char));
    strcpy(error, "ERROR: could not open file ");
    strcat(error, fileName);
    strcat(error, "\n\n");
    errorPrint(error);
    free(error);
  }
  else
  {
    
    printf("code,name,city,lat,lon\n"); 
      
   /*airport = malloc(sizeof(airPdata));
   if(airport == NULL)
   {
     errorPrint("Airport Struct Malloc Failed!\n");   
   }*/
   
   line = malloc(BUFFERSIZE * sizeof(char));
   
   while(fgets(line, BUFFERSIZE, inputFile) != NULL)
   {
     if(isEmpty(headPtr))
     {
       token = strtok(line,comma);
       if(strlen(token) > 3)              
         token = strtok(NULL, comma);
       if(validAirport(token))
       {
         while(token != NULL)  
         {
           
           if(fieldRead == 0)
           {
             headPtr = malloc(sizeof(LListAirPdataPtr));
             headPtr->nextAirPdataList = NULL;
             headPtr->curAirPdataP = malloc(sizeof(airport));
             headPtr->curAirPdataP->seqNumber = counter;
             storeLocID(headPtr->curAirPdataP, token);
             fieldRead++;
           }
               
           else if(fieldRead == 1)
           {
             storeFieldName(headPtr->curAirPdataP, token);
             fieldRead++;      
           }      
         
           else if(fieldRead == 2)
           { 
             storeCity(headPtr->curAirPdataP, token);
             fieldRead++;
           }               
           
           else if((fieldRead == 3 && strchr(dashPtr, '-')!=NULL) || (fieldRead == 3 && strtof(token,NULL) != 0.0F))
           {
               storeLatitude(headPtr->curAirPdataP, token);
               fieldRead++;          
           }
           else if(fieldRead == 4 && strchr(dashPtr, '-')!=NULL || (fieldRead == 4 && strtof(token,NULL) != 0.0F))
           {
               storeLongitude(headPtr->curAirPdataP, token);
               fieldRead++;
               counter++;
           }     
           token = strtok(NULL, comma);
         }
       }
     }
     
     else
     {
       
       token = strtok(line,comma);
       if(strlen(token) > 3)       
         token = strtok(NULL, comma);
       
       if(validAirport(token))
       {       
         temp = headPtr;
         while(temp->nextAirPdataList !=NULL)
         {
           temp = temp->nextAirPdataList;
         }
         
         while(token != NULL)  
         {
           if(fieldRead == 0)
           {
             newPtr = malloc(sizeof(LListAirPdataPtr));
             newPtr->nextAirPdataList = NULL;
             newPtr->curAirPdataP = malloc(sizeof(airport));
             newPtr->curAirPdataP->seqNumber = counter;
             storeLocID(newPtr->curAirPdataP, token);
             fieldRead++;
           }            
               
           else if(fieldRead == 1)
           {
             storeFieldName(newPtr->curAirPdataP, token);
             fieldRead++;      
           }      
         
           else if(fieldRead == 2)
           { 
             storeCity(newPtr->curAirPdataP, token);
             fieldRead++;
           }               
           
           else if((fieldRead == 3 && strchr(dashPtr, '-')!=NULL) || (fieldRead == 3 && strtof(token,NULL) != 0.0F))
           {
               storeLatitude(newPtr->curAirPdataP, token);
               fieldRead++;          
           }
           else if(fieldRead == 4 && strchr(dashPtr, '-')!=NULL || (fieldRead == 4 && strtof(token,NULL) != 0.0F))
           {
               storeLongitude(newPtr->curAirPdataP, token);
               fieldRead++;
               counter++;
           }     
           token = strtok(NULL, comma);
         }
         temp->nextAirPdataList = newPtr;
       }       
     }
     
     fieldRead = 0;
   }
   
   
    //freeAirport(airport);
    free(line);
    fclose(inputFile);
  }
  return headPtr;
}

int isEmpty(lListAirPdata *headPtr) 
{   
  return headPtr == NULL;
}

/*The next several functions take a pointer to struct, 
and an input string, allocate memory for the member of the 
struct and copies the input string into
the struct. There is a function for each member
of the struct. The longitude and latitude functions
store the value if it's decimal, if it's DMS, they
call the conversion function and store the returned
decimal
*/

void storeLocID(airPdata *airP, char *input)
{
  airP->LocID = malloc(BUFFERSIZE * sizeof(char));
  
  if (airP->LocID == NULL) 
  {   
      errorPrint("LocID Malloc Failed!\n");   
  }
  else
    airP->LocID = strdup(input);
}

void storeFieldName(airPdata *airP, char *input)
{
  
  airP->fieldName = malloc(BUFFERSIZE * sizeof(char));
  
  if (airP->fieldName == NULL) 
  {   
      errorPrint("fieldName Malloc Failed!\n");   
  }
  else
    airP->fieldName = strdup(input);
}

void storeCity(airPdata *airP, char *input)
{
  airP->city = malloc(BUFFERSIZE * sizeof(char));
  
  if (airP->city == NULL) 
  {   
    errorPrint("storeState Malloc Failed!\n");
  }
  else
    airP->city = strdup(input);
}


void storeLatitude(airPdata *airP, char *input)
{
  char *dotPtr = strchr(input, '.');
  
  if(input[strlen(input) - 1] == 'N' || input[strlen(input) - 1] == 'S')
    airP->latitude = sexag2decimal(input);
  else if(dotPtr!=NULL)
    airP->latitude = strtof(input, NULL);
  else
    airP->latitude = 0.0;
}

void storeLongitude(airPdata *airP, char *input)
{
  char *dotPtr = strchr(input, '.');
  
  if(input[strlen(input) - 1] == 'E' || input[strlen(input) - 1] == 'W')
      airP->longitude = sexag2decimal(input);
  else if(dotPtr !=NULL)
    airP->longitude = strtof(input, NULL);
  else
    airP->longitude = 0.0;
}

//Prints the struct
void print (airPdata *airP)
{ 
  printf("%d. ",airP->seqNumber);
  printf("%s, ",airP->LocID);
  printf("%s, ",airP->fieldName);
  printf("%s, ",airP->city);
  printf("%.4f, ",airP->latitude);
  printf("%.4f\n",airP->longitude);
}

/*Frees the stuff inside the struct before
going on to free the struct itself*/
void freeAirport(airPdata *airP)
{
  free(airP->LocID);
  free(airP->fieldName);
  free(airP->city);
  free(airP);
}

/*
  Converts Degrees DD-MM-SS to Decimal by getting rid of any dashes
  and using string to float to convert each field and then doing the math
  as described in the homework instruction file. Returns 0.0 if the result is
  invalid. Checks the last character in the degreeString and if it's west or south
  makes the float negative
*/
float sexag2decimal(char *degreeString)
{
  char *dashPtr;
  char *degreeStringPtr;
  char direction;
  float degrees, minutes, seconds, sixty = 60.0, sixtySquared = 3600.0;
  int field = 0;
  
  if(degreeString == NULL)
    return 0.0;
  else
  {
    direction = degreeString[strlen(degreeString) - 1];
    degreeString[strlen(degreeString) - 1] = '\0';
    dashPtr = strchr(degreeString, '-');
    while(dashPtr != NULL)
    {
      *dashPtr = ' ';
      dashPtr = strchr(dashPtr + 1, '-');
    }
    
    degrees = strtof(degreeString, &degreeStringPtr);
    minutes = strtof(degreeStringPtr, &degreeStringPtr);
    seconds = strtof(degreeStringPtr, NULL);
    
    //printf("degrees: %f, minutes: %f, seconds: %f\n", degrees,minutes,seconds);
    
    if(degrees < 0.0 || degrees > 180.0 || minutes < 0.0 || minutes > 59.0 || seconds > 59.9999 || seconds < 0.0)
      return 0.0;
    else
    { 
      degrees+= ((minutes/sixty) + (seconds/sixtySquared));
      if(direction == 'S' || direction == 'W')
      {
        degrees = -(degrees);
      }
    }
  }
  return degrees;
}