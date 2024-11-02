#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define queueSize 10
#define initialFrontOfQueue 9
#define INSERT 1
#define REMOVE 2
#define toUndo 1
#define toRedo 2
#define MAX_LENGTH 21//it is rare to use a word longer than 20, the one is for the null character
typedef struct stack *stack_ptr;

struct stack {
    char *word;
    short operation;//since short is more than enough to contain all the choices which are only there is no need for int
    int index;//short can be used as wel but let's assume we can have an extremely large text
    stack_ptr next;//To traverse the stacks
};

stack_ptr undo_stack = NULL, redo_stack = NULL;
// stack_ptr queueFront = NULL, queueRear = NULL;
char *original = NULL, *CurrentText = NULL;

stack_ptr queue[queueSize];//A circular array representing the queue to help insert a sentence or a word
//Short since there are only 10 spaces (index 0-9)
short emptySpot = 9, frontOfQueue = 9;//It points to the empty spot which is also the nearest spot the front of the queue
//It is initialized with the front index which I considered to be 9

void load_file();//A function to get the content (text) from the input file

stack_ptr createNode(short operation, char *text, int index);//A function to create the nodes and initializing them

void enqueue(short operation, char *text, int index);//A function to add a word or a letter to the queue

void dequeue(short addSpace, int indexOfQueue);//A function to remove a word or a letter from the queue

void Push(short To, stack_ptr nodeOfTheText);//A function to pop a specific stack's first node

void Pop(short fromPrint, short To, stack_ptr nodeOfTheText);//A function to push a specific stacks first node

void UpdateText(short addSpace);//A function to dequeue all the contents of the queue

void InsertToken(stack_ptr node);

void RemoveToken(stack_ptr node);

void printStack(short To);//A function to print a specific stack with the details of its nodes

void linkedListPrint();//Print while defying the restrictions of a stack

void FreeStack(stack_ptr stack);//A function to free the nodes in the stack to free the stack

void saveToFile();//Prints the CurrentText to the output file

int main() {
    short choice = 0, addSpace = 0;
    int at;
    char token[MAX_LENGTH * 10], WORD[MAX_LENGTH], *T = NULL;//assume a sentence consists of 10 words
    while (1) {
        printf("1. Load the input file which contains the initial text.\n"
               "2. Print the loaded text.\n"
               "10. Print the current text.\n"
               "3. Insert strings to the text.\n"
               "4. Remove strings from the text.\n"
               "5. Perform Undo operation\n"
               "6. Perform Redo operation\n"
               "7. Print the Undo Stack and the Redo stack\n"
               "8. Save the updated text to the output file.\n"
               "9. Exit\n");
        scanf("%hd", &choice);//%hd is used to read a short from the user
        while (getchar() !=
               '\n');//To clear buffer, if the user pressed space after entering the choice instead of enter it will clear it as well and whatever they typed
        switch (choice) {
            case 1:
                load_file();
                if (original ==
                    NULL || CurrentText ==
                            NULL)//if original isn't pointing to the original text, there isn't enough memory to store the original text and there won't be enough memory for the stacks leading to the failure of the program, so it terminates
                    return 0;
                break;
            case 2://Printing the text from the input file
                printf("\n%s\n\n", original);//original is pointing to the original text
                break;
            case 3:
                printf("Enter the index to enter after, but if it is meant to be added at the end enter '-1'\n");
                while (scanf("%d", &at) != 1 || at >
                                                strlen(CurrentText)) {//In case the entered is not an integer, the user is asked to try again
                    if (at == -1)
                        break;
                    printf("Invalid input. Try again\n");
                    while (getchar() != '\n');//To prevent an infinite loop by clearing the buffer
                }
                while (getchar() !=
                       '\n');//To clear buffer, if the user pressed space after entering the index instead of enter it will clear it as well and whatever the typed
                printf("Enter the text to be inserted\n");
                scanf("%[^\n]", token);//Reads from user even if it is a sentence
                //If a sentence is read, split it into words
                T = strtok(token, " ");
                while (T != NULL) {
                    if (at != -1)
                        at += (int) strlen(T);
                    enqueue(INSERT, strcpy(WORD, T), at);
                    T = strtok(NULL, " ");
                }
                if (queue[8] != NULL)
                    addSpace = 1;
                UpdateText(addSpace);
                if (CurrentText ==
                    NULL)//if CurrentText isn't pointing to a text, there isn't enough memory to store the text and there won't be enough memory for the stacks leading to the failure of the program, so it terminates
                    return 0;
                printf("The text got inserted, you can check by choosing instruction '10'\n\n");
                break;
            case 4:
                printf("Enter the index of the text to be removed, but if you intend to remove the first occurrence enter '-2'\n");
                while (scanf("%d", &at) != 1 || at >
                                                strlen(CurrentText)) {//In case the entered is not an integer, the user is asked to try again
                    if (at == -2)
                        break;
                    printf("Invalid input. Try again\n");
                    while (getchar() != '\n');//To prevent an infinite loop by clearing the buffer
                }
                while (getchar() !=
                       '\n');//To clear buffer, if the user pressed space after entering the index instead of enter it will clear it as well
                printf("Enter the text to be removed\n");
                scanf("%[^\n]", token);//Reads from user even if it is a sentence
                enqueue(REMOVE, token, at);
                UpdateText(addSpace);
                if (CurrentText ==
                    NULL)//if CurrentText isn't pointing to a text, there isn't enough memory to store the text and there won't be enough memory for the stacks leading to the failure of the program, so it terminates
                    return 0;
                break;
            case 5:
                Pop(0, toUndo, undo_stack);
                printf("Undo done\n\n");
                break;
            case 6:
                Pop(0, toRedo, redo_stack);
                printf("Redo done\n\n");
                break;
            case 7:
                printf("(Undo Stack)\n");
                printStack(toUndo);
                printf("*******************************************************************\n\n");
                printf("(Redo Stack)\n");
                printStack(toRedo);
                //  linkedListPrint();
                printf("\n\n");
                break;
            case 8:
                saveToFile();
                break;
            case 9:
                //It gets freed automatically when the program terminates though
                free(original);
                free(CurrentText);
                if (undo_stack != NULL)
                    FreeStack(undo_stack);
                if (redo_stack != NULL)
                    FreeStack(redo_stack);
                return 0;
            case 10://Printing the current text(with all the changes)
                printf("\n%s\n\n", CurrentText);
                break;
            default:
                printf("\nThere is no instruction assigned to this number.\nChoose a valid operation number\n\n");
                break;
        }
    }
}

