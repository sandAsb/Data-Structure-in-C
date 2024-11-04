#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN_S 50
#define forTheCopy 2
#define forTheOriginal 1//or backwards
typedef struct DIS *d_ptr;
typedef struct towns *t_ptr;
struct towns {
    char town[MAX_LEN_S + 1];
    int population;
    t_ptr nxt, prv;
};
struct DIS {
    d_ptr next, prev;
    int popul_dis, max_popul, min_popul;
    char district[MAX_LEN_S + 1];
    t_ptr t;//pointing to a linked list of towns
};
struct sort {
    char l;
    d_ptr dis, tail;
};
d_ptr original_head = NULL, copy = NULL, original_tail = NULL, tail = NULL, d_sorted = NULL, t_sorted = NULL, sorted = NULL;
char *max_name = NULL, *min_name = NULL;

void insertDIS(int type, char dis[]);

void loading_input_file();

void districtSorting();

void toNextDis(d_ptr dis);

unsigned short NumOfLettersOfLongestWord(d_ptr d);

d_ptr find(char name[]);

void insertTowns(t_ptr *t, char name[], int population);

void printDistricts(d_ptr ptr);

void printTowns(int type, d_ptr ptr);

char *fix(char *str);

void deleteTown(t_ptr delete, char town_name[]);

void DeleteDistrict(char dis_name[]);

t_ptr createTown(char town[], int population);

void PopulationSort(t_ptr *head);

void populationOfPalestine();

void createDistrict(int type, char dis[]);

void printTotal();

void printOrigin();

void save();

int total(d_ptr d);

void freeAll(d_ptr TAIL);

int main() {
    int choice, scan_popul;
    char scan_district[50], scan_town[50];
    d_ptr findIt = NULL;
    while (1) {
        printf("\n-------------------------------------------------------------------------\n1. Load the input file (Palestinian districts and their town with population).\n"
               "2. Print the loaded information before sorting (as in the input file format).\n"
               "3. Sort the districts alphabetically using Radix sort.\n"
               "4. Sort the towns for each district based on population in ascending order.\n"
               "5. Print the sorted information.\n"
               "6. Add a new district to the list of sorted districts (and sort the list)\n"
               "7. Add a new town to a certain district.\n"
               "8. Delete a town from a specific district.\n"
               "9. Delete a complete district; here, you must delete all towns belonging to that district\n"
               "before deleting it.\n"
               "10.Calculate the population of Palestine, the max and min town population.\n"
               "11.Print the districts and their total population (without towns details).\n"
               "12.Change the population of a town, after prompting the user to enter the district name.\n"
               "13.Save to output file.\n"
               "15.Reset (delete all changes). **Note: You cannot reset more than once**\n"
               "14.Exit\n");
        scanf("%d", &choice);
        if (choice == 14) {
            freeAll(tail);
            freeAll(original_tail);
            printf("\nExiting the program...\nThank you for using my program");
            break;
        }
        while (getchar() != '\n');
        switch (choice) {
            case 1:
                loading_input_file();
                printf("\nFile got loaded\n\n");
                break;
            case 2:
                printOrigin();
                break;
            case 3:
                districtSorting();
                if (t_sorted != NULL)
                    sorted = copy;
                break;
            case 4:
                toNextDis(copy);
                if (d_sorted != NULL)
                    sorted = copy;
                break;
            case 5:
                if (sorted != NULL || d_sorted != NULL || t_sorted != NULL)
                    printDistricts(copy);//it prints the last updated list
                else {
                    printf("Since the type of sorting is not specified, it will be sorted according to the population\n");
                    toNextDis(copy);
                    printDistricts(copy);
                }
                break;
            case 6:
                printf("Enter the name\n");
                scanf("%49[^\n]", scan_district);
                insertDIS(forTheCopy, scan_district);
                districtSorting();
                break;
            case 7:
                printf("Enter the name of the district containing the town\nThen the town to add and its population\n");
                scanf("%49[^\n]", scan_district);
                while (getchar() != '\n');
                scanf("%49[^\n]", scan_town);
                while (getchar() != '\n');
                scanf("%d", &scan_popul);
                findIt = find(scan_district);
                if (findIt == NULL)
                    printf("There is no district with this name\n");
                else {
                    insertTowns(&(findIt->t), scan_town, scan_popul);
                    printf("The town got added\n");
                }
                t_sorted = NULL;//it is no loner sorted based on population
                break;
            case 8:
                printf("Enter the name of the district containing the town to be deleted\nThen enter the name of the town\n");
                scanf("%49[^\n]", scan_district);
                while (getchar() != '\n');
                scanf("%49[^\n]", scan_town);
                findIt = find(scan_district);
                if (findIt == NULL)
                    printf("There is no district with this name\n");
                else {
                    deleteTown((findIt->t), scan_town);
                }
                break;
            case 9:
                printf("enter the name of the district\n");
                scanf("%49[^\n]", scan_district);
                DeleteDistrict(scan_district);
                break;
            case 10:
                populationOfPalestine();
                break;
            case 11:
                printTotal();
                break;
            case 12:
                printf("Type the name of the district of the town\nAlso, enter the town which population is to be changed and its new population\n");
                scanf("%49[^\n]", scan_district);
                while (getchar() != '\n');
                scanf("%49[^\n]", scan_town);
                while (getchar() != '\n');
                scanf("%d", &scan_popul);
                findIt = find(scan_district);
                if (findIt == NULL) {
                    printf("There is no such district\n");
                    break;
                }
                strcpy(scan_town, fix(scan_town));
                t_ptr currentTown = findIt->t;
                while (currentTown != NULL && strcmp(currentTown->town, scan_town) != 0) {
                    currentTown = currentTown->nxt;
                }
                if (currentTown == NULL)
                    printf("District not found or no towns in the district.\n");
                else currentTown->population = scan_popul;
                t_sorted = NULL;
                break;
            case 13:
                save();
                break;
            case 15:
                if (copy == original_head) {
                    printf("Resetting cannot happen more than once and it had already been used once before.\n");
                } else {
                    copy = original_head;
                    tail = original_tail;
                    printf("All changes got cancelled\n");
                }
                break;
            default:
                printf("There is no such option");
                break;
        }
    }
    return 0;
}

