#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

//remember to make an if statements to not exceed 50 chars on lexeme
struct Node* createNode(TokenType tokentype, char lexeme[], int line, int column) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->tokentype = tokentype;
    strcpy(newNode->lexeme, lexeme);
    newNode->line = line;
    newNode->column = column;
    newNode->next = NULL;
    return newNode;
}

struct LexemeNode* createNodeLexeme(char lexeme[], int line, int column) {
    struct LexemeNode* newLexemeNode = (struct LexemeNode*)malloc(sizeof(struct LexemeNode));
    if (!newLexemeNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(newLexemeNode->lexeme, lexeme);
    newLexemeNode->line = line;
    newLexemeNode->column = column;
    newLexemeNode->next = NULL;
    return newLexemeNode;
}

void insertAtBeginning(struct Node** head, TokenType tokentype, char lexeme[], int line, int column) {
    struct Node* newNode = createNode(tokentype, lexeme, line, column);
    newNode->next = *head;
    *head = newNode;
}

void insertAtBeginningLexeme(struct LexemeNode** head, char lexeme[], int line, int column) {
    struct LexemeNode* newLexemeNode = createNodeLexeme(lexeme, line, column);
    newLexemeNode->next = *head;
    *head = newLexemeNode;
}

void insertAtEnd(struct Node** head, TokenType tokentype, char lexeme[], int line, int column) {
    struct Node* newNode = createNode(tokentype, lexeme, line, column);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    struct Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void insertAtEndLexeme(struct LexemeNode** head, char lexeme[], int line, int column) {
    struct LexemeNode* newLexemeNode = createNodeLexeme(lexeme, line, column);
    if (*head == NULL) {
        *head = newLexemeNode;
        return;
    }
    struct LexemeNode* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newLexemeNode;
}

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case IDENTIFIER: return "IDENTIFIER";
        case OPERATION: return "OPERATION";
        case KEYWORDS: return "KEYWORDS";
        case RESERVED_WORDS: return "RESERVED_WORDS";
        case NOISE_WORDS: return "NOISE_WORDS";
        case COMMENT: return "COMMENT";
        case WHITE_SPACE: return "WHITE_SPACE";
        case DELIMITER: return "DELIMITER";
        default: return "UNKNOWN";
    }
}

void displayList(struct Node* head) {
    struct Node* temp = head;
    printf("Token Stream:\n");
    while (temp != NULL) {
        printf("<%s, \"%s\">\n", tokenTypeToString(temp->tokentype), temp->lexeme);
        temp = temp->next;
    }
    printf("NULL\n");
}

void displayListLexeme(struct LexemeNode* head) {
    struct LexemeNode* temp = head;
    printf("Token Stream:\n");
    while (temp != NULL) {
        printf("<%s>, Line: %d, Column: %d\n", temp->lexeme, temp->line, temp->column);
        temp = temp->next;
    }
    printf("NULL\n");
}

void writeSymbolTableToFile(struct Node* head, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening output file");
        return;
    }

    // Write each token in simple format
    struct Node* temp = head;
    while (temp != NULL) {
        fprintf(file, "%s ", tokenTypeToString(temp->tokentype));
        
        // Write lexeme character by character, escaping special characters
        int len = strlen(temp->lexeme);
        for (int i = 0; i < len; i++) {
            if (temp->lexeme[i] == '\n') {
                fprintf(file, "\\n");
            } else if (temp->lexeme[i] == '\t') {
                fprintf(file, "\\t");
            } else if (temp->lexeme[i] == ' ') {
                fprintf(file, "_");
            } else {
                fputc(temp->lexeme[i], file);
            }
        }
        
        fprintf(file, " %d %d\n", temp->line, temp->column);
        temp = temp->next;
    }
    
    fclose(file);
    printf("Symbol table written to '%s' successfully!\n", filename);
}