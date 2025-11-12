#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pcre.h>
#include "node_implementation.h"
#include "token.h"
#include "Lexical Analyzer Prompting.h"
#define MAX_LEXEME_LEN 50

void readFileAndStoreLexemes(const char *filename, struct LexemeNode **head);
int IsIdentifier (char* input);

int main () {
    struct LexemeNode* lexeme_head = NULL;
    readFileAndStoreLexemes("SourceCode.txt", &lexeme_head);

    while (lexeme_head != NULL) {
        lexicalAnalyzer(lexeme_head->lexeme);

        lexeme_head = lexeme_head->next;
    }

    // displayListLexeme(lexeme_head);
/*     struct Node* head = NULL;

    struct LexemeNode* temp_lexeme = lexeme_head;

    while (temp_lexeme != NULL) {
        if (IsIdentifier(temp_lexeme->lexeme)) {
            insertAtEnd(&head, IDENTIFIER, temp_lexeme->lexeme, temp_lexeme->line, temp_lexeme->column);
        }

        temp_lexeme = temp_lexeme->next;
    }

    displayList(head); */

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
    int next_ch;
    int line = 1;
    int column = 1;
    char lexeme[MAX_LEXEME_LEN] = "";
    int lexeme_index = 0;
    char delimiter[4] = "";  // can hold up to 3 chars + '\0'

    while ((ch = fgetc(file)) != EOF) {
        // If we reach a delimiter (space, punctuation, newline)
        if (isspace(ch) || ispunct(ch)) {
            // First, store the previous lexeme if it exists
            if (lexeme_index > 0) {
                lexeme[lexeme_index] = '\0';
                insertAtEndLexeme(head, lexeme, line, column - lexeme_index);
                lexeme_index = 0;
            }

            // Handle newline separately
            if (ch == '\n') {
                delimiter[0] = '\n';
                delimiter[1] = '\0';
                insertAtEndLexeme(head, delimiter, line, column);
                line++;
                column = 1;
            } 
            else if (ch == '\t') {
                delimiter[0] = '\t';
                delimiter[1] = '\0';
                insertAtEndLexeme(head, delimiter, line, column);
                column += 4;
            } 
            else if (isspace(ch)) {
                delimiter[0] = ' ';
                delimiter[1] = '\0';
                insertAtEndLexeme(head, delimiter, line, column);
                column++;
            } 
            else if (ispunct(ch)) {
                // Peek next character
                next_ch = fgetc(file);

                // Check if ch and next_ch form a two-character operator
                char two_char[3] = { (char)ch, (char)next_ch, '\0' };

                if (
                    strcmp(two_char, "==") == 0 || strcmp(two_char, "!=") == 0 ||
                    strcmp(two_char, ">=") == 0 || strcmp(two_char, "<=") == 0 ||
                    strcmp(two_char, "++") == 0 || strcmp(two_char, "--") == 0 ||
                    strcmp(two_char, "+=") == 0 || strcmp(two_char, "-=") == 0 ||
                    strcmp(two_char, "*=") == 0 || strcmp(two_char, "/=") == 0 ||
                    strcmp(two_char, "&&") == 0 || strcmp(two_char, "||") == 0 ||
                    strcmp(two_char, "##") == 0 || strcmp(two_char, "<<") == 0 ||
                    strcmp(two_char, ">>") == 0 || strcmp(two_char, "%=") == 0 ||
                    strcmp(two_char, "&=") == 0 || strcmp(two_char, "|=") == 0 ||
                    strcmp(two_char, "^=") == 0 || strcmp(two_char, "->") == 0
                ) {
                    strcpy(delimiter, two_char);
                    insertAtEndLexeme(head, delimiter, line, column);
                    column += 2;
                } else {
                    // Not a two-character operator
                    ungetc(next_ch, file);
                    delimiter[0] = (char)ch;
                    delimiter[1] = '\0';
                    insertAtEndLexeme(head, delimiter, line, column);
                    column++;
                }
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


/* int IsIdentifier (char* input) {
    if (!isalpha(input[0]) && input[0] != '_')
        return 0;

    for (int i = 1; input[i] != '\0'; i++) {
        if (!isalnum(input[i]) && input[i] != '_')
            return 0;
    }

    return 1;
} */