void load_file() {
    char filename[2 * MAX_LENGTH];//To insure it is big enough I doubled the MAX_LENGTH
    printf("Enter the name of the file\n");
    scanf("%s", filename);
    short locationOFtxt = (short) strlen(filename);
    if (strcmp(filename + locationOFtxt - 4, ".txt") !=
        0)//Points to the substring starting at the index 4 letters less than the length of the filename to check if they equal '.txt'
        strcat(filename, ".txt");
    FILE *file = fopen(filename, "r");
    if (file == NULL) {//To make sure the file exists
        printf("Failed to open the file, check if it really exists, then run the program again\n\n");
        return;
    }
    char text[MAX_LENGTH * 10];//assuming it s a sentence of 10 words
    while (fgets(text, sizeof(text), file) != NULL) {//Reading the content of the file until there is no more
        text[strcspn(text, "\n")] = '\0';//To replace the newline character read from the file with a null character
        if (original == NULL) {
            original = strdup(
                    text);//It reserves memory, gets the length of the string by strlen, adds 1 for the null terminator '\0' and assigns a pointer to a copy of the string
            CurrentText = strdup(
                    text);//This is a stable string of the original text so it can be printed excluding all the changes that happen to the text
            if (original == NULL || CurrentText == NULL) {
                printf("Failed to allocate memory; there might not be enough memory to proceed with the program so it will terminate\n\n");
                return;
            }
        } else {//If there was still more text in the file
            char *temp_original = (char *) realloc(original, strlen(original) + strlen(text) +
                                                             1);//It increases the reserved memory by adding the size of the new string and 1 for the null terminator after setting the pointer to NULL
            char *temp_CurrentText = (char *) realloc(CurrentText, strlen(CurrentText) + strlen(text) +
                                                                   1);//The same as the line above since this is a copy
            if (temp_original == NULL || temp_CurrentText == NULL) {//To make sure the file exists
                free(original);//Since the previous allocation succeeded, we free it
                free(CurrentText);
                fclose(file);
                printf("Failed to allocate memory, there might not be enough memory to proceed with the program so it will terminate\n\n");
                return;
            }
            //Add the extra text read from the input file to previous text
            strcat(temp_original, text);
            strcat(temp_CurrentText, text);
            original = temp_original;
            CurrentText = temp_CurrentText;
        }
    }
    fclose(file);
    printf("\nFile loaded successfully\n\n");
}

