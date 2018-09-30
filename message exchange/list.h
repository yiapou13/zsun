//Source: https://gist.github.com/ardrabczyk/1cdc413b9eaa9a9a0c4be71d3fc9bc37

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Implements the necessary information of a message
typedef struct
{
    char sender[32];
	char receiver[32];
	char text[256];
} DATA;

//Implements the nodes for the list
struct node
{
	DATA data;
	struct node *next;
};

/*int compare( const void* node,  char* username)
{
	return(strcmp(((DATA*)node)->receiver, username) == 0);
}

int cmpfunc (const void* nodeA, const void* nodeB)
{
	return((strcmp(((DATA*)nodeA)->sender, ((DATA*)nodeB)->sender) == 0) && (strcmp(((DATA*)nodeA)->receiver, ((DATA*)nodeB)->receiver) == 0) && (strcmp(((DATA*)nodeA)->text, ((DATA*)nodeB)->text) == 0));
}*/

//Creates the list. Called once at the start of the server.
int init(struct node **head, DATA data)
{
	*head = malloc(sizeof(struct node));
	if (!*head)
	{
		fprintf(stderr, "Failed to init a linked list\n");
		return 1;
	}

	(*head)->data = data;
	(*head)->next = NULL;

	return 0;
}

//Inserts a new message to the list.
int insert(struct node **head, DATA data)
{
	struct node *current = *head;
	struct node *tmp;

	do
	{
		tmp = current;
		current = current->next;
	} while (current);

	//create a new node after tmp
	struct node *new = malloc(sizeof(struct node));
	if (!new)
	{
		fprintf(stderr, "Failed to insert a new element\n");
		return 1;
	}
	new->next = NULL;
	new->data = data;

	tmp->next = new;

	return 0;

}

//Delete a specific node from the list.
void delete(struct node **head, DATA data)
{
	if((*head)->next)
	{
		struct node *current = *head;
		struct node *prev = NULL;

		do {
			
			if((strcmp(((DATA*)current)->sender, (data.sender)) == 0) && (strcmp(((DATA*)current)->receiver, (data.receiver)) == 0) && (strcmp(((DATA*)current)->text, (data.text)) == 0))
				break;
			
			prev = current;
			current = current->next;
		} while (current);

		//if the first element
		if (current == *head)
		{
			//reuse prev
			prev = *head;
			*head = current->next;
			return;
		}

		//if the last element
		if (current->next == NULL)
		{
			prev->next = NULL;
			return;
		}

		prev->next = current->next;
		return;
	}
}

//Function with secondary functionality, helps with error checking and verification
void display(struct node** head)
{
	struct node *current = *head;
	while (current)
	{
		printf("current data: %s--%s--%s, address: %p\n", current->data.sender, current->data.receiver, current->data.text, current);
		current = current->next;
	}
}

//Function with secondary functionality, helps with error checking and verification
void messageDisplay(const void* info)
{
	printf("Sent from: %s\nMessage: %s\n", ((DATA*)info)->sender, ((DATA*)info)->text);
}
