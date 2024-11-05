#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define String_SIZE 16//Extra 1 for null terminator
#define INFINITY 2147483647//The largest value of int to represent infinity

typedef struct node *node_ptr;
typedef struct graph *graph_ptr;
typedef struct headOfList *listHead;
typedef struct HNode *Heap;
typedef struct MinHeap *minHQ;
struct node {
    char *vertex;
    int weight;
    node_ptr next;
};

struct headOfList {
    node_ptr head;//A pointer to the head of a list of nodes
};

struct graph {
    int VerNum;
    listHead arrayOfLists;//An array of heads of lists
};

struct HNode {
    char *word;
    int dis;
};

struct MinHeap {
    int maxsize;
    int size;
    struct HNode *Heap;
};
graph_ptr dijk = NULL, topSort = NULL;
char *recent_topSorted = NULL, *recent_path = NULL;
int distance;

node_ptr createNode(char *vertex, int weight);

char *fix(char *word);

graph_ptr createGraph(int VerNum);

void insertEdge(graph_ptr GRAPH, char *from, char *to, int weight);

void freeAll(node_ptr l);

int findNode(graph_ptr GRAPH, char *word);

void loadFiles();

void printGraph(graph_ptr GRAPH);

int *GetInDegrees(graph_ptr GRAPH);

void topological(graph_ptr GRAPH);

void print_topSorted();

void swap(struct HNode *x, struct HNode *y);

int parent(int i);

int leftChild(int i);

int rightChild(int i);

int isLeaf(int pos, struct MinHeap *heap);

void createHeap(struct MinHeap *heap, int maxsize);

void insert(char *word, int dis, struct MinHeap *heap);

void minHeapify(int pos, struct MinHeap *heap);

void dijkstra(graph_ptr GRAPH, char *from, char *to);

char *extractMin(minHQ heap);

int main() {
    short choice;
    char from[String_SIZE], to[String_SIZE];
    while (1) {
        printf("\n-------------------------------------------------------------------------\n"
               "1. Load the two files.\n"
               "2. Calculate the shortest distance between two buildings.\n"
               "3. Print the shortest route between two buildings and the total distance.\n"
               "4. Sort the courses using the topological sort.\n"
               "5. Print the sorted courses.\n"
               "6. Exit the application.\n"
               "7. Print graph.\n");
        scanf("%hd", &choice);
        getchar(); // Clear the newline character from the input buffer
        switch (choice) {
            case 1:
                loadFiles();
                if (topSort == NULL || dijk == NULL) {
                    freeAll(topSort->arrayOfLists->head);
                    freeAll(dijk->arrayOfLists->head);
                    free(topSort);
                    free(dijk);
                    return 0;
                }
                break;
            case 2: {
                printf("Enter the source building: ");
                scanf("%s", from);
                printf("Enter the destination building: ");
                scanf("%s", to);
                dijkstra(dijk, from, to);
                break;
            }
            case 3:
                printf("For the recent?\n");
                scanf("%s", from);
                if (strcasecmp(from, "yes") == 0) {
                    printf("%s\ntotal: %d\n", recent_path, distance);
                    break;
                } else {
                    printf("Enter the source building: ");
                    scanf("%s", from);
                    printf("Enter the destination building: ");
                    scanf("%s", to);
                    free(recent_path);
                    recent_path = NULL;
                    dijkstra(dijk, from, to);
                }
                if (recent_path != NULL)
                    printf("%s\ntotal from %s to %s: %d\n", recent_path, from, to, distance);
                break;
            case 4:
                topological(topSort);
                printf("got sorted\n");
                break;
            case 5:
                if (recent_topSorted == NULL)
                    topological(topSort);
                print_topSorted();
                break;
            case 6:
                freeAll((node_ptr) topSort->arrayOfLists->head);
                freeAll((node_ptr) dijk->arrayOfLists->head);
                free(topSort);
                free(dijk);
                return 0;
            case 7:
                printf("  Courses:-\n");
                printGraph(topSort);
                printf("\n  Buildings:-\n");
                printGraph(dijk);
                break;
            default:
                printf("There is no such option\n");
                break;
        }
    }
}

