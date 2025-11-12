#include <stdio.h>
#include <string.h>

/*
  - The 'switch(currentChar)' acts as the main transition from the START state (S0/Code).
  - The 'if/else if/else' blocks implement the "lookahead" logic to handle multi-character tokens.
  - For single-character tokens (like ';', ','), it's a simple, direct case.
 */


void lexicalAnalyzer(char* input) {
    int i = 0;
    int len = strlen(input);

    printf("=== Lexical Analysis Start ===\n");

    while (i < len) {
        char currentChar = input[i];

		// This switch statement handles the transitions from S0 or the starting state
        switch (currentChar) {

            // for Delimiters and Brackets 
        case ';': 
            printf("Token: END_OF_STATEMENT, Lexeme: ;\n");
            i++;
            break;

        case ':': 
            printf("Token: START_CODE_BLOCK, Lexeme: :\n");
            i++;
            break;

        case ',': 
            printf("Token: SEPARATOR, Lexeme: ,\n");
            i++;
            break;

        case '"': 
            if (i + 1 < len && input[i + 1] == '"') {
                printf("Token: STRING_LITERAL, Lexeme: \"\"\n");
                i += 2;
            }
            else {
                printf("Token: UNKNOWN_TOKEN, Lexeme: \"\n");
                i++;
            }
            break;

        case '(': 
            if (i + 1 < len && input[i + 1] == ')') {
                printf("Token: GROUP_EXPRESSION, Lexeme: ()\n");
                i += 2;
            }
            else {
                printf("Token: LEFT PARENTHESIS, Lexeme: (\n");
                i++;
            }
            break;

        case ')':
            printf("Token: RIGHT PARENTHESIS, Lexeme: )\n");
            i++;
            break;

        case '{': 
            if (i + 1 < len && input[i + 1] == '}') {
                printf("Token: CODE_BLOCK, Lexeme: {}\n");
                i += 2;
            }
            else {
                printf("Token: LEFT BRACE, Lexeme: {\n");
                i++;
            }
            break;

        case '}':
            printf("Token: RIGHT BRACE, Lexeme: }\n");
            i++;
            break;

        case '[': // S0 -> [ -> Array_or_List -> ] -> Final
            if (i + 1 < len && input[i + 1] == ']') {
                printf("Token: ARRAY_LIST, Lexeme: []\n");
                i += 2;
            }
            else {
                printf("Token: LEFT BRACKET, Lexeme: [\n");
                i++;
            }
            break;

        case ']':
            printf("Token: RIGHT BRACKET, Lexeme: ]\n");
            i++;
            break;

            // Other Characters

        case ' ':
        case '\t':
        case '\n':
            // Ignore whitespace
            i++;
            break;

        default:
            // Handle Other tokens temporarily
            printf("Token: OTHER, Lexeme: %c\n", currentChar);
            i++;
            break;
        }
    }
    printf("=== Lexical Analysis End ===\n");
}

// Main function to run the lexical analyzer
int main() {
    // A test string containing tokens from ALL diagrams
    char testCode[] = "a = b + c; // This is a test\n"
        "x++ * (y--); \n"
        "if (a >= 10 && b <= 20 || c != 0) { \n"
        "  z *= 2; \n"
        "  myFunc(d, e); \n"
        "  char* s = \"\"; \n"
        "  int arr[] = {}; \n"
        "}";

    printf("Analyzing the following code:\n%s\n\n", testCode);

    // Run the analyzer
    lexicalAnalyzer(testCode);

    return 0;
}