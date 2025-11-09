#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "node_implementation.h"
#include "token.h"
#define MAX_LEXEME_LEN 50

void readFileAndStoreLexemes(const char *filename, struct LexemeNode **head);

int main () {
    struct LexemeNode* lexeme_head = NULL;

    readFileAndStoreLexemes("SourceCode.txt", &lexeme_head);

    displayListLexeme(lexeme_head);

    TokenType token = IDENTIFIER;
    
    struct Node* head = NULL;

    // test
    insertAtBeginning(&head, token, "Hello", 2, 3);

    displayList(head);


    //Input Buffer

    //Token Recognizer

    //Symbol Table Output

    // Token Stream Output

    return 0;
}

// function to read all characters and make a node for each lexeme
void readFileAndStoreLexemes(const char *filename, struct LexemeNode **head) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int ch;
    int line = 1;
    int column = 1;
    char lexeme[MAX_LEXEME_LEN] = "";
    int lexeme_index = 0;
    char delimiter[2] = "";  // to store single-character delimiters as strings

    while ((ch = fgetc(file)) != EOF) {
        // If we reach a delimiter (space, punctuation, newline)
        if (isspace(ch) || ispunct(ch)) {
            // First, store the previous lexeme if it exists
            if (lexeme_index > 0) {
                lexeme[lexeme_index] = '\0';
                insertAtEndLexeme(head, lexeme, line, column - lexeme_index);
                lexeme_index = 0;
            }

            // Handle newline separately for readability
            if (ch == '\n') {
                strcpy(delimiter, "\\n");  // represent newline as "\n"
                insertAtEndLexeme(head, delimiter, line, column);
                line++;
                column = 1;
            } 
            else if (ch == '\t') {
                strcpy(delimiter, "\\t");  // represent tab as "\t"
                insertAtEndLexeme(head, delimiter, line, column);
                column += 4; //  tab length
            } 
            else if (isspace(ch)) {
                strcpy(delimiter, " ");  // space delimiter
                insertAtEndLexeme(head, delimiter, line, column);
                column++;
            } 
            else if (ispunct(ch)) {
                // punctuation like ; , ( ) + - { } etc.
                delimiter[0] = (char)ch;
                delimiter[1] = '\0';
                insertAtEndLexeme(head, delimiter, line, column);
                column++;
            }
        } 
        else {
            // Build the current lexeme
            if (lexeme_index < MAX_LEXEME_LEN - 1) {
                lexeme[lexeme_index++] = ch;
            }
            column++;
        }
    }

    // If file ends and there's still an unfinished lexeme
    if (lexeme_index > 0) {
        lexeme[lexeme_index] = '\0';
        insertAtEndLexeme(head, lexeme, line, column - lexeme_index);
    }

    fclose(file);
}