char *fix(char *word) {
    // Remove leading spaces
    while (word[0] == ' ') {
        int i = 0;
        while (word[i + 1] != '\0') {
            word[i] = word[i + 1];
            i++;
        }
        word[strlen(word) - 1] = '\0';
    }
    // Remove trailing spaces
    if (word[strlen(word) - 1] == ' ') {
        word[strlen(word) - 1] = '\0';
    }
    return word;
}

void loadFiles() {
    topSort = createGraph(0);
    dijk = createGraph(0);
    char line1[String_SIZE * 3], line2[String_SIZE * 3], word1[String_SIZE], word2[String_SIZE];
    FILE *Topological = fopen("input_courses.txt", "r");
    FILE *Dijkstra = fopen("input_buildings.txt", "r");
    if (Topological == NULL || Dijkstra == NULL) {
        printf("Failed to find the file. Program will terminate\n");
        if (Topological) fclose(Topological);
        if (Dijkstra) fclose(Dijkstra);
        return;
    }
    while (fgets(line1, sizeof(line1), Topological) != NULL) {
        // Remove newline character (\n) after each line in file
        line1[strcspn(line1, "\n")] = '\0';
        char *token = strtok(line1, "#");
        if (token != NULL) {
            strcpy(word1, fix(token));
            token = strtok(NULL, "#");
            if (token != NULL) {
                strcpy(word2, fix(token));
                insertEdge(topSort, word2, word1, 0);
                token = strtok(NULL, "#");
                if (token != NULL) {
                    strcpy(word2, fix(token));
                    insertEdge(topSort, word2, word1, 0);
                }
            }
        }
    }
    while (fgets(line2, sizeof(line2), Dijkstra) != NULL) {
        line2[strcspn(line2, "\n")] = '\0';
        char *token = strtok(line2, "#");
        if (token != NULL) {
            strcpy(word1, fix(token));
            token = strtok(NULL, "#");
            if (token != NULL) {
                strcpy(word2, fix(token));
                token = strtok(NULL, "#");
                if (token != NULL) {
                    insertEdge(dijk, word1, word2, atoi(fix(token)));
                }
            }
        }
    }
    fclose(Topological);
    fclose(Dijkstra);
}

node_ptr createNode(char *vertex, int weight) {
    node_ptr new = (node_ptr) malloc(sizeof(struct node));
    if (new == NULL) {
        printf("Out of space. Program will terminate\n");
        return NULL;
    }
    new->vertex = (char *) malloc(strlen(vertex) + 1); // +1 for the null terminator
    strcpy(new->vertex, vertex);
    new->weight = weight;
    new->next = NULL;
    return new;
}

graph_ptr createGraph(int VerNum) {
    graph_ptr graph = (graph_ptr) malloc(sizeof(struct graph));
    if (graph == NULL) {
        printf("Out of space. Program will terminate\n");
        return NULL;
    }
    graph->VerNum = VerNum;
    graph->arrayOfLists = (listHead) malloc(VerNum * sizeof(struct headOfList));
    if (graph->arrayOfLists == NULL) {
        printf("Out of space. Program will terminate\n");
        free(graph);
        return NULL;
    }
    for (int i = 0; i < VerNum; i++)
        graph->arrayOfLists[i].head = NULL;
    return graph;
}

void insertEdge(graph_ptr GRAPH, char *from, char *to, int weight) {
    if (strlen(from) == 0) return;
    int indexFrom = findNode(GRAPH, from);
    if (indexFrom == -1) {
        indexFrom = GRAPH->VerNum;
        GRAPH->arrayOfLists = realloc(GRAPH->arrayOfLists, (GRAPH->VerNum + 1) * sizeof(struct headOfList));
        if (GRAPH->arrayOfLists == NULL) {
            printf("Out of space. Program will terminate\n");
            freeAll(GRAPH->arrayOfLists->head);
            return;
        }
        GRAPH->arrayOfLists[indexFrom].head = createNode(from, weight);
        GRAPH->VerNum++;
    }
    int indexTo = findNode(GRAPH, to);
    if (indexTo == -1) {
        indexTo = GRAPH->VerNum;
        GRAPH->arrayOfLists = realloc(GRAPH->arrayOfLists, (GRAPH->VerNum + 1) * sizeof(struct headOfList));
        if (GRAPH->arrayOfLists == NULL) {
            printf("Out of space. Program will terminate\n");
            freeAll(GRAPH->arrayOfLists->head);
            return;
        }
        GRAPH->arrayOfLists[indexTo].head = createNode(to, weight);
        GRAPH->VerNum++;
    }
    // Add the 'to' node to the list of the 'from' head node
    node_ptr destination = createNode(to, weight);
    destination->next = GRAPH->arrayOfLists[indexFrom].head->next;
    GRAPH->arrayOfLists[indexFrom].head->next = destination;
}

