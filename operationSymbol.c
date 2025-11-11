#include <stdio.h>
#include <string.h>

/**
 * It scans an input string and identifies tokens (operators) based on the state transitions.
 * How it maps to the DFA:
    - The main 'while' loop iterates through the string.
    - The 'switch(currentChar)' acts as the main transition from the START state (S0).
    - The 'if/else if/else' blocks inside each 'case' implement the transitions
 */

void lexicalAnalyzer(char* input) {
    int i = 0;
    int len = strlen(input); //length of input string

    printf("=== Lexical Analysis Start === \n");

    while (i < len) {
        char currentChar = input[i];

        // This switch statement will handle the transitions from S0
        switch (currentChar) {


        case '+':
            // S0 -> S1 on '+'
            if (i + 1 < len && input[i + 1] == '+') {   // S1 -> Final on '+'
                printf("Token: ARITHMETIC_OP, Lexeme: ++\n");
                i += 2;
            }
            else if (i + 1 < len && input[i + 1] == '=') { // S1 -> Final on '='
                printf("Token: ASSIGNMENT_OP, Lexeme: +=\n");
                i += 2;
            }
            else { // S1 -> Final on 'epsilon'
                printf("Token: ARITHMETIC_OP, Lexeme: +\n");
                i++;
            }
            break;

        case '-':
            // S0 -> S2 on '-'
            if (i + 1 < len && input[i + 1] == '-') { // S2 -> Final on '-'
                printf("Token: ARITHMETIC_OP, Lexeme: --\n");
                i += 2;
            }
            else if (i + 1 < len && input[i + 1] == '=') { // S2 -> Final on '='
                printf("Token: ASSIGNMENT_OP, Lexeme: -=\n");
                i += 2;
            }
            else { // S2 -> Final on 'epsilon'
                printf("Token: ARITHMETIC_OP, Lexeme: -\n");
                i++;
            }
            break;

        case '*':
            // S0 -> S3 on '*'
            if (i + 1 < len && input[i + 1] == '=') { // S3 -> Final on '='
                printf("Token: ASSIGNMENT_OP, Lexeme: *=\n");
                i += 2;
            }
            // Added based on your "C. Exponent = (**)"
            else if (i + 1 < len && input[i + 1] == '*') {
                printf("Token: EXPONENT_OP, Lexeme: **\n");
                i += 2;
            }
            else { // S3 -> Final on 'epsilon'
                printf("Token: ARITHMETIC_OP, Lexeme: *\n");
                i++;
            }
            break;

        case '/':
            // S0 -> S4 on '/'
            if (i + 1 < len && input[i + 1] == '=') { // S4 -> Final on '='
                printf("Token: ASSIGNMENT_OP, Lexeme: /=\n");
                i += 2;
            }
            else { // S4 -> Final on 'epsilon'
                printf("Token: ARITHMETIC_OP, Lexeme: /\n");
                i++;
            }
            break;

        case '%':
            // S0 -> S5 on '%'
            if (i + 1 < len && input[i + 1] == '=') { // S5 -> Final on '='
                printf("Token: ASSIGNMENT_OP, Lexeme: %%=\n");
                i += 2;
            }
            else { // S5 -> Final on 'epsilon'
                printf("Token: ARITHMETIC_OP, Lexeme: %%\n");
                i++;
            }
            break;

            // for Boolean/Relational Operations (Image 2) 

        case '<':
            // S0 -> S1 on '<'
            if (i + 1 < len && input[i + 1] == '=') { // S1 -> Final on '='
                printf("Token: RELATIONAL_OP, Lexeme: <=\n");
                i += 2;
            }
            else { // S1 -> Final on 'epsilon'
                printf("Token: RELATIONAL_OP, Lexeme: <\n");
                i++;
            }
            break;

        case '>':
            // S0 -> S2 on '>'
            if (i + 1 < len && input[i + 1] == '=') { // S2 -> Final on '='
                printf("Token: RELATIONAL_OP, Lexeme: >=\n");
                i += 2;
            }
            else { // S2 -> Final on 'epsilon'
                printf("Token: RELATIONAL_OP, Lexeme: >\n");
                i++;
            }
            break;

        case '=':
            // S0 -> S3 on '='
            if (i + 1 < len && input[i + 1] == '=') { // S3 -> Final on '='
                printf("Token: RELATIONAL_OP, Lexeme: ==\n");
                i += 2;
            }
            else { // S3 -> Final on 'epsilon'
                printf("Token: ASSIGNMENT_OP, Lexeme: =\n");
                i++;
            }
            break;

        case '!':
            // S0 -> S4 on '!'
            if (i + 1 < len && input[i + 1] == '=') { // S4 -> Final on '='
                printf("Token: RELATIONAL_OP, Lexeme: !=\n");
                i += 2;
            }
            else { // S4 -> Final on 'epsilon'
                printf("Token: LOGICAL_OP, Lexeme: !\n");
                i++;
            }
            break;

        case '&':
            // S0 -> S5 on '&'
            if (i + 1 < len && input[i + 1] == '&') { // S5 -> Final on '&'
                printf("Token: LOGICAL_OP, Lexeme: &&\n");
                i += 2;
            }
            else { // S5 -> Final on 'epsilon'
                printf("Token: UNKNOWN_TOKEN, Lexeme: &\n");
                i++;
            }
            break;

        case '|':
            // S0 -> S6 on '|'
            if (i + 1 < len && input[i + 1] == '|') { // S6 -> Final on '|'
                printf("Token: LOGICAL_OP, Lexeme: ||\n");
                i += 2;
            }
            else { // S6 -> Final on 'epsilon'
                printf("Token: UNKNOWN_TOKEN, Lexeme: |\n");
                i++;
            }
            break;

            // Other Characters (not part of the operator DFAs) 

        case ' ':
        case '\t':
        case '\n':

            // Ignore whitespacegir
            i++;
            break;

        default:
            // If the character is not a recognized operator or whitespace,
            // we'll just label it as "OTHER" for this example.
            // A full lexer would analyze for identifiers, numbers, etc.
            printf("Token: OTHER, Lexeme: %c\n", currentChar);
            i++;
            break;
        }
    }
    printf("=== Lexical Analysis End ===\n");
}

// Main function to run the lexical analyzer
int main() {
    char testCode[] = "a = b + c; \n"
        "x++ * (y--); \n"
        "if (a >= 10 && b <= 20 || c != 0) { \n"
        "  z *= 2; \n"
        "  d **= 3; \n"
        "}";

    printf("Analyzing the following code:\n%s\n\n", testCode);

    // to run the analyzer
    lexicalAnalyzer(testCode);

    return 0;
}