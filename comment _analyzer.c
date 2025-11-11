#include <stdio.h>
#include <string.h>

/* Treat comments as WHITESPACE with blank lexeme.
   Line comment:  ## ... (ends at newline)
   Block comment: #* ... *#
*/

void lexicalAnalyzer(char *input) {
    int i = 0;
    int len = (int)strlen(input);

    while (i < len) {
        char c = input[i];

        switch (c) {
        case '#': {
            /* ## line comment */
            if (i + 1 < len && input[i + 1] == '#') {
                i += 2;                         /* consume '##' */
                while (i < len && input[i] != '\n') i++;
                printf("Token: WHITESPACE, Lexeme: \n");
                if (i < len && input[i] == '\n') i++;  /* consume newline */
                break;
            }
            /* #* block comment ... *# */
            if (i + 1 < len && input[i + 1] == '*') {
                i += 2;/* consume '#*' */
                while (i < len) {
                    if (input[i] == '*' && (i + 1 < len) && input[i + 1] == '#') {
                        i += 2; /* consume '*#' */
                        break;
                    }
                    i++;
                }
                printf("Token: WHITESPACE, Lexeme: \n");
                break;
            }

            /* Lone '#' (not a comment by the rules): skip one char */
            i++;
            break;
        }

        default:
            /* Not a comment start: skip */
            i++;
            break;
        }
    }
}

int main(void) {
    char code[] =
        "x = 1; ## line comment here\n"
        "y = 2; #* block comment\n"
        "spans lines *# z = 3;\n"
        "## another one\n"
        "edge cases: # alone, * alone, #* unterminated at EOF maybe";

    printf("Analyzing sample input:\n%s\n\n", code);
    lexicalAnalyzer(code);
    return 0;
}