int findNode(graph_ptr GRAPH, char *word) {
    for (int i = 0; i < GRAPH->VerNum; i++) {
        node_ptr finder = GRAPH->arrayOfLists[i].head;
        if (finder != NULL && strcasecmp(finder->vertex, word) == 0)
            return i; // Found, so it is already in the graph
    }
    return -1;
}

void freeAll(node_ptr l) {
    while (l != NULL) {
        node_ptr temp = l;
        l = l->next;
        free(temp->vertex);
        free(temp);
    }
}

void printGraph(graph_ptr GRAPH) {
    for (int i = 0; i < GRAPH->VerNum; i++) {
        printf("Vertex %d: ", i);
        node_ptr current = GRAPH->arrayOfLists[i].head;
        printf("[%s]-> ", current->vertex);
        current = current->next;
        while (current != NULL) {
            printf("(%s, weight= %d) ", current->vertex, current->weight);
            current = current->next;
        }
        printf("\n");
    }
}

int *GetInDegrees(graph_ptr GRAPH) {
    int *inDegreeArray = (int *) malloc(GRAPH->VerNum * sizeof(int));
    if (inDegreeArray == NULL) {
        printf("Out of memory.\n");
        return NULL;
    }
    for (int i = 0; i < GRAPH->VerNum; i++) {
        inDegreeArray[i] = 0;
    }
    for (int i = 0; i < GRAPH->VerNum; i++) {
        node_ptr current = GRAPH->arrayOfLists[i].head->next;
        while (current != NULL) {
            int indexTo = findNode(GRAPH, current->vertex);
            if (indexTo != -1)
                inDegreeArray[indexTo]++;
            current = current->next;
        }
    }
    return inDegreeArray;
}

void topological(graph_ptr GRAPH) {
    recent_topSorted = (char *) malloc((GRAPH->VerNum * String_SIZE) + 1);//a dynamic array
    if (recent_topSorted == NULL) {
        printf("Out of space. You can no longer sort the graph\n");
        return;
    }
    recent_topSorted[0] = '\0';//initializing it
    int *inDegreeArray = GetInDegrees(GRAPH);
    char *stack[GRAPH->VerNum];//a stack array with a top pointer
    int top = -1;
    for (int i = 0; i < GRAPH->VerNum; i++) {
        if (inDegreeArray[i] == 0) {
            stack[++top] = GRAPH->arrayOfLists[i].head->vertex;
        }
    }
    while (top != -1) {
        char *finished = stack[top--];
        strcat(recent_topSorted, finished);//stores the nodes whose children were already visited
        strcat(recent_topSorted, " ");
        int index = findNode(GRAPH, finished);//to get the index of it so we reach its children
        node_ptr temp = GRAPH->arrayOfLists[index].head->next;
        while (temp != NULL) {
            int ver = findNode(GRAPH, temp->vertex);
            inDegreeArray[ver]--;//we found a predecessor
            if (inDegreeArray[ver] == 0) {//when all predecessors got found, inDegree becomes zero so we pop it
                stack[++top] = GRAPH->arrayOfLists[ver].head->vertex;
            }
            temp = temp->next;
        }
    }
    printf("\n");
    free(inDegreeArray);
}

void print_topSorted() {
    printf("Topologically sorted courses:-\n%s\n", recent_topSorted);
    free(recent_topSorted);
    recent_topSorted = NULL;//reset the sorted array
}

void swap(struct HNode *x, struct HNode *y) {
    struct HNode temp = *x;
    *x = *y;
    *y = temp;
}

int parent(int i) { return (i - 1) / 2; }

int leftChild(int i) { return (2 * i + 1); }

int rightChild(int i) { return (2 * i + 2); }

