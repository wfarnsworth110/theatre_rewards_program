// Programming Assignment 5
// Walker Farnsworth
// wa934080

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define COMMAND 300000
#define MAXLEN 19

typedef struct customer{
	
	char name[MAXLEN + 1];
	int points;
	
}customer;

typedef struct treenode{
	
	customer* cPtr;
	int size;
	struct treenode* left;
	struct treenode* right;
	
}treenode;

treenode * create_node(char * name, int points);
customer * new_customer(char * name, int points);
void free_node(treenode * root);
treenode * add(treenode * root, char * name, int points, int exists);
treenode * sub(treenode * root, char * name, int points);
treenode * del(treenode * root, char * name, int exists);
treenode * inorderPredecessor(treenode * node);
int halfsearch(treenode * root, char * name);
void fullsearch(treenode * root, char * name, int * depth);
int count_smaller(treenode * root, char * name, int * counter, int * saved_direction, int * current_direction);
int bst_compare(char * name, char * root_str);
int sort_compare(customer * left, customer * right);
int toArray(treenode * root, customer ** library, int index);
void merge(customer ** library, int start, int divide, int end);
void mergeSort(customer ** library, int start, int end);
void destroyTree(treenode * root);
void clearBuffer();
void chomp(char * word);

int main()
{
	// Plant our tree :)
	treenode * root = NULL;
	
	// Command limit must be between 0 and 300,000
	int n;
	int nFlag = 1;
	do{
		scanf("%d", &n);
		clearBuffer();
		
		if(n < 0 || n > COMMAND)
			printf("Invalid input, please try again\n");
		else
			nFlag = 0;
	}while(nFlag);
	
	// Every input command
	for(int i = 0; i < n; ++i)
	{
		// Valid fgets() input per n
		int flag = 1;
		do{
			// Input variables
			char select[38] = {};
			char command[14] = {};
			char name[20] = {};
			char ptsInput[4] = {};
			int points = 0;
			
			// Helper variables
			int spaceCheck = 0;
			int checkpoint = 0;
			
			// Process input
			fgets(select, 38, stdin);
			chomp(select);
			int sizeSelect = strlen(select);
			
			for(int x = 0; x < sizeSelect; ++x)
			{
				if(select[x] == ' ')
				{
					++spaceCheck;
					strncpy(command, select, x);
					break;
				}
			}
			
			if(spaceCheck == 0)
			{
				printf("Invalid input, please try again\n");
				continue;
			}
			
			if(strcmp(command, "add") == 0)
			{
				spaceCheck = 0;
				checkpoint = strlen(command) + 1;
				
				for(int x = checkpoint; x < sizeSelect; ++x)
				{
					if(select[x] == ' ')
					{
						++spaceCheck;
						strncpy(name, &select[checkpoint], x - checkpoint);
						break;
					}
				}
				
				if(spaceCheck == 0)
				{
					printf("Invalid input, please try again\n");
					continue;
				}
				
				checkpoint = strlen(command) + strlen(name) + 2;
				strcpy(ptsInput, &select[checkpoint]);
				points = atoi(ptsInput);
				
				if(points < 0 || points > 100)
				{
					printf("Invalid input, please try again\n");
					continue;
				}
				
				// Execute add function
				int existsAdd = halfsearch(root, name);
				root = add(root, name, points, existsAdd);
				
				flag = 0;
			}
			else if(strcmp(command, "sub") == 0)
			{
				spaceCheck = 0;
				checkpoint = strlen(command) + 1;
				
				for(int x = checkpoint; x < sizeSelect; ++x)
				{
					if(select[x] == ' ')
					{
						++spaceCheck;
						strncpy(name, &select[checkpoint], x - checkpoint);
						break;
					}
				}
				
				if(spaceCheck == 0)
				{
					printf("Invalid input, please try again\n");
					continue;
				}
				
				checkpoint = strlen(command) + strlen(name) + 2;
				strcpy(ptsInput, &select[checkpoint]);
				points = atoi(ptsInput);
				
				if(points < 0 || points > 100)
				{
					printf("Invalid input, please try again\n");
					continue;
				}
				
				// Execute sub function
				root = sub(root, name, points);
				
				flag = 0;
			}
			else if(strcmp(command, "del") == 0)
			{
				checkpoint = strlen(command) + 1;
				strcpy(name, &select[checkpoint]);
				
				// Execute del function
				int existsDel = halfsearch(root, name);
				if(existsDel == 0)
					printf("%s not found\n", name);
				else
				{
					root = del(root, name, existsDel);
					printf("%s deleted\n", name);
				}
				
				flag = 0;
			}
			else if(strcmp(command, "search") == 0)
			{
				checkpoint = strlen(command) + 1;
				strcpy(name, &select[checkpoint]);
				
				// Execute search function
				int depth = 0;
				fullsearch(root, name, &depth);
				
				flag = 0;
			}
			else if(strcmp(command, "count_smaller") == 0)
			{
				checkpoint = strlen(command) + 1;
				strcpy(name, &select[checkpoint]);
				
				int counter = 0;
				int saved_direction = 0; // 0 will be treated as left
				int current_direction = 0; // 1 will be treated as right
				
				// Execute count_smaller function
				int smaller = count_smaller(root, name, &counter, &saved_direction, &current_direction);
				if (smaller < 0)
					printf("%s not found\n", name);
				else
					printf("%d\n", smaller);
				
				flag = 0;
			}
			else
				printf("Invalid input, please try again\n");
			
		}while(flag);
	}
	
	// Create array of customer pointers
	int customer_count = root->size;
	customer ** library = calloc(customer_count, sizeof(customer*));
	
	if(library == NULL)
	{
		printf("calloc error in main...program exiting\n");
		exit(1);
	}
	
	// Copy pointers from BST to array
	int arrSize = toArray(root, library, 0);
	
	// Merge sort
	mergeSort(library, 0, arrSize - 1);
	
	// Print library
	for(int i = 0; i < customer_count; ++i)
	{
		printf("%s %d\n", library[i]->name, library[i]->points);
	}
	
	// Wrap things up
	free(library);
	destroyTree(root);
	root = NULL;
	
	return 0;
}

