#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

typedef enum TokenTypes {
    IDENTIFIER,
    WHITESPACE,
    PUNCTUATION,
    LITERAL
} TokenTypes;

// Node structure for linked list
struct Node {
    TokenTypes tokenType;
    char value[100];
    struct Node* next;
};

// Function to create a new node
struct Node* createNode(TokenTypes type, char value[]) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->tokenType = type;
    strcpy(newNode->value, value);
    newNode->next = NULL;
    return newNode;
}

// Function to append a node to the end of the list
void appendNode(struct Node** head, TokenTypes type, char value[]) {
    struct Node* newNode = createNode(type, value);
    if (*head == NULL) {
        *head = newNode;
        return;
    }

    struct Node* temp = *head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = newNode;
}

// Function to print the linked list
void printList(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        switch (temp->tokenType) {
            case IDENTIFIER: printf("IDENTIFIER -> "); break;
            case WHITESPACE: printf("WHITESPACE -> "); break;
            case PUNCTUATION: printf("PUNCTUATION -> "); break;
            case LITERAL: printf("LITERAL ->"); break;
        }
        printf("%s\n", temp->value);
        temp = temp->next;
    }
}

int IsIdentifier (char InputBuffer[]) {
    int i = 0;

    // Rule 1: Empty string check
    if (InputBuffer[0] == '\0')
        return 0;

    // Rule 2: First character must be a letter or underscore
    if (!(isalpha(InputBuffer[0]) || InputBuffer[0] == '_'))
        return 0;

    // Rule 3: Remaining characters must be alphanumeric or underscore
    for (i = 1; InputBuffer[i] != '\0'; i++) {
        if (!(isalnum(InputBuffer[i]) || InputBuffer[i] == '_'))
            return 0;
    }

    return 1;
}

int main() {
    struct Node* head = NULL;
    char InputBuffer[100] = "Hi";
    char InputBuffer2[100] = "  ";

    if (IsIdentifier(InputBuffer) == 1)
    {
        appendNode(&head, IDENTIFIER, InputBuffer);
    } else {
        printf("Error: The input is not an identifier\n");
    }

    if (IsIdentifier(InputBuffer2) == 1)
    {
        appendNode(&head, IDENTIFIER, InputBuffer2);
    } else {
        printf("Error: The input is not an identifier\n");
    }
    

    // Example usage
    printList(head);

    // Free memory
    struct Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
