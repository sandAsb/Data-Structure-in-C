#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_LENGTH 46 //since the longest word in english is 45 the extra 1 for the null terminator
#define LINE_LENGTH 210 //since the longest line of the smallest format in a txt file seemed to be able to contain 210
//there will be no need to check if the inputWords are not larger than the WORD_LENGTH since WORD_LENGTH is the length of the longest word in English
#define LEFT_HEAVY 0
#define RIGHT_HEAVY 1
#define true 1
#define false 0
#define toPrint 1
#define toFillHash 0
#define hashOf_CoMp 367 //the hash value of CoMp without %SIZE to increase security

typedef struct NodeOfTree *tree_ptr;
typedef struct hashNode *chain;
struct hashNode {
    char word[WORD_LENGTH];
    int frequency;
    chain next;
};
struct NodeOfTree {
    chain word_frequency;
    int height;
    tree_ptr left, right;
};
tree_ptr AVLtree = NULL;
char *textFromFile = NULL;
int SIZE = 0, numOfElements = 0;
chain *table = NULL;//an array of pointers of struct hashNode

void loadData(FILE *input);

void createTree();

int isAlphabet(char c);

tree_ptr createNode(char *word);

tree_ptr addToTree_thenBalance(char *word, tree_ptr tree);

int getHeight(tree_ptr treeNode);

tree_ptr Balance(tree_ptr treeRoot, short type, char *word);

tree_ptr Find(char *toFind, tree_ptr treeRoot);

tree_ptr FindMin(tree_ptr treeRoot);

int Max(int lhs, int rhs);

tree_ptr SingleRotateWithRight(tree_ptr unbalanceNode);

tree_ptr SingleRotateWithLeft(tree_ptr unbalanceNode);

tree_ptr DoubleRotateWithLeft(tree_ptr unbalancedNode);

tree_ptr DoubleRotateWithRight(tree_ptr unbalancedNode);

tree_ptr deleteNodeFromTree(tree_ptr root, char *wordToDelete);

void InOrder(tree_ptr t, short type);

tree_ptr MakeEmpty(tree_ptr tree);

void hashTableSize();

unsigned int Hashing(char *word);

void createHashTable();

void insertToTable(chain node);

void chaining(chain node, unsigned int index);

chain FindInChains(int index, char *word);

void printHash();

void moreThan(int conditionNum);

void unique();

void mostFrequented();

void addingToHash(char *word);

void deleteFromHash(char *word, char *type);

