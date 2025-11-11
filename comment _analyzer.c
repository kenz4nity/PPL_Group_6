#include <stdio.h>
#include <string.h>

/* Treat comments as WHITESPACE with blank lexeme.
   Emit a token for each delimiter:
     ##  -> SL_COMMENT
     #*  -> SML_COMMENT (start of multi-line)
     *#  -> EML_COMMENT (end of multi-line)
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
                /* recognize the delimiter */
                printf("Token: SL_COMMENT, Lexeme: \n");
                i += 2; /* consume '##' */
                while (i < len && input[i] != '\n') i++;
                if (i < len && input[i] == '\n') i++;  /* consume newline */
                break;
            }

            /* #* block comment start */
            if (i + 1 < len && input[i + 1] == '*') {
                printf("Token: SML_COMMENT, Lexeme: \n");
                i += 2; /* consume '#*' */

                int closed = 0;
                /* skip everything until '*#' */
                while (i < len) {
                    if (input[i] == '*' && (i + 1 < len) && input[i + 1] == '#') {
                        /* recognize end delimiter */
                        printf("Token: EML_COMMENT, Lexeme: \n");
                        i += 2; /* consume '*#' */
                        closed = 1;
                        break;
                    }
                    i++;
                }
                if (!closed) {
                    /* Unterminated block comment at EOF */
                    fprintf(stderr, "Warning: Unterminated block comment (missing '*#')\n");
                }
                break;
            }

            /* Lone '#' (not a comment delimiter by these rules): skip one char */
            i++;
            break;
        }

        default:
            /* Not a comment start: skip this character */
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
        "## another one\n";

    printf("Analyzing sample input:\n%s\n\n", code);
    lexicalAnalyzer(code);
    return 0;
}