/*using queue as a linked list
void enqueue(short operation, char *text, int index) {//the Text can be a word or a letter
    if (queueFront == NULL) {
        queueFront = createNode(operation, text, index);
        queueRear = queueFront;
    } else {
        queueRear->next = createNode(operation, text, index);
        queueRear = queueTail->next;
    }
}*/

void enqueue(short operation, char *text, int index) {
    // Allocate memory for the copy of the text to prevent overwriting
    char *wordToSend = malloc(strlen(text) + 1);
    if (wordToSend == NULL) {
        printf("There is not enough memory to proceed\n");
        return;
    }
    strcpy(wordToSend, text);

    //Check if the queue is full and handle accordingly
    if (emptySpot == -1 || (queue[frontOfQueue] != NULL && emptySpot == frontOfQueue)) {
        dequeue(1, frontOfQueue); // Make the first spot vacant if the queue is full
        if (emptySpot == -1) {
            emptySpot = initialFrontOfQueue; //Reset emptySpot if it was -1
        }
        frontOfQueue--; //Move frontOfQueue pointer back

        if (frontOfQueue < 0) {
            frontOfQueue = initialFrontOfQueue - 1; // Assuming circular queue
        }
    }
    // Create the new node and add it to the empty spot
    queue[emptySpot] = createNode(operation, wordToSend, index);

    // Update the emptySpot index
    emptySpot--;
}


stack_ptr createNode(short operation, char *text, int index) {
    stack_ptr new = (stack_ptr) malloc(sizeof(struct stack));
    new->word = text;
    new->operation = operation;
    new->index = index;
    new->next = NULL;
    return new;
}

void dequeue(short addSpace, int indexOfQueue) {
    Push(toUndo, queue[indexOfQueue]);
    //To perform the operation as instructed
    short yes = -1;//Anything but 0 and 1
    if (addSpace == 1)
        yes = 1;
    if (queue[indexOfQueue]->operation == INSERT) {
        if (addSpace == 0) {
            while (yes != 0 && yes != 1) {
                printf("Do you want to enter a space before the text provided\nEnter 1 if yes and 0 if no\n");
                scanf("%hd", &yes);
                while (getchar() !=
                       '\n');//To clear buffer
            }
        }
        if (yes == 1) {//To add a space before the word
            int length_word = (int) strlen(queue[indexOfQueue]->word);
            char temp[length_word + 2];
            strcat(strcpy(temp, " "), queue[indexOfQueue]->word);
            temp[length_word + 1] = '\0';
            strcpy(queue[indexOfQueue]->word, temp);
        }
        char *temp = (char *) realloc(CurrentText,
                                      strlen(CurrentText) + strlen(queue[indexOfQueue]->word) +
                                      1);//1 for the '\0' only
        CurrentText = temp;
        InsertToken(queue[indexOfQueue]);//To insert a token at a specific index
    } else {
        RemoveToken(queue[indexOfQueue]);//To remove the token based on an index or by default the first occurrence
    }
    queue[indexOfQueue] = NULL;//Make it empty
}

/*using queue as a linked list
void dequeue(short addSpace) {
    Push(toUndo, queueFront);
    //To perform the operation as instructed
    short yes = -1;//Anything but 0 and 1
    if (addSpace == 1)
        yes = 1;
    if (queueFront->operation == INSERT) {
        if (addSpace == 0) {
            while (yes != 0 && yes != 1) {
                printf("Do you want to enter a space before the text provided\nEnter 1 if yes and 0 if no\n");
                scanf("%hd", &yes);
                while (getchar() !=
                       '\n');//To clear buffer
            }
        }
        if (yes == 1) {//To add a space before the word
            int length_word = (int) strlen(queueFront->word);
            char temp[length_word + 2];
            strcat(strcpy(temp, " "), queueFront->word);
            temp[length_word + 1] = '\0';
            strcpy(queueFront->word, temp);
        }
        char *temp = (char *) realloc(CurrentText,
                                      strlen(CurrentText) + strlen(queueFront->word) +
                                      1);//1 for the '\0' only
        CurrentText = temp;
        InsertToken(queueFront);//To insert a token at a specific index
    } else {
        RemoveToken(queueFront);//To remove the token based on an index or by default the first occurrence
    }
    queueFront = NULL;//Make it point to NULL to make it empty
}*/