int main() {
    short choice;
    FILE *input;
    char typedWord[WORD_LENGTH];
    while (1) {
        printf("\n1. Load data from the file.\n"
               "2. Create the AVL tree.\n"
               "3. Insert a word to the AVL tree.\n"
               "4. Delete a word from the AVL tree.\n"
               "5. Print the words as sorted in the AVL tree.\n"
               "6. Create the Hash Table.\n"
               "7. Insert a word to the Hash table.\n"
               "8. Delete a word from the hash table.\n"
               "9. Search for a word in the hash table and print its frequency.\n"
               "10. Print words statistics.\n"
               "11. Exit the application.\n"
               "12. Print from hash table\n\n");
        while (scanf("%hd", &choice) != 1) {
            printf("\nInvalid input. choose a number from the menu\n");
            while (getchar() !=
                   '\n');//To remove the scanned invalid input from the buffer so it doesn't result in an infinite loop if scanf was not successful
        }
        switch (choice) {
            case 1:
                input = fopen("input.txt", "r");
                if (input == NULL) {//To make sure the file exists
                    printf("Failed to open the file, check if it really exists, then run the program again\n\n");
                    return 0;
                }
                loadData(input);
                if (textFromFile == NULL) {
                    return 0;
                }
                fclose(input);
                break;
            case 2:
                createTree();
                break;
            case 3:
                printf("Type the word to insert. ((It must consist of only alphabets))\n");
                scanf("%s", typedWord);
                int length = (short) strlen(typedWord);
                int i = 0;
                while (i < length) {//is it a valid input?
                    if (isAlphabet(typedWord[i]))
                        i++;
                    else {
                        printf("invalid input. Enter a word that only has alphabets after choosing '2' again!\n");
                        break;
                    }
                }
                //if valid insert
                AVLtree = addToTree_thenBalance(typedWord,
                                                AVLtree);//assigning the value to AVLtree is not necessary
                break;

            case 4:
                printf("Type the word to delete.\n");
                scanf("%s", typedWord);
                printf("Do you want to delete the whole node or reduce its frequency\nyes to delete it whole or no?\n");
                char answer[4];
                scanf("%s", answer);
                if (strcasecmp(answer, "yes") == 0) {
                    deleteNodeFromTree(AVLtree, typedWord);
                } else {
                    tree_ptr found = Find(typedWord, AVLtree);
                    if (found != NULL) {
                        found->word_frequency->frequency--;
                        if (found->word_frequency->frequency == 0)
                            deleteNodeFromTree(AVLtree, typedWord);
                    } else
                        printf("There is no node with the entered word. You can only choose nodes existing in the tree.\n");
                }
                break;
            case 5:
                InOrder(AVLtree, toPrint);
                break;
            case 6:
                if (AVLtree == NULL) {
                    printf("The AVL tree has not got created, it will be created automatically in order to create the hash table\n");
                    createTree();
                }
                createHashTable();
                break;
            case 7:
                printf("Type the word to insert. ((It must consist of only alphabets))\n");
                scanf("%s", typedWord);
                length = (short) strlen(typedWord);
                i = 0;
                while (i < length) {//is it a valid input?
                    if (isAlphabet(typedWord[i]))
                        i++;
                    else {
                        printf("invalid input. Enter a word that only has alphabets after choosing '7' again!\n");
                        break;
                    }
                }
                addingToHash(typedWord);
                break;
            case 8:
                printf("Type the word to delete.\n");
                scanf("%s", typedWord);
                printf("Do you want to delete the whole node or reduce its frequency\nyes to delete it whole or no?\n");
                scanf("%s", answer);
                deleteFromHash(typedWord, answer);
                break;
            case 9:
                printf("Enter the word\n");
                scanf("%s", typedWord);
                unsigned int index = Hashing(typedWord);
                if (table[index] != NULL) {
                    if (strcmp(table[index]->word, typedWord) != 0) {
                        chain found = FindInChains((int) index, typedWord);
                        if (found == NULL)
                            printf("There is no such word in the hash table\n");
                        else printf("Word %s has a frequency of %d\n", typedWord, found->frequency);
                    } else printf("Word %s has a frequency of %d\n", typedWord, table[index]->frequency);
                } else printf("There is no such word in the hash table\n");
                break;
            case 10:
                printf("Choose from the following:-\n1) Total number of unique words\n2) Print the number of words and the words with a frequency greater than  number you choose\n3) The most frequented word\n");
                short option;
                while (scanf("%hd", &option) != 1) {
                    if (!(option > 0 && option < 4))
                        printf("Invalid option. Choose from the list\n");
                    while (getchar() !=
                           '\n');
                }
                if (option == 1) unique();
                else if (option == 2) {
                    printf("Type the number\n");
                    while (scanf("%hd", &option) != 1) {
                        if (option < 0)
                            printf("Invalid for a frequency\n");
                        while (getchar() !=
                               '\n');
                    }
                    moreThan(option);
                    option = 0;
                } else mostFrequented();
                break;
            case 12:
                printHash();
                break;
            case 11:
                MakeEmpty(AVLtree);
                return 0;
            default:
                printf("\nThere is no such choice. Choose a number from the menu\n");
                break;
        }
    }

}

void loadData(FILE *input) {
    char current[LINE_LENGTH];
    int prevSize = 0;
    textFromFile = (char *) malloc(1);//as an initialisation
    if (textFromFile == NULL) {
        printf("Not enough space. The program will stop\n");
        return;
    }
    while (fgets(current, LINE_LENGTH, input) != NULL) {//reads line by line from the file
        textFromFile = (char *) realloc(textFromFile, (strlen(current)) + 1 + prevSize);
        if (textFromFile == NULL) {
            printf("Not enough space. The program will stop\n");
            return;
        }
        strcat(textFromFile, current);//to append the string
        prevSize += (int) strlen(current);
    }
}

void createTree() {
    char *token = strtok(textFromFile, " ");
    while (token != NULL) {
        int i = 0, j, until = (short) strlen(token);
        while (i <
               until) {//this is good for handling cases where token has non-alphabetical characters at the middle, so it divides into more than one word depending on the chars
            int k = i;//the k is important for cases were a non-alphabetical character is at the middle of a word in order to know where to split
            while (isAlphabet(token[i])) {
                i++;
            }
            j = i - k;
            char validWord[j + 1];//the +1 for the \0
            validWord[j] = '\0';//null terminate the word
            if (j != 0) {
                AVLtree = addToTree_thenBalance(strncpy(validWord, token + k, j), AVLtree);
            } else { i++; }//if the beginning of the text is not an alphabet since it won't enter the while loop to update i
        }
        token = strtok(NULL, " ");
    }
}

