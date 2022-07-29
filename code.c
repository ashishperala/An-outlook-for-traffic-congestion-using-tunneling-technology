#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include<time.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#define MAX 256
struct node{
	int vertex, weight;
	struct node* next;
};
struct AdjList{
    struct node *head;
};
struct Graph{
	int V;
	char* arrayOfCities[8];
	struct AdjList* array;
};
struct MinHeapNode{
	int v;
	int dist;
};
struct MinHeap{
	int size;	
	int capacity;
	int *pos;
	struct MinHeapNode **array;
};
char userName[30];
void reg(){
    FILE *ptr1;
    ptr1=fopen("user.txt","a+");
    char password[30];
    printf("ENTER USERNAME\n");
    scanf("%s",userName);
    printf("ENTER PASSWORD\n");
    scanf("%s",password);
    fprintf(ptr1,"%s %s\n",userName,password);
    fclose(ptr1);
    FILE *fptr1;
    fptr1 = fopen(userName, "a+");
    fprintf(fptr1, "W:%d\n",500);
    fclose(fptr1);
}
int login(){
    char tempUserName[30],tempPass[30],pass[30];
    printf("ENTER USERNAME\n");
    scanf("%s",userName);
    FILE *ptr1;
    ptr1=fopen("user.txt","a+");
    while(!feof(ptr1)){
        fscanf(ptr1,"%s %s\n",tempUserName,tempPass);
        if(strcmp(tempUserName,userName)==0){
            pass:
            printf("ENTER PASSWORD\n");
            scanf("%s",pass);
            if(strcmp(pass,tempPass)==0){
                printf("\nLOGIN SUCCESSFUL !!!\n");
                fclose(ptr1);
                return 0;
            }
            else{
                printf("\nWRONG PASSWORD");
                goto pass;
            }
        }
    }
    printf("\nUSER NOT FOUND PLEASE TRY AGAIN\n");
    login();
    return 0;
}
int getBalence(char fname[]){
    FILE *ptr1;
    ptr1=fopen(fname,"r");
    if (!ptr1) 
    { 
        printf("ERROR009");
    }
    fseek(ptr1, -10, SEEK_END);
    int bal;
    char ch;
    while(1)
    {
	    ch = fgetc(ptr1);
	    if(ch=='W')
	    {
	        fseek(ptr1, 1, SEEK_CUR);
	        fscanf(ptr1,"%d\n",&bal);
	    }
	    if(ch == EOF)
	    	break;
    }
    fclose(ptr1);
    return bal;
}
void putBalence(char fname[],int bal){
    FILE *ptr1;
    ptr1=fopen(fname,"r+");
    if (!ptr1) 
    { 
        printf("ERROR010");
    }
    fseek(ptr1, -10, SEEK_END);
    char ch;
    while(1)
    {
	    ch = fgetc(ptr1);
	    if(ch=='W')
	    {
	        fseek(ptr1, -1, SEEK_CUR);
	        fprintf(ptr1, "W:%d   ",bal);
	    }
	    if(ch == EOF)
	    	break;
    }
    fclose(ptr1);
}
void addToHistory(char fname[],char from[],char to[]){
    char ch;
    FILE *fptr1,*fptr2;
    char str[MAX], temp[] = "temp.txt";
    fptr1 = fopen(fname, "a+");
    if (!fptr1) 
	{
        printf("ERROR005\n");
        return ;
    }
    fptr2 = fopen(temp, "a+"); 
    if (!fptr2) 
	{
        printf("ERROR006\n");
        fclose(fptr1);
        return ;
    }
    time_t t;  
    time(&t);
    char info[MAX];strcpy(info,ctime(&t));
    fprintf(fptr2, "FROM:%s TO:%s ON %s",from,to,info);
    while (!feof(fptr1)) 
    {
        strcpy(str, "\0");
        fgets(str, MAX, fptr1);
        if (!feof(fptr1))
        {
            fprintf(fptr2, "%s", str);
        }
    }
    fclose(fptr1);
    fclose(fptr2);
    remove(fname);
    rename(temp, fname);
}
void poll(char fname[]){
    int ctr = 0,lno=0;
    char ch;
    FILE *fptr1,*fptr2;
    char str[MAX], temp[] = "temp.txt";
    fptr1 = fopen(fname, "a+");
    if (!fptr1) 
	{
        printf("ERROR003\n");
        return ;
    }
    fptr2 = fopen(temp, "a+"); 
    if (!fptr2) 
	{
        printf("ERROR004\n");
        fclose(fptr1);
        return ;
    }
    lno++;
    while (!feof(fptr1)) 
    {
        strcpy(str, "\0");
        fgets(str, MAX, fptr1);
        if (!feof(fptr1)) 
        {
            ctr++;
            if (ctr != lno) 
            {
                fprintf(fptr2, "%s", str);
            }
        }
    }
    fclose(fptr1);
    fclose(fptr2);
    remove(fname);
    rename(temp, fname);
}
void showHistory(char fname[]){
    FILE *ptr1;
    ptr1=fopen(fname,"r");
    if (!ptr1) 
    { 
        printf("ERROR007");
    } 
    // Read contents from file 
    char c;
    c = fgetc(ptr1); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(ptr1); 
    } 
    fclose(ptr1);
}
void clearHistory(char fname[]){
    FILE *ptr1;
    ptr1=fopen(fname,"w");
    if (!ptr1) 
    { 
        printf("ERROR007");
    }
    fclose(ptr1);
}
int found=0;
//////
struct MinHeapNode* newMinHeapNode(int v,int dist){
	struct MinHeapNode* minHeapNode =(struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->dist = dist;
	return minHeapNode;
}
struct MinHeap* createMinHeap(int capacity){
	struct MinHeap* minHeap =(struct MinHeap*)malloc(sizeof(struct MinHeap));
	minHeap->pos = (int *)malloc(capacity * sizeof(int));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =(struct MinHeapNode**)malloc(capacity *sizeof(struct MinHeapNode*));
	return minHeap;
}
void swapMinHeapNode(struct MinHeapNode** a,struct MinHeapNode** b){
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}
void minHeapify(struct MinHeap* minHeap,int idx){
	int smallest, left, right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;
	if (left < minHeap->size &&minHeap->array[left]->dist <minHeap->array[smallest]->dist )
	smallest = left;
	if (right < minHeap->size &&minHeap->array[right]->dist <minHeap->array[smallest]->dist )
	smallest = right;
	if (smallest != idx){
		struct MinHeapNode* smallestNode =minHeap->array[smallest];
		struct MinHeapNode* idxNode =minHeap->array[idx];
		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;
		swapMinHeapNode(&minHeap->array[smallest],&minHeap->array[idx]);
		minHeapify(minHeap, smallest);
	}
}
int isEmpty(struct MinHeap* minHeap){
	return minHeap->size == 0;
}
struct MinHeapNode* extractMin(struct MinHeap*minHeap){
	if (isEmpty(minHeap))
	return NULL;
	struct MinHeapNode* root =minHeap->array[0];
	struct MinHeapNode* lastNode =
	minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;
	minHeap->pos[root->v] = minHeap->size-1;
	minHeap->pos[lastNode->v] = 0;
	--minHeap->size;
	minHeapify(minHeap, 0);
	return root;
}
void decreaseKey(struct MinHeap* minHeap,int v, int dist){
	int i = minHeap->pos[v];
	minHeap->array[i]->dist = dist;
	while (i && minHeap->array[i]->dist <minHeap->array[(i - 1) / 2]->dist)
	{
		minHeap->pos[minHeap->array[i]->v] =(i-1)/2;
		minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
		swapMinHeapNode(&minHeap->array[i],&minHeap->array[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}
int isInMinHeap(struct MinHeap *minHeap, int v){
    if (minHeap->pos[v] < minHeap->size)
	    return 1;
    return 0;
}
int getDistance(struct Graph* graph, int src,int vertexination){
	int V = graph->V;
	int dist[V];	
	struct MinHeap* minHeap = createMinHeap(V);
	for (int v = 0; v < V; ++v)
	{
		dist[v] = INT_MAX;
		minHeap->array[v] = newMinHeapNode(v,dist[v]);
		minHeap->pos[v] = v;
	}
	minHeap->array[src] =newMinHeapNode(src, dist[src]);
	minHeap->pos[src] = src;
	dist[src] = 0;
	decreaseKey(minHeap, src, dist[src]);
	minHeap->size = V;
	while (!isEmpty(minHeap))
	{
		struct MinHeapNode* minHeapNode =extractMin(minHeap);
		int u = minHeapNode->v;
		struct node* pCrawl =graph->array[u].head;
		while (pCrawl != NULL)
		{
			int v = pCrawl->vertex;
			if (isInMinHeap(minHeap, v) &&dist[u] != INT_MAX &&pCrawl->weight + dist[u] < dist[v])
			{
				dist[v] = dist[u] + pCrawl->weight;
				decreaseKey(minHeap, v, dist[v]);
			}
			pCrawl = pCrawl->next;
		}
	}
	return dist[vertexination];
}
//////////
float bill_generator(struct Graph* graph,int src, int dest){//returns time of travel
    int d;
    float timeOfTravel=0.0;
    d=getDistance(graph,src,dest);
    timeOfTravel=((float)d)/3.0;
    return timeOfTravel;
}
struct node* newnode(int vertex, int weight)
{
	struct node* newNode =(struct node*)malloc(sizeof(struct node));
	newNode->vertex = vertex;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}
struct Graph* createGraph(int V){
	struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
	graph->V = V;
	graph->array = (struct AdjList*)malloc(V * sizeof(struct AdjList));
	for (int i = 0; i < V; ++i)
		graph->array[i].head = NULL;
	return graph;
}
void addTunnel(struct Graph* graph, int src,int vertex, int weight)
{
	struct node* newNode =newnode(vertex, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;
	newNode = newnode(src, weight);
	newNode->next = graph->array[vertex].head;
	graph->array[vertex].head = newNode;
}
int main() {
    int option,trayNum,src,dest,amount,lr,start=0;
    char trayq[20];
    char passName[25];
    int flag=0,travelTime;
    float t;
  struct Graph* Hogwarts = createGraph(8);
  addTunnel(Hogwarts,0,3,15);
  addTunnel(Hogwarts,0,4,13);
  addTunnel(Hogwarts,1,4,2);
  addTunnel(Hogwarts,1,3,1);
  addTunnel(Hogwarts,3,4,2);
  addTunnel(Hogwarts,3,5,5);
  addTunnel(Hogwarts,5,2,3);
  addTunnel(Hogwarts,3,6,3);
  addTunnel(Hogwarts,6,7,6);
  addTunnel(Hogwarts,6,4,2);
  addTunnel(Hogwarts,4,7,20);
  Hogwarts->arrayOfCities[0]="Hogsmeade Station";
  Hogwarts->arrayOfCities[1]="Black Lake";
  Hogwarts->arrayOfCities[2]="Shrieking Shack";
  Hogwarts->arrayOfCities[3]="Quidditch Pitch";
  Hogwarts->arrayOfCities[4]="Hogwarts";
  Hogwarts->arrayOfCities[5]="Hogsmeade village";
  Hogwarts->arrayOfCities[6]="Hagrid's Hut";
  Hogwarts->arrayOfCities[7]="Forbidden Forest";
  start01 :printf("\nLOGIN OR SIGNUP TO CONTINUE\nFOR LOGIN PRESS 1 FOR REGISTER PRESS 2\n");
  scanf("%d",&lr);
  switch(lr){
      case 1:{
          login();
      }
      break;
      case 2:{
          reg();
      }
      break;
      default:{printf("\nWRONG KEY");goto start01;}
  }
  int currentBalence,updatedBalence;
  start02 :printf("\nWHAT DO YOU LIKE TO DO\n");
  printf("1.TO BOOK A TRAY FOR YOUR CAR\n2.TO SIT AND TRAVEL\n3.TO DELIVER A PACKAGE\n4.CHECK YOUR PREVIOUS HISTORY\n5.CLEAR YOUR PREVIOUS HISTORY\n6.TO CHECK YOUR WALLET BALENCE\n7.LOGOUT\n");
  scanf("%d",&option);
  switch(option){
      case 1:{
          printf("\nYou can Enter and Exit Trays from regions listed here\n0.HOGSMEADE STATION\n1.BLACK LAKE\n2.SHRIEKING SHACK\n3.QUIDDITCH PITCH\n4.HOGWARTS\n5.HOGSMEADE VILLAGE\n6.HAGRID'S HUT\n7.FORBIDDEN FOREST");
          printf("\nENTER SOURCE AND DESTINATION INDICES\n");
          scanf("%d%d",&src,&dest);
          printf("THERE ARE 4 TRAYS AVAILABLE AT SELECTED LOCATION\nPICK ANY ONE OF THEM AND ENTER TRAY NUMBER\n");
          scanf("%d",&trayNum);
          sprintf(trayq, "%d", trayNum);
          char a[10];
          sprintf(a, "%d", src);
          strcat(trayq,a);
          FILE *fptr1;
          fptr1 = fopen(trayq, "a+");
          if (!fptr1) {
                printf("\nERROR 001\n");
                return 0;
          }
          char ch=getc(fptr1);
          if(ch==EOF){
              t=bill_generator(Hogwarts,src,dest);
              currentBalence=getBalence(userName);
              if(currentBalence<(t*30)){
                printf("YOUR BALENCE IS NOT ENOUGH FOR TRAVELLING");
                goto start02;
              }
              printf("QUEUE IS EMPTY PLEASE PROCEED TO THE TRAY\nPRESS 1 ONCE YOU ENTERED THE TRAY\n");
              check01:scanf("%d",&start);
              if(start==1){
                printf("\nWELCOME TO THE TUNNELS YOU WILL BE TRAVELLING AT A SPEED OF 180 KM/HR\nYOUR TOTAL TIME OF TRAVEL IS %f MINUTES\nAND COST OF YOUR TICKET IS %f",t,(t*30));
                updatedBalence=currentBalence-(int)t*30;
                fprintf(fptr1,"%s FROM:%d TO:%d\n",userName,src,dest);
                //sleep(t*60);
                fclose(fptr1);
                addToHistory(userName,Hogwarts->arrayOfCities[src],Hogwarts->arrayOfCities[dest]);
                printf("\n\nYOUR JOURNEY IS COMPLETED");
                poll(trayq);
                putBalence(userName,updatedBalence);
                goto start02;
              }
              else{
                printf("\nWRONG CHOICE PLEASE ENTER AGAIN\n");
                goto check01;
              }
          }
          else{
            printf("YOU HAVE BEEN ADDED TO QUEUE PLEASE WAIT THE TRAY IS BUSY\n");
            sleep(5);
            check:fseek(fptr1,0,SEEK_SET);
            char ch=getc(fptr1);
            if(ch==EOF){
               flag=1;
            }
            else{
                fclose(fptr1);
                sleep(5);
                fptr1 = fopen(trayq, "a+");
                if (!fptr1) {
                    printf("\nERROR 008\n");
                    return 0;
                }
                goto check;
            }
            if(flag==1){
                printf("ITS YOUR TURN NOW PLEASE ENTER THE TRAY\nPRESS 1 ONCE YOU ENTERED THE TRAY\n");
                fprintf(fptr1,"%s FROM:%d TO:%d\n",userName,src,dest);
                check02:scanf("%d",&start);
                if(start==1){
                    t=bill_generator(Hogwarts,src,dest);
                    sleep(t*60);
                }
                else{
                    printf("\nWRONG CHOICE PLEASE ENTER AGAIN\n");
                    goto check02;
                }
                printf("\nYOUR JOURNEY IS COMPLETED");
                fclose(fptr1);
                poll(trayq);
                goto start02;
            }
          }
          goto start02;
      }
      break;
      case 2:
      {
          printf("\nYou can Enter and Exit buses from regions listed here\n0.HOGSMEADE STATION\n1.BLACK LAKE\n2.SHRIEKING SHACK\n3.QUIDDITCH PITCH\n4.HOGWARTS\n5.HOGSMEADE VILLAGE\n6.HAGRID'S HUT\n7.FORBIDDEN FOREST");
          printf("\nENTER SOURCE AND DESTINATION INDICES\n");
          scanf("%d%d",&src,&dest);
          t=bill_generator(Hogwarts,src,dest);
          currentBalence=getBalence(userName);
          if(currentBalence<(t*30)){
            printf("YOUR BALENCE IS NOT ENOUGH FOR TRAVELLING");
            goto start02;
          }
          printf("\nPRESS 1 ONCE YOU ENTERED THE BUS\n");
          check03:scanf("%d",&start);
          if(start==1){
            printf("\nWELCOME TO THE TUNNELS YOU WILL BE TRAVELLING AT A SPEED OF 180 KM/HR\nYOUR TOTAL TIME OF TRAVEL IS %f MINUTES\nAND COST OF YOUR TICKET IS %f",t,(t*30));
            updatedBalence=currentBalence-(int)t*30;
            sleep(t*60);
            addToHistory(userName,Hogwarts->arrayOfCities[src],Hogwarts->arrayOfCities[dest]);
            printf("\n\nYOUR JOURNEY IS COMPLETED");
            putBalence(userName,updatedBalence);
            goto start02;
          }
          else{
            printf("\nWRONG CHOICE PLEASE ENTER AGAIN\n");
            goto check03;
          }
      }
      break;
      case 3:
      {
          printf("\nYou can deliver and recieve pakages from regions listed here\n0.HOGSMEADE STATION\n1.BLACK LAKE\n2.SHRIEKING SHACK\n3.QUIDDITCH PITCH\n4.HOGWARTS\n5.HOGSMEADE VILLAGE\n6.HAGRID'S HUT\n7.FORBIDDEN FOREST");
          printf("\nENTER SOURCE AND DESTINATION INDICES\n");
          scanf("%d%d",&src,&dest);
          t=bill_generator(Hogwarts,src,dest);
          currentBalence=getBalence(userName);
          if(currentBalence<(t*10)){
            printf("YOUR CURRENT BALENCE IS NOT ENOUGH ");
            goto start02;
          }
          printf("\nPRESS 1 ONCE YOU PLACED YOUR PACKAGE IN PACKAGE DELIVERY TRAY\n");
          check04:scanf("%d",&start);
          if(start==1){
            printf("\nYOUR PACKAGE WILL REACH %s IN %f MINUTES STARTING FROM NOW...\nAND COST OF YOUR TICKET IS %f.",Hogwarts->arrayOfCities[dest],t,(t*10));
            updatedBalence=currentBalence-(int)t*10;
            putBalence(userName,updatedBalence);
            goto start02;
          }
          else{
            printf("\nWRONG CHOICE PLEASE ENTER AGAIN\n");
            goto check04;
          }
      }
      break;
      case 4:showHistory(userName);goto start02;
      break;
      case 5:clearHistory(userName);goto start02;
      break;
      case 6:printf("YOUR CURRENT BALENCE IS %d",getBalence(userName));goto start02;
      break;
      case 7:return 0;
      break;
      default:{printf("\nENTER CORRECT CHOICE");goto start02;}
    }   
    return 0;
}