int isLeaf(int pos, minHQ heap) { return pos >= (heap->size / 2) && pos < heap->size; }

void createHeap(minHQ heap, int maxsize) {
    heap->maxsize = maxsize;
    heap->size = 0;
    heap->Heap = (Heap) malloc(maxsize * sizeof(struct HNode));
    if (heap->Heap == NULL) {
        printf("Out of space\n");
        return;
    }
}

void insert(char *word, int dis, minHQ heap) {
    if (heap->size >= heap->maxsize) {
        return;
    }
    int current = heap->size;
    heap->Heap[current].word = word;
    heap->Heap[current].dis = dis;
    heap->size++;
    while (current != 0 && heap->Heap[current].dis < heap->Heap[parent(current)].dis) {
        swap(&heap->Heap[current], &heap->Heap[parent(current)]);
        current = parent(current);
    }
}


void minHeapify(int pos, minHQ heap) {
    if (!isLeaf(pos, heap)) {
        if (heap->Heap[pos].dis > heap->Heap[leftChild(pos)].dis ||
            heap->Heap[pos].dis > heap->Heap[rightChild(pos)].dis) {
            if (heap->Heap[leftChild(pos)].dis < heap->Heap[rightChild(pos)].dis) {
                swap(&heap->Heap[pos], &heap->Heap[leftChild(pos)]);
                minHeapify(leftChild(pos), heap);
            } else {
                swap(&heap->Heap[pos], &heap->Heap[rightChild(pos)]);
                minHeapify(rightChild(pos), heap);
            }
        }
    }
}

void dijkstra(graph_ptr GRAPH, char *from, char *to) {
    if (findNode(GRAPH, from) == -1 || findNode(GRAPH, to) == -1) {
        printf("Make sure the buildings already exist in the file. Add them to the file then rerun the program\n");
        return;
    }
    int containing = GRAPH->VerNum, Dis[containing], prev[containing];
    char *nodes[containing];
    for (int i = 0; i < containing; i++) {
        nodes[i] = GRAPH->arrayOfLists[i].head->vertex;
        Dis[i] = INFINITY;
        prev[i] = -1;
    }
    struct MinHeap heap;
    createHeap(&heap, containing);
    int srcIndex = findNode(GRAPH, from);
    Dis[srcIndex] = 0;
    insert(from, 0, &heap);
    while (heap.size != 0) {
        char *u = extractMin(&heap);
        if (strcmp(u, to) == 0)
            break; //Reached the destination
        int uIndex = findNode(GRAPH, u);
        node_ptr temp = GRAPH->arrayOfLists[uIndex].head->next;
        while (temp != NULL) {
            int v = findNode(GRAPH, temp->vertex);
            if (Dis[v] > Dis[uIndex] + temp->weight) {
                Dis[v] = Dis[uIndex] + temp->weight;
                prev[v] = uIndex;
                insert(temp->vertex, Dis[v], &heap);
            }
            temp = temp->next;
        }
    }
    int destIndex = findNode(GRAPH, to);
    if (Dis[destIndex] == INFINITY) {
        printf("No path exists from %s to %s\n", from, to);
    } else {
        printf("Shortest path from %s to %s is %d\n", from, to, Dis[destIndex]);
        free(recent_path);
        recent_path = NULL;
    }
    recent_path = (char *) malloc(containing * sizeof(char *));
    if (recent_path == NULL) {
        printf("Out of space\n");
        return;
    }
    recent_path[0] = '\0';
    int pathLength = 0, path[containing], total = 0;
    int i = findNode(GRAPH, to);
    while (i != -1) {
        path[pathLength++] = i;
        if (prev[i] != -1) total += Dis[i] - Dis[prev[i]];
        i = prev[i];
    }
    for (i = pathLength - 1; i >= 0; i--) {
        strcat(recent_path, nodes[path[i]]);
        if (i > 0)
            strcat(recent_path, " -> ");
    }
    distance = Dis[destIndex];
}

char *extractMin(minHQ heap) {
    if (heap->size == 0) return NULL;
    struct HNode root = heap->Heap[0];
    heap->Heap[0] = heap->Heap[heap->size - 1];
    heap->size--;
    minHeapify(0, heap);
    return root.word;
}