void Push(short To, stack_ptr nodeOfTheText) {
    if (To == toUndo) {//Make the inserted (pushed) node the top of the undo stack
        if (undo_stack == NULL) {
            undo_stack = nodeOfTheText;
        } else {
            nodeOfTheText->next = undo_stack;
            undo_stack = nodeOfTheText;//Make the pointer of the undo stack point to the new first node, its new top
        }
    } else {//Add to redo
        if (redo_stack == NULL) {
            redo_stack = nodeOfTheText;
        } else {
            nodeOfTheText->next = redo_stack;//Make the inserted (pushed) node the top of the redo stack
            redo_stack = nodeOfTheText;//Make the pointer of the redo stack point to the new first node, its new top
        }
    }
}

void Pop(short fromPrint, short To, stack_ptr nodeOfTheText) {
    if (To == toUndo) {
        if (undo_stack == NULL) {
            printf("Undo stack is empty\n");
            return;
        }
        nodeOfTheText = undo_stack;
        undo_stack = undo_stack->next;
        nodeOfTheText->next = NULL;
        if (fromPrint == 1) {//Pop called from printStack
            if (nodeOfTheText->operation == INSERT) {
                printf("_____________________________________________\n  |%s|\t\t |Insert|\t\t|%d|\n_____________________________________________\n",
                       nodeOfTheText->word, nodeOfTheText->index);
            } else {
                printf("_____________________________________________\n  |%s|\t\t |Remove|\t\t|%d|\n_____________________________________________\n",
                       nodeOfTheText->word, nodeOfTheText->index);
            }
        }
        Push(toRedo, nodeOfTheText);
        if (nodeOfTheText->operation == INSERT) {//Since it is Undo we perform the opposite operation
            RemoveToken(nodeOfTheText);
        } else
            InsertToken(nodeOfTheText);
    } else {
        if (redo_stack == NULL) {
            printf("Redo stack is empty\n");
            return;
        }
        nodeOfTheText = redo_stack;
        redo_stack = redo_stack->next;
        nodeOfTheText->next = NULL;
        if (fromPrint == 1) {//Pop called from printStack
            if (nodeOfTheText->operation == INSERT) {
                printf("_____________________________________________\n  |%s|\t\t |Insert|\t\t|%d|\n_____________________________________________\n",
                       nodeOfTheText->word, nodeOfTheText->index);
            } else {
                printf("_____________________________________________\n  |%s|\t\t |Remove|\t\t|%d|\n_____________________________________________\n",
                       nodeOfTheText->word, nodeOfTheText->index);
            }
        }
        Push(toUndo, nodeOfTheText);
        if (nodeOfTheText->operation == INSERT)//Since it is Redo we perform the same operation
            InsertToken(nodeOfTheText);
        else
            RemoveToken(nodeOfTheText);
    }
    printf("Node moved to the other stack\n");
}

void UpdateText(short addSpace) {
    if (frontOfQueue ==
        initialFrontOfQueue) {//If the queue hasn't been filled completely we dequeue starting from the front until the last entered node
        int until = frontOfQueue;
        while (until > emptySpot) {//We dequeue until the last element which is the one before the emptySpot
            dequeue(addSpace, until);
            until--;
        }
    } else {
        int until = frontOfQueue +
                    1;//If the front is not the initial front then the last node to dequeue is the one before the current front
        while (frontOfQueue >= 0) {
            dequeue(addSpace, frontOfQueue);
            frontOfQueue--;
            if (frontOfQueue == until)//It stops to dequeue here
                break;
            if (frontOfQueue ==
                -1)//We reached the end of the queue so we make it point to the initial front again so it dequeues the rest
                frontOfQueue = initialFrontOfQueue;
        }
    }
    frontOfQueue = initialFrontOfQueue;//Resetting the front since now the queue is empty
    emptySpot = initialFrontOfQueue;
}

