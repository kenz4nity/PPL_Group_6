#include <stdio.h>
#include <string.h>
#include <ctype.h>   // for isdigit() and isalpha()
#include <stdbool.h> // for bool type

void lexicalAnalyzer(char* input) {
    int i = 0;
    int len = strlen(input);

    printf("=== Lexical Analysis for Constants ===\n");

    while (i < len) {
        char currentChar = input[i];

        // This handles the start of known literals 
        switch (currentChar) {

        case '"': {
            char lexeme[100];
            int k = 0;
            lexeme[k++] = input[i++]; // Add the opening "

           
            while (i < len && input[i] != '"') {
                lexeme[k++] = input[i++];
            }

            if (i < len && input[i] == '"') {
                lexeme[k++] = input[i++]; // Add the closing "
            }
            lexeme[k] = '\0';

            printf("Token: CONSTANT, Lexeme: %s\n", lexeme);
            break;
        }

                // For Handling Char Literals
        case '\'': {
            
            if (i + 2 < len && input[i + 2] == '\'') {
                printf("Token: CONSTANT, Lexeme: '");
                putchar(input[i + 1]); // Print the char inside
                printf("'\n");
                i += 3; // Skip over the 'c'
            }
            else {
                printf("Token: UNKNOWN_TOKEN, Lexeme: %c\n", currentChar);
                i++;
            }
            break;
        }

                 //Handle Whitespace
        case ' ':
        case '\t':
        case '\n':
            // Ignore whitespace
            i++;
            break;

            //Handle Numbers, Booleans, and other "letter" constants
        default:
            // Validation if it is Int or Float
            if (isdigit(currentChar)) {
                char lexeme[100];
                int k = 0;

                // Loop while it's a digit or a decimal point
                while (i < len && (isdigit(input[i]) || input[i] == '.')) {
                    lexeme[k++] = input[i++];
                }
                lexeme[k] = '\0'; // Null-terminate the string

                // output CONSTANT for both Ints and Floats
                printf("Token: CONSTANT, Lexeme: %s\n", lexeme);
            }
            // Is it a letter or string 
            else if (isalpha(currentChar)) {
                char lexeme[100];
                int k = 0;

                // Loop while it's a letter or a digit
                while (i < len && isalnum(input[i])) {
                    lexeme[k++] = input[i++];
                }
                lexeme[k] = '\0'; // Null-terminate the string

                // This handles true, false, and abc with one rule.
                printf("Token: CONSTANT, Lexeme: %s\n", lexeme);
            }
            // Otherwise, it's an unknown symbol
            else {
                printf("Token: UNKNOWN_TOKEN, Lexeme: %c\n", currentChar);
                i++;
            }
            break;
        }
    }
    printf("=== Lexical Analysis End ===\n");
}

// Main function to run the lexical analyzer
int main() {
    char testCode[] = "char name = kenneth; \n"
        "int number = 20; \n"
        "float number2 = 40.5; \n"
        "bool flag = true; \n"
        "string title = \"My App\"; \n"
        "char letter = 'Z';";

    printf("Analyzing the following code:\n%s\n\n", testCode);

    
    lexicalAnalyzer(testCode);

    return 0;
}