#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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

    int ch, next_ch;
    int line = 1;
    int column = 1;
    char lexeme[MAX_LEXEME_LEN] = "";
    int lexeme_index = 0;
    char delimiter[4] = "";

    while ((ch = fgetc(file)) != EOF) {
        if (isspace(ch) || ispunct(ch)) {

            // Store accumulated word before delimiter
            if (lexeme_index > 0) {
                lexeme[lexeme_index] = '\0';
                insertAtEndLexeme(head, lexeme, line, column - lexeme_index);
                lexeme_index = 0;
            }

            // Handle newlines and spaces
            if (ch == '\n') {
                insertAtEndLexeme(head, "\n", line, column);
                line++;
                column = 1;
                continue;
            } else if (ch == '\t') {
                insertAtEndLexeme(head, "\t", line, column);
                column += 4;
                continue;
            } else if (isspace(ch)) {
                insertAtEndLexeme(head, " ", line, column);
                column++;
                continue;
            }

            // Punctuation: peek next
            next_ch = fgetc(file);

            // --- Single-line comment (##) ---
            if (ch == '#' && next_ch == '#') {
                char comment[MAX_LEXEME_LEN] = "##";
                int i = 2;
                while ((ch = fgetc(file)) != EOF && ch != '\n' && i < MAX_LEXEME_LEN - 1) {
                    comment[i++] = ch;
                }
                comment[i] = '\0';
                insertAtEndLexeme(head, comment, line, column);
                if (ch == '\n') {
                    insertAtEndLexeme(head, "\n", line, column);
                    line++;
                    column = 1;
                }
                continue;
            }

            // --- Multi-line comment (#* ... *#) ---
            if (ch == '#' && next_ch == '*') {
                char comment[MAX_LEXEME_LEN * 10] = "#*";  // buffer for comment
                int i = 2;
                int prev = 0;

                while ((ch = fgetc(file)) != EOF && i < (int)sizeof(comment) - 1) {
                    comment[i++] = ch;

                    if (ch == '\n') {
                        line++;
                        column = 1;
                    } else {
                        column++;
                    }

                    // ✅ Move prev update *before* the check
                    if (prev == '*' && ch == '#') {
                        // We already added '#' into comment[i-1]
                        break;  // properly includes the terminator
                    }

                    prev = ch;
                }

                comment[i] = '\0';
                insertAtEndLexeme(head, comment, line, column);
                continue;
            }

            // --- Two-character operators ---
            char two_char[3] = { (char)ch, (char)next_ch, '\0' };
            if (
                strcmp(two_char, "==") == 0 || strcmp(two_char, "!=") == 0 ||
                strcmp(two_char, ">=") == 0 || strcmp(two_char, "<=") == 0 ||
                strcmp(two_char, "++") == 0 || strcmp(two_char, "--") == 0 ||
                strcmp(two_char, "+=") == 0 || strcmp(two_char, "-=") == 0 ||
                strcmp(two_char, "*=") == 0 || strcmp(two_char, "/=") == 0 ||
                strcmp(two_char, "&&") == 0 || strcmp(two_char, "||") == 0 ||
                strcmp(two_char, "->") == 0
            ) {
                insertAtEndLexeme(head, two_char, line, column);
                column += 2;
            } else {
                ungetc(next_ch, file);
                delimiter[0] = (char)ch;
                delimiter[1] = '\0';
                insertAtEndLexeme(head, delimiter, line, column);
                column++;
            }

        } else {
            // Build normal lexeme
            if (lexeme_index < MAX_LEXEME_LEN - 1) {
                lexeme[lexeme_index++] = ch;
            }
            column++;
        }
    }

    // Handle final lexeme
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