int isAlphabet(char c) {
    return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') ? 1 : 0;
}


tree_ptr addToTree_thenBalance(char *word, tree_ptr treeRoot) {
    //returns the pointer to the root node after inserting the new node
    if (treeRoot == NULL) //if the tree pointer is not pointing to anything
        treeRoot = createNode(word);
        //find the right location to add the node
    else if (strcasecmp(word, treeRoot->word_frequency->word) < 0) {
        treeRoot->left = addToTree_thenBalance(word, treeRoot->left);
        if (getHeight(treeRoot->left) - getHeight(treeRoot->right) > 1)
            treeRoot = Balance(treeRoot, LEFT_HEAVY, word);
    } else if (strcasecmp(word, treeRoot->word_frequency->word) > 0) {
        treeRoot->right = addToTree_thenBalance(word, treeRoot->right);
        if (getHeight(treeRoot->right) - getHeight(treeRoot->left) > 1)
            treeRoot = Balance(treeRoot, RIGHT_HEAVY, word);
    } else {
        treeRoot->word_frequency->frequency++;
        return treeRoot;//there are no changes to the balance so the height stays the same
    }
    treeRoot->height = Max(getHeight(treeRoot->left), getHeight(treeRoot->right)) + 1;
    return treeRoot;
}

tree_ptr createNode(char *word) {
    tree_ptr tree = (tree_ptr) malloc(sizeof(struct NodeOfTree));
    chain node = (chain) malloc(sizeof(struct hashNode));
    if (tree == NULL || node == NULL) {
        printf("Out of space.\n");
        return NULL;
    } else {//create node after it finds the rightful place for it
        strcpy(node->word, word);
        tree->height = 0;// 0 means it has no children
        node->frequency = 1;
        tree->left = tree->right = NULL;
        node->next = NULL;
        tree->word_frequency = node;
    }
    numOfElements++;
    return tree;
}

tree_ptr Balance(tree_ptr treeRoot, short type, char *word) {
    if (type == LEFT_HEAVY) {
        if (strcasecmp(word, treeRoot->left->word_frequency->word) < 0)
            treeRoot = SingleRotateWithLeft(treeRoot);
        else
            treeRoot = DoubleRotateWithLeft(treeRoot);
    } else if (type == RIGHT_HEAVY) {
        if (strcasecmp(word, treeRoot->right->word_frequency->word) > 0)
            treeRoot = SingleRotateWithRight(treeRoot);
        else
            treeRoot = DoubleRotateWithRight(treeRoot);
    }
    return treeRoot;
}

int getHeight(tree_ptr treeNode) {
    if (treeNode == NULL)
        return -1;
    else
        return treeNode->height;
}

tree_ptr Find(char *toFind, tree_ptr treeRoot) {
    if (treeRoot == NULL)
        return NULL;
    if (strcasecmp(toFind, treeRoot->word_frequency->word) < 0)
        return Find(toFind, treeRoot->left);
    else if (strcasecmp(toFind, treeRoot->word_frequency->word) > 0)
        return Find(toFind, treeRoot->right);
    else
        return treeRoot;
}

tree_ptr FindMin(tree_ptr treeRoot) {
    if (treeRoot == NULL)
        return NULL;
    else if (treeRoot->left == NULL)
        return treeRoot;
    else
        return FindMin(treeRoot->left);
}

int Max(int lhs, int rhs) {
    return lhs > rhs ? lhs : rhs;
}

//to balance in cases left-left
/*                   unbalanceNode
 *                  /
 *           newRoot
 *          /
 *      left
 * */
tree_ptr SingleRotateWithLeft(tree_ptr unbalanceNode) {
    tree_ptr newRoot = unbalanceNode->left;
    unbalanceNode->left = newRoot->right;//in case the root already has a right node we attach it to the left of the original root
    newRoot->right = unbalanceNode;
//the third node stays as the second's left which became the new root
    unbalanceNode->height = Max(getHeight(unbalanceNode->left), getHeight(unbalanceNode->right)) + 1;
    newRoot->height = Max(getHeight(newRoot->left), unbalanceNode->height) + 1;
//+1 is added because the height of the node itself is one more than the maximum height of its children (if they exist)
    return newRoot;
}