treenode * create_node(char * name, int points)
{
	treenode * temp = malloc(sizeof(treenode));
	
	if(temp == NULL)
	{
		printf("malloc error in create_node...program exiting\n");
		exit(1);
	}
	
	temp->cPtr = new_customer(name, points);
	temp->size = 1;
	temp->left = NULL;
	temp->right = NULL;
	
	printf("%s %d\n", temp->cPtr->name, temp->cPtr->points);
	return temp;
}

customer * new_customer(char * name, int points)
{
	customer * user = malloc(sizeof(customer)); //malloc may require tuning
	
	if(user == NULL)
	{
		printf("malloc error in new_customer...program exiting\n");
		exit(1);
	}
	
	strcpy(user->name, name);
	user->points = points;
	
	return user;
}

void free_node(treenode * root)
{
	if(root != NULL)
	{
		free(root->cPtr);
		root->cPtr = NULL;
		free(root);
		root = NULL;
	}
}

treenode * add(treenode * root, char * name, int points, int exists)
{
	if(root == NULL)
	{
		return create_node(name, points);
	}
	
	if(exists == 0)
		++root->size;
	
	int cmp = bst_compare(name, root->cPtr->name);
	
	if(cmp < 0)
	{
		root->left = add(root->left, name, points, exists);
		return root;
	}
	else if(cmp > 0)
	{
		root->right = add(root->right, name, points, exists);
		return root;
	}
	
	root->cPtr->points += points;
	printf("%s %d\n", root->cPtr->name, root->cPtr->points);
	
	return root;
}