void InsertToken(stack_ptr node) {
    if (node->index == -1)
        strcat(CurrentText, node->word);//We update the CurrentText
    else {
        int length = (int) strlen(CurrentText);
        int length_word = (int) strlen(node->word);
        char temp[length + length_word + 1];//The one is for the '\0'
        strncpy(temp, CurrentText,
                node->index);//Copy the part before the place to insert => [0 - (index + 1)] the 1 is for getting the index after the index from user which is the one before the location
        strcat(temp, node->word);
        strcat(temp, CurrentText +
                     node->index);//Add the rest of the text to the string after adding the word at the intended location
        char *reallocate = (char *) realloc(CurrentText, length + length_word + 1);
        if (reallocate == NULL) {
            free(CurrentText);
            printf("Failed to allocate memory; there might not be enough memory to proceed with the program so it will terminate\n\n");
            return;
        }
        CurrentText = reallocate;
        strcpy(CurrentText, temp);//Update the CurrentText
    }
}

void RemoveToken(stack_ptr node) {
    if (node->index == -1) {
        int length_word = (int) strlen(node->word);
        int length_Current = (int) strlen(CurrentText);
        CurrentText[length_Current - length_word] = '\0';
    } else {
        if (node->index == -2) {
            char *ptrToWhereTheWordIs = strstr(CurrentText,
                                               node->word); //Find the first occurrence of the string in text
            if (ptrToWhereTheWordIs == NULL) {//String not found in the CurrentText
                printf("Invalid. The text entered to be removed is not in the current text\n");
                node = undo_stack;
                undo_stack = undo_stack->next;
                node->next = NULL;
                free(node);//We consider it to be invalid and free the node
                return;
                //If false string is found
            } else
                node->index = (int) (ptrToWhereTheWordIs - CurrentText);//Make it point to where it was found}
        }
        int length_Current = (int) strlen(CurrentText);
        int length = (int) strlen(node->word);
        char temp[length_Current - length + 1];//1 for null terminator
        strncpy(temp, CurrentText,
                node->index);//Copy the part before the place to delete, we leave an extra space for the '\0'
        temp[node->index] = '\0';
        strcat(temp,
               CurrentText + node->index +
               length);//Add the rest of the text to temp after deleting the word at the intended location
        strcpy(CurrentText, temp
        );
    }//Update the CurrentText
    printf("'%s' got removed\n", node->word);
}

void linkedListPrint() {
    stack_ptr redoPrint = redo_stack, undoPrint = undo_stack;
    printf(" |Token|\t|Operation|\t|Index|\n");
    printf("(UNDO STACK)\n");
    while (undoPrint != NULL) {
        if (undoPrint->operation == INSERT) {
            printf("_____________________________________________\n  |%s|\t\t |Insert|\t\t|%d|\n_____________________________________________\n",
                   undoPrint->word, undoPrint->index);
        } else {
            printf("_____________________________________________\n  |%s|\t\t |Remove|\t\t|%d|\n_____________________________________________\n",
                   undoPrint->word, undoPrint->index);
        }
        undoPrint = undoPrint->next;
    }
    printf("(REDO STACK)\n");
    while (redoPrint != NULL) {
        if (redoPrint->operation == INSERT) {
            printf("_____________________________________________\n  |%s|\t\t |Insert|\t\t|%d|\n_____________________________________________\n",
                   redoPrint->word, redoPrint->index);
        } else {
            printf("_____________________________________________\n  |%s|\t\t |Remove|\t\t|%d|\n_____________________________________________\n",
                   redoPrint->word, redoPrint->index);
        }
        redoPrint = redoPrint->next;
    }
}

void printStack(short To) {
    short POP, recreate;
    int count = 0;
    if (To == toUndo) {//To know which stack
        POP = toUndo;//To pop this stack
        recreate = toRedo;//To pop the other after
        while (undo_stack != NULL) {
            count++;
            Pop(1, POP, undo_stack);
        }
        while (redo_stack != NULL && count != 0) {
            Pop(0, recreate, redo_stack);
            count--;
        }
    } else {
        POP = toRedo;
        recreate = toUndo;
        while (redo_stack != NULL) {
            Pop(1, POP, redo_stack);
            count++;
        }
        while (undo_stack != NULL && count != 0) {
            Pop(0, recreate, undo_stack);
            count--;
        }
    }
}

void FreeStack(stack_ptr stack) {
    while (stack != NULL) {
        stack_ptr temp = stack;//Pointing to the node that is currently the first
        stack = stack->next;//Make it point to the one connected to it
        free(temp->word);//Since the word was allocated in enqueue
        free(temp);//Free the node
    }
}

void saveToFile() {
    FILE *file = fopen("output.txt", "w");
    //There is no need to check if the file exists since it will automatically create one
    fprintf(file, "%s", CurrentText);
    fclose(file);
    printf("Text got saved to file output\n\n");
}