//to balance in cases right-right
/* unbalanceNode
 *             \
 *              newRoot
 *                      \
 *                       right
 * */
tree_ptr SingleRotateWithRight(tree_ptr unbalanceNode) {
    tree_ptr newRoot = unbalanceNode->right;
    unbalanceNode->right = newRoot->left;//in case the root already has a left node we attach it to the right of the original root
    newRoot->left = unbalanceNode;
//the third node stays as the second's right which became the new root
    unbalanceNode->height = Max(getHeight(unbalanceNode->left), getHeight(unbalanceNode->right)) + 1;
    newRoot->height = Max(getHeight(newRoot->left), unbalanceNode->height) + 1;
//+1 is added because the height of the node itself is one more than the maximum height of its children (if they exist)
    return newRoot;
}

tree_ptr DoubleRotateWithLeft(tree_ptr unbalancedNode) {    //to make it left-left
    unbalancedNode->left = SingleRotateWithRight(unbalancedNode->left);
    return SingleRotateWithLeft(unbalancedNode);
}

tree_ptr DoubleRotateWithRight(tree_ptr unbalancedNode) {
    //to make right-right
    unbalancedNode->right = SingleRotateWithLeft(unbalancedNode->right);
    return SingleRotateWithRight(unbalancedNode);
}

tree_ptr deleteNodeFromTree(tree_ptr treeRoot, char *word) {
    if (treeRoot == NULL) {//means it was not found
        printf("There is no node with the entered word. You can only delete nodes existing in the tree.\n");
        return treeRoot;
    }
    if (strcasecmp(word, treeRoot->word_frequency->word) < 0) {
        treeRoot->left = deleteNodeFromTree(treeRoot->left, word);
    } else if (strcasecmp(word, treeRoot->word_frequency->word) > 0) {
        treeRoot->right = deleteNodeFromTree(treeRoot->right, word);
    } else {
        if (treeRoot->left == NULL) {
            tree_ptr temp = treeRoot->right;
            free(treeRoot);
            return temp;
        } else if (treeRoot->right == NULL) {
            tree_ptr temp = treeRoot->left;
            free(treeRoot);
            return temp;
        }
        tree_ptr temp = FindMin(treeRoot->right);
        strcpy(treeRoot->word_frequency->word, temp->word_frequency->word);
        treeRoot->word_frequency->frequency = temp->word_frequency->frequency;
        treeRoot->right = deleteNodeFromTree(treeRoot->right, temp->word_frequency->word);
    }
    treeRoot->height = Max(getHeight(treeRoot->left), getHeight(treeRoot->right)) + 1;
    int balance = getHeight(treeRoot->left) - getHeight(treeRoot->right);
    if (balance > 1) {
        treeRoot = Balance(treeRoot, LEFT_HEAVY, word);
    } else if (balance < -1) {
        treeRoot = Balance(treeRoot, RIGHT_HEAVY, word);
    }
    return treeRoot;
}

void InOrder(tree_ptr t, short type) {
    if (t != NULL) {
        InOrder(t->left, type);
        if (type == toPrint)
            printf("%s, %d times\n", t->word_frequency->word, t->word_frequency->frequency);
        else insertToTable(t->word_frequency);
        InOrder(t->right, type);
    }
}

tree_ptr MakeEmpty(tree_ptr tree) {
    if (tree != NULL) {
        MakeEmpty(tree->left);
        MakeEmpty(tree->right);
        free(tree);
    }
    return NULL;
}

unsigned int Hashing(char *word) {
    unsigned int hash_value = 0;
    while (*word != '\0') {
        hash_value += *word++;
    }
    return (hashOf_CoMp + hash_value) % SIZE;
}

//O(sqrt(N))
short isPrime(int num) {
    if (num <= 1)
        return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

void hashTableSize() {
    int size = numOfElements + numOfElements / 2;
    if (!isPrime(size)) {
        do size++;
        while (isPrime(size) == 0);
    }
    SIZE = size;
}

void chaining(chain node, unsigned int index) {
    //insert at the beginning since it takes less time complexity
    if (table[index] == NULL)
        table[index] = node;
    else {
        node->next = table[index];
        table[index] = node;
    }
}

chain FindInChains(int index, char *word) {
    // Ensure `table[index]` is not NULL before trying to access `table[index]->next`
    if (table[index] == NULL) {
        return NULL;
    }

    chain current = table[index]->next; // Start searching from the second node
    while (current != NULL) {
        if (strcmp(word, current->word) == 0) {
            return current; // Return the node if the word matches
        }
        current = current->next;
    }
    return NULL; // Return NULL if the word is not found
}


void createHashTable() {
    hashTableSize();
    table = (chain *) malloc(SIZE * sizeof(chain));//creates a global array of pointers pointing to struct hashNode
    for (int i = 0; i < SIZE; i++)
        table[i] = NULL;
    InOrder(AVLtree, toFillHash);
}

void insertToTable(chain node) {
    unsigned int index = Hashing(node->word);
    if (table[index] == NULL) {
        table[index] = node;
    } else chaining(node, index);
}

void printHash() {
    for (int i = 0; i < SIZE; i++) {
        if (table[i] != NULL) {
            printf("word: %s, frequency: %d\n", table[i]->word, table[i]->frequency);
            chain current = table[i]->next;
            while (current != NULL) {
                printf("word: %s, frequency: %d\n", current->word, current->frequency);
                current = current->next;
            }
        }
    }
}

void unique() {
    int unique = 0;
    chain current;
    for (int i = 0; i < SIZE; i++) {
        current = table[i];
        while (current != NULL) {
            if (current->frequency == 1)
                unique++;
            current = current->next;
        }
    }
    printf("The number of unique words is: %d\n", unique);
}

void moreThan(int conditionNum) {
    int moreThan = 0;
    chain tableI;
    for (int i = 0; i < SIZE; i++) {
        tableI = table[i];
        while (tableI != NULL) {
            if (tableI->frequency > conditionNum) {
                printf("word: %s, frequency: %d\n", tableI->word, tableI->frequency);
                moreThan++;
            }
            tableI = tableI->next;
        }
    }
    printf("Number of words with frequency greater than %d: (%d)\n", conditionNum, moreThan);
}

void mostFrequented() {
    int max = table[0]->frequency;
    chain tableI;
    for (int i = 1; i < SIZE; i++) {
        tableI = table[i];
        while (tableI != NULL) {
            if (tableI->frequency > max)
                max = tableI->frequency;
            tableI = tableI->next;
        }
    }
    for (int i = 0; i < SIZE; i++) {
        tableI = table[i];
        while (tableI != NULL) {
            if (tableI->frequency == max)
                printf("The most frequented word is: %s, with frequency of %d\n", tableI->word, max);
            tableI = tableI->next;
        }
    }
}

void addingToHash(char *word) {
    unsigned int index = Hashing(word);
    chain current = table[index];
    if (current != NULL) {
        if (strcmp(current->word, word) == 0) { printf("a\n"); } else current = FindInChains((int) index, word);
        if (current != NULL) {
            current->frequency++;
            printf("It is already in the table, so the frequency got increased by one\n");
            return;
        }
    }
    chain new = (chain) malloc(sizeof(struct hashNode));
    if (new == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    strcpy(new
                   ->word, word);
    new->
            frequency = 1;
    new->
            next = NULL;
    insertToTable(new);
}

void deleteFromHash(char *word, char *type) {
    unsigned int index = Hashing(word);
    short frequencyReducing = 0;

    if (strcasecmp(type, "no") == 0)
        frequencyReducing = 1;

    chain current = table[index];
    chain prev = NULL;
    FindInChains((int) index, word);

    //If the word was not found in the hash table
    if (current == NULL) {
        printf("Word was not found\n");
        return;
    }

    //If frequency reduction is required, decrement frequency
    if (frequencyReducing == 1) {
        current->frequency--;

        if (current->frequency == 0) {
            frequencyReducing = 0;
        } else {
            return; //Exit if only frequency reduction was needed
        }
    }
    //If frequencyReducing is 0, delete the node from the linked list
    if (frequencyReducing == 0) {
        // If current is the head of the list
        if (prev == NULL) {
            table[index] = current->next; //Update head of the list
        } else {
            prev->next = current->next;
        }
        printf("Word deleted from hash. It will be deleted from the tree as well if it is there\n");
        //Remove from AVL tree
        deleteNodeFromTree(AVLtree, word);
    }
}