//It actually creates and inserts districts when used in the loading function
void createDistrict(int type, char districtName[]) {
    d_ptr newDistrict = (d_ptr) malloc(sizeof(struct DIS));
    if (newDistrict == NULL) {
        printf("Memory allocation error for district.\n");
        return;
    }
    strcpy(newDistrict->district, districtName);
    newDistrict->t = NULL;
    newDistrict->next = NULL;
    newDistrict->prev = NULL;
    newDistrict->popul_dis = 0;
    newDistrict->max_popul = 0;
    newDistrict->min_popul = 0;
    if (type == forTheCopy) {
        copy = newDistrict;
        tail = newDistrict;
        //to place the tail at the end since the new node is inserted at the end

    } else {
        original_head = newDistrict;
        original_tail = newDistrict;//to place the tail at the end since the new node is inserted at the end
    }
}

void insertDIS(int type, char dis[]) {
    d_ptr insert = (d_ptr) malloc(sizeof(struct DIS));
    dis = fix(dis);
    if (insert == NULL) {
        printf("Not enough memory\n");
        return;
    } else {
        strcpy(insert->district, dis);
        insert->prev = NULL;
        insert->t = NULL;
        insert->popul_dis = 0;
        insert->max_popul = 0;
        insert->min_popul = 0;
        max_name = NULL;//when the content of the list is changed they are no longer accurate
        min_name = NULL;
        if (type == forTheOriginal) {//insert at the beginning
            insert->next = original_head;
            original_head->prev = insert;
            original_head = insert;
        } else {
            insert->next = copy;
            copy->prev = insert;
            copy = insert;
        }
    }
}

t_ptr createTown(char town[], int population) {
    t_ptr new = (t_ptr) malloc(sizeof(struct towns));
    if (new == NULL) {
        printf("No memory left\n");
        return NULL;
    } else {
        strcpy(new->town, town);
        new->population = population;
        new->nxt = NULL;
        new->prv = NULL;
        return new;
    }
}

//to capitalize the first letter and change the rest to lowercase
char *fix(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (i == 0 || str[i - 1] == ' ')
            str[i] = toupper(str[i]);
        else
            str[i] = tolower(str[i]);
    }
    return str;
}

d_ptr find(char name[]) {
    strcpy(name, fix(name));
    d_ptr finder = copy;
    while (finder != NULL && strcmp(finder->district, name) != 0)
        finder = finder->next;
    if (finder != NULL) {
        finder->popul_dis = 0;//find is used in functions to add, delete or change the population
        finder->max_popul = 0;//so the total changes
        finder->min_popul = 0;
    }
    return finder;
}