treenode * sub(treenode * root, char * name, int points)
{
	if(root == NULL)
	{
		printf("%s not found\n", name);
		return root;
	}
	
	int cmp = bst_compare(name, root->cPtr->name);
	
	if(cmp < 0)
	{
		root->left = sub(root->left, name, points);
		return root;
	}
	else if(cmp > 0)
	{
		root-> right = sub(root->right, name, points);
		return root;
	}
	
	root->cPtr->points -= points;
	if(root->cPtr->points < 0)
		root->cPtr->points = 0;
	printf("%s %d\n", root->cPtr->name, root->cPtr->points);
	
	return root;
}

treenode * del(treenode * root, char * name, int exists)
{
	if(root == NULL)
	{
		return root;
	}
	
	if(exists == 1)
		--root->size;
	
	int cmp = bst_compare(name, root->cPtr->name);
	
	if(cmp < 0)
	{
		root->left = del(root->left, name, exists);
		return root;
	}
	else if(cmp > 0)
	{
		root->right = del(root->right, name, exists);
		return root;
	}
	else
	{
		// One or no children
		if(root->left == NULL && root->right == NULL)
		{
			free_node(root);
			return NULL;
		}
		else if(root->left == NULL && root->right != NULL)
		{
			treenode * temp = root->right;
			free_node(root);
			return temp;
		}
		else if(root->right == NULL && root->left != NULL)
		{
			treenode * temp = root->left;
			free_node(root);
			return temp;
		}
		
		// Two children
		treenode * temp = inorderPredecessor(root->left);
		
		// Transfer customer pointer but keep size the same
		free(root->cPtr);
		root->cPtr = new_customer(temp->cPtr->name, temp->cPtr->points);
		
		// Delete the previous node storing predecessor
		char temp_name[20] = {};
		strcpy(temp_name, temp->cPtr->name);
		root->left = del(root->left, temp_name, 1);
	}
	
	return root;
}

treenode * inorderPredecessor(treenode * node)
{
	treenode * current = node;
	
	while(current->right != NULL)
		current = current->right;
	
	return current;
}

int halfsearch(treenode * root, char * name)
{
	if(root == NULL)
		return 0;
	
	int cmp = bst_compare(name, root->cPtr->name);
	
	if(cmp < 0)
		return halfsearch(root->left, name);
	else if(cmp > 0)
		return halfsearch(root->right, name);
	
	return 1;
}

void fullsearch(treenode * root, char * name, int * depth)
{
	if(root == NULL)
	{
		printf("%s not found\n", name);
		return;
	}
	
	// strcmp target, root->name
	int cmp = bst_compare(name, root->cPtr->name);
	
	if(cmp < 0)
	{
		*depth += 1;
		fullsearch(root->left, name, depth);
		return;
	}
	else if(cmp > 0)
	{
		*depth += 1;
		fullsearch(root->right, name, depth);
		return;
	}
	
	printf("%s %d %d\n", root->cPtr->name, root->cPtr->points, *depth);
}