//adds a town to a linked list of towns in a certain district
//the passed pointer is the pointer of the linked list of towns from a certain district
//if it receives 'finder' then it is updating the copy
//if it receives DIS->t then it is creating a linked list while loading the file
void insertTowns(t_ptr *t, char name[],
                 int population) {//we use *t which is a pointer to the pointer in order to modify the original linked list
    t_ptr new = createTown(name, population);
    if (new == NULL) {
        printf("Creating a town failed\n");
        return;
    } else {
        if ((*t) == NULL) {
            (*t) = new;
        } else {
            new->nxt = *t;
            (*t)->prv = new;
            *t = new;
        }
        max_name = NULL;
        min_name = NULL;
    }
}

//create the list and a clone so we don't lose the original
void loading_input_file() {
    FILE *file = fopen("districts.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file, check if it really exists\n");
        return;
    }
    char line[(MAX_LEN_S * 3) + 3];
    char district_name[50], town_name[50];
    int population, i = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        // to remove newline character(\n) after each line in file districts
        line[strcspn(line, "\n")] = '\0';
        strcpy(district_name, fix(strtok(line, "|")));
        strcpy(town_name, fix(strtok(NULL, "|")));
        population = atoi(strtok(NULL, "|"));
        //to remove the space if not between two words
        if (district_name[0] == ' ') {
            while (district_name[i + 1] != '\0') {
                district_name[i] = district_name[i + 1];
                i++;
            }
            district_name[strlen(district_name) - 2] = '\0';
        } else {
            if (district_name[strlen(district_name) - 1] == ' ')
                district_name[strlen(district_name) - 1] = '\0';
        }
        i = 0;
        if (town_name[0] == ' ') {
            while (town_name[i + 1] != '\0') {
                town_name[i] = town_name[i + 1];
                i++;
            }
            town_name[strlen(town_name) - 2] = '\0';
        } else {
            if (town_name[strlen(town_name) - 1] == ' ')
                town_name[strlen(town_name) - 1] = '\0';
        }
        //for the towns that share the same district (check if the district is the same)
        if (original_head == NULL) {
            createDistrict(forTheOriginal, district_name);
            createDistrict(forTheCopy,
                           district_name);//the copy is the list that will get edited throughout using the program
            insertTowns(&(original_head->t), town_name, population);
            insertTowns(&(copy->t), town_name, population);
        } else if (strcmp(original_head->district, district_name) != 0) {
            insertDIS(forTheOriginal, district_name);
            insertDIS(forTheCopy, district_name);
            insertTowns(&(original_head->t), town_name, population);
            insertTowns(&(copy->t), town_name, population);
        } else {
            insertTowns(&(original_head->t), town_name, population);
            insertTowns(&(copy->t), town_name, population);
        }
    }
    fclose(file);
}

void PopulationSort(t_ptr *head) {
    t_ptr current = *head;
    if (current == NULL || current->nxt == NULL) // If it is empty or has only one node, no sorting needed
        return;
    t_ptr popul_sorted = NULL, next_town = NULL;
    //popul_sorted is a dummy head to the list of towns which is affected by the sorting instead of the original before reaching the final sorted list
    while (current != NULL) {
        next_town = current->nxt;
        if (popul_sorted == NULL || current->population < popul_sorted->population) {
            current->nxt = popul_sorted;
            popul_sorted = current;
        } else {
            t_ptr temp = popul_sorted;
            while (temp->nxt != NULL && temp->nxt->population <
                                        current->population) {//stops when it reaches the town with a population larger than th current town's
                temp = temp->nxt;
            }
            current->nxt = temp->nxt;
            temp->nxt = current;
        }
        current = next_town;
    }
    *head = popul_sorted;
}

void toNextDis(d_ptr dis) {
    d_ptr current_district = dis;
    while (current_district != NULL) {
        PopulationSort(&(current_district->t));
        current_district = current_district->next;
    }
    t_sorted = copy;
}

void districtSorting() {
    int num = NumOfLettersOfLongestWord(copy);
    struct sort letters[28] = {0}; // Array for buckets A-Z, space, and null
    d_ptr sorter = copy;
    d_ptr temp = NULL;
    //Initializing each letter bucket's character (A-Z, space, null)
    for (char c = 'A'; c <= 'Z'; c++) {
        letters[c - 'A'].l = c;
    }
    letters[26].l = ' ';
    letters[27].l = '\0';

    //Sorting based on each character position from last to first
    for (int i = num - 1; i >= 0; i--) {
        sorter = copy;
        //to clear each bucket list
        for (int j = 0; j < 28; j++) {
            letters[j].dis = NULL;
            letters[j].tail = NULL;
        }
        //Distributing nodes into buckets based on i-th character
        while (sorter != NULL) {
            temp = sorter->next;
            sorter->next = NULL;
            sorter->prev = NULL;

            char CurrentLetter = toupper(sorter->district[i]);
            int option;

            if (CurrentLetter >= 'A' && CurrentLetter <= 'Z') {
                option = CurrentLetter - 'A';
            } else if (CurrentLetter == ' ') {
                option = 26;
            } else {
                option = 27;
            }

            if (letters[option].dis == NULL) {
                letters[option].dis = sorter;
                letters[option].tail = sorter;
            } else {
                letters[option].tail->next = sorter;
                sorter->prev = letters[option].tail;
                letters[option].tail = sorter;
            }
            sorter = temp;
        }
        //Merging all buckets into a single sorted list
        d_ptr sort_head = NULL;
        d_ptr sort_tail = NULL;

        for (int option = 0; option < 28; option++) {
            if (letters[option].dis != NULL) {
                if (sort_head == NULL) {
                    sort_head = letters[option].dis;
                    sort_tail = letters[option].tail;
                } else {
                    sort_tail->next = letters[option].dis;
                    letters[option].dis->prev = sort_tail;
                    sort_tail = letters[option].tail;
                }
            }
        }
        copy = sort_head;
    }
    d_sorted = copy;
}

void DeleteDistrict(char dis_name[]) {
    d_ptr toDelete = find(dis_name);
    if (toDelete == NULL) {
        printf("District not found.\n");
        return;
    }
    t_ptr town_deleter = toDelete->t;
    while (town_deleter != NULL) {
        t_ptr next_town = town_deleter->nxt;
        deleteTown(toDelete->t, town_deleter->town);
        town_deleter = next_town;  // Move to the next town
    }
    if (toDelete->prev != NULL)
        toDelete->prev->next = toDelete->next;
    if (toDelete->next != NULL)
        toDelete->next->prev = toDelete->prev;
    if (toDelete == original_head)
        original_head = toDelete->next;
    if (toDelete == copy)
        copy = toDelete->next;

    //Free memory allocated for the district node
    free(toDelete);
}

unsigned short NumOfLettersOfLongestWord(d_ptr d) {
    d_ptr helper = d;
    unsigned short numOfLetters = 0;
    char max[MAX_LEN_S + 1];
    strcpy(max, helper->district);
    while (helper != NULL) {
        if (strlen(helper->district) >= numOfLetters) {
            numOfLetters = strlen(helper->district);
            strcpy(max, helper->district);
        }
        helper = helper->next;
    }
    numOfLetters = (unsigned short) strlen(max);
    return numOfLetters;
}

void printDistricts(d_ptr ptr) {
    d_ptr printer = ptr;
    if (ptr->next == NULL) {
        while (printer != NULL) {
            printf("%s District, Population = %d\n", printer->district, total(printer));
            printTowns(forTheCopy, printer);
            printer = printer->prev;
        }
    } else {
        while (printer != NULL) {
            printf("%s District, Population = %d\n", printer->district, total(printer));
            printTowns(forTheCopy, printer);
            printer = printer->next;
        }
    }
}

void printTowns(int type, d_ptr ptr) {
    t_ptr printer = ptr->t;
    if (printer == NULL) {
        printf("No towns in this district.\n");
        return;
    }
    if (type == forTheOriginal) {
        while (printer->nxt != NULL)
            printer = printer->nxt;
        while (printer != NULL) {
            printf(" %s, %d\n", printer->town, printer->population);
            printer = printer->prv;
        }
    } else {
        while (printer != NULL) {
            printf(" %s, %d\n", printer->town, printer->population);
            printer = printer->nxt;
        }
    }
}