int count_smaller(treenode * root, char * name, int * counter, int * saved_direction, int * current_direction)
{
	// Person doesn't exist
	if(root == NULL)
		return -1;
	
	int cmp = bst_compare(name, root->cPtr->name);
	
	if(cmp < 0)
	{
		// Check for directional change
		*current_direction = 0;
		if(*saved_direction != *current_direction)
			*counter -= root->size;
		*saved_direction = *current_direction;
		
		return count_smaller(root->left, name, counter, saved_direction, current_direction);
	}
	else if(cmp > 0)
	{
		// Check for directional change
		*current_direction = 1;
		if(*saved_direction != *current_direction)
			*counter += root->size;
		*saved_direction = *current_direction;
		
		return count_smaller(root->right, name, counter, saved_direction, current_direction);
	}
	
	int smaller = 0;
	
	// Match
	*current_direction = 1;
	if(root->right == NULL && root->left == NULL)
	{
		// Node has no children
		if(*counter == 0)
			return 0;
		
		if(*saved_direction == *current_direction)
			*counter -= 1;

		smaller = *counter;
		return smaller;
	}
	else if(root->right == NULL && root->left != NULL)
	{
		// Node has left child only
		if(*counter == 0)
			return root->left->size;
		
		if(*saved_direction == *current_direction)
			*counter -= root->size;
		
		*counter += root->left->size;
		smaller = *counter;
		return smaller;
	}
	else if(root->right != NULL && root->left == NULL)
	{
		// Node has right child only
		if(*counter == 0)
			return 0;
		
		if(*saved_direction == *current_direction)
			*counter -= root->size;
		
		smaller = *counter;
		return smaller;
	}
	else
	{
		// Node has right and left child
		if(*counter == 0)
			return root->left->size;
		
		if(*saved_direction == *current_direction)
			*counter -= root->size;
		
		*counter += root->left->size;
		smaller = *counter;
		return smaller;
	}
}

int bst_compare(char * name, char * root_str)
{
	return strcmp(name, root_str);
}

int sort_compare(customer * left, customer * right)
{
	return strcmp(left->name, right->name);
}

int toArray(treenode * root, customer ** library, int index)
{
	if(root == NULL)
		return index;
	
	library[index] = root->cPtr;
	++index;
	
	if(root->left != NULL)
		index = toArray(root->left, library, index);
	
	if(root->right != NULL)
		index = toArray(root->right, library, index);
	
	return index;
}

void merge(customer ** library, int start, int divide, int end)
{
	int sizeLeft = divide - start + 1;
	int sizeRight = end - divide;
	
	customer ** shelfLeft = malloc(sizeof(customer*) * sizeLeft);
	customer ** shelfRight = malloc(sizeof(customer*) * sizeRight);
	
	if(shelfLeft == NULL || shelfRight == NULL)
	{
		printf("malloc error in merge...program exiting\n");
		exit(1);
	}
	
	for(int x = 0; x < sizeLeft; ++x)
		shelfLeft[x] = library[start + x];
	
	for(int x = 0; x < sizeRight; ++x)
		shelfRight[x] = library[divide + x + 1];
	
	int i = 0;
	int j = 0;
	int k = start;
	
	// Meat and potatoes of merge
	while(i < sizeLeft && j < sizeRight)
	{
		if(shelfLeft[i]->points > shelfRight[j]->points)
			library[k] = shelfLeft[i++];
		else if(shelfLeft[i]->points < shelfRight[j]->points)
			library[k] = shelfRight[j++];
		else
		{
			// Scenario: points tied, determine winner by alphabetical precedence
			int cmp = sort_compare(shelfLeft[i], shelfRight[j]);
			
			if(cmp < 0)
				library[k] = shelfLeft[i++];
			else
				library[k] = shelfRight[j++]; //if they have the same name, then it doesn't matter who comes first
		}
		
		++k;
	}
	
	while(i < sizeLeft)
	{
		library[k] = shelfLeft[i++];
		++k;
	}
	
	while(j < sizeRight)
	{
		library[k] = shelfRight[j++];
		++k;
	}
	
	free(shelfLeft);
	free(shelfRight);
}

void mergeSort(customer ** library, int start, int end)
{
	if(start < end)
	{
		int divide = (start + end) / 2;
		mergeSort(library, start, divide);
		mergeSort(library, divide + 1, end);
		merge(library, start, divide, end);
	}
}

void destroyTree(treenode * root)
{
	if(root != NULL)
	{
		destroyTree(root->left);
		destroyTree(root->right);
		free_node(root);
	}
}

void clearBuffer()
{
	while(getchar() != '\n');
}

void chomp(char * word)
{
	if(word[strlen(word) - 1] == '\n')
		word[strlen(word) - 1] = '\0';
}