int total(d_ptr d) {
    if (d == NULL || d->t == NULL) {
        return 0; //Return 0 if the district or its towns are NULL
    }
    t_ptr helper = d->t;
    int max, min, total = 0;
    char *max_n, *min_n;
    max = helper->population;
    min = 1000000000;
    while (helper != NULL) {
        total += helper->population;
        if (helper->population > max) {
            max = helper->population;
            max_n = (helper->town);
        }
        if (helper->population < min) {
            min = helper->population;
            min_n = (helper->town);
        }
        helper = helper->nxt;
    }
    d->max_popul = max;
    d->min_popul = min;
    if (d->prev == NULL) {
        max_name = max_n;
        min_name = min_n;
    } else {
        if (d->prev->max_popul < max)
            max_name = max_n;
        if (d->prev->min_popul > min)
            min_name = min_n;
    }
    return total;
}

void populationOfPalestine() {
    d_ptr popul_helper = copy;
    int pop_P = 0, max = popul_helper->max_popul, min = 1000000000;
    if (popul_helper->popul_dis != 0) {
        while (popul_helper != NULL) {
            pop_P += popul_helper->popul_dis;
            if (popul_helper->max_popul > max)
                max = popul_helper->max_popul;
            if (popul_helper->min_popul < min)
                min = popul_helper->min_popul;
            popul_helper = popul_helper->next;
        }
    } else {
        while (popul_helper != NULL) {
            pop_P += total(popul_helper);
            if (popul_helper->max_popul > max)
                max = popul_helper->max_popul;
            if (popul_helper->min_popul < min)
                min = popul_helper->min_popul;
            popul_helper = popul_helper->next;
        }
    }
    printf("The total population of Palestine= %d\nThe highest population= %d, The lowest population= %d",
           pop_P, max, min);
    int show;
    printf("\nDo you want the name of the town with the highest population and the town with lowest population respectively?\nEnter 1 if yes and 2 if no\n");
    scanf("%d", &show);
    if (show == 1)
        printf("%s has the highest, %s has the lowest\n", max_name, min_name);
}


void printTotal() {
    d_ptr printer = copy;
    int tot;
    while (printer != NULL) {
        if (printer->popul_dis != 0) {
            tot = printer->popul_dis;
        } else {
            tot = total(printer);
            printer->popul_dis = tot;
        }
        printf("%s District, Population = %d\n", printer->district, tot);
        printer = printer->next;
    }
}

void deleteTown(t_ptr deleteT, char town_name[]) {
    if (deleteT == NULL) {
        printf("\nDistrict not found.\n");
        return;
    }
    strcpy(town_name, fix(town_name));
    t_ptr currentTown = deleteT;
    t_ptr prevTown = NULL;
    while (currentTown != NULL && strcmp(currentTown->town, town_name) != 0) {
        prevTown = currentTown;
        currentTown = currentTown->nxt;
    }
    if (currentTown == NULL) {
        printf("\nThere is no town with this name.\n");
        return;
    }
    if (prevTown != NULL) {
        prevTown->nxt = currentTown->nxt;
    }
    if (currentTown->nxt != NULL) {
        currentTown->nxt->prv = prevTown;
    }
    printf("'%s' got deleted successfully.\n", town_name);
    free(currentTown);
}


void save() {
    FILE *districts = fopen("sorted_districts.txt", "w");
//there is no need to check if the file exists since it will automatically create one
    d_ptr updated = copy;
    while (updated != NULL) {
        fprintf(districts, "%s District, Population = %d\n", updated->district, total((updated)));
        while (updated->t != NULL) {
            fprintf(districts, "%s, %d\n", updated->t->town, updated->t->population);
            updated->t = updated->t->nxt;
        }
        updated = updated->next;
    }
    printf("The data is now in the file\n");
    fclose(districts);
}

void freeAll(d_ptr TAIL) {
    d_ptr temp_d;
    t_ptr temp_t;
    while (TAIL != NULL) {
        temp_d = TAIL;
        TAIL = TAIL->prev;
        while (temp_d->t != NULL) {
            temp_t = temp_d->t;
            temp_d->t = temp_d->t->nxt;
            free(temp_t);//free the town linked list
        }
        free(temp_d);//free the district linked list
    }
}

void printOrigin() {
    d_ptr printer = original_tail;
    while (printer != NULL) {
        t_ptr townP = printer->t;
        while (townP->nxt != NULL)
            townP = townP->nxt;
        while (townP != NULL) {
            printf("%s | %s | %d\n", printer->district, townP->town, townP->population);
            townP = townP->prv;
        }
        printer = printer->prev;
    }
}

