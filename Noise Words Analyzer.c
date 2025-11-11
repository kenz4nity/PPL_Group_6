#include <stdio.h>
#include <string.h>
#include <ctype.h> 

// 1. Define the list of "Noise Words" (Keywords)
const char* keywords[] = {
    "if", "else", "while", "for", "int", "char", "float", "return", "void", "main"
};
const int KEYWORD_COUNT = sizeof(keywords) / sizeof(keywords[0]);

// Function to check if a word is a keyword
int isKeyword(char* word) {
    for (int k = 0; k < KEYWORD_COUNT; k++) {
        if (strcmp(keywords[k], word) == 0) {
            return 1; 
        }
    }
    return 0; 
}

// --- MAIN ANALYZER FUNCTION ---
void lexicalAnalyzer(char* input) {
    int i = 0;
    int len = strlen(input);
    
    printf("=== Lexical Analysis Start === \n");

    while (i < len) {
        char currentChar = input[i];

        switch (currentChar) {
            
            // --- OPERATOR & DELIMITER CASES ---
            // These cases handle multi-character and single-character symbols.

            case '+': 
                if (i + 1 < len && input[i + 1] == '+') { printf("Token: ARITHMETIC_OP, Lexeme: ++\n"); i += 2; } 
                else if (i + 1 < len && input[i + 1] == '=') { printf("Token: ASSIGNMENT_OP, Lexeme: +=\n"); i += 2; } 
                else { printf("Token: ARITHMETIC_OP, Lexeme: +\n"); i++; }
                break;
            case '-': 
                if (i + 1 < len && input[i + 1] == '-') { printf("Token: ARITHMETIC_OP, Lexeme: --\n"); i += 2; } 
                else if (i + 1 < len && input[i + 1] == '=') { printf("Token: ASSIGNMENT_OP, Lexeme: -=\n"); i += 2; } 
                else { printf("Token: ARITHMETIC_OP, Lexeme: -\n"); i++; }
                break;
            case '*': 
                if (i + 1 < len && input[i + 1] == '=') { printf("Token: ASSIGNMENT_OP, Lexeme: *=\n"); i += 2; } 
                else if (i + 1 < len && input[i + 1] == '*') { printf("Token: EXPONENT_OP, Lexeme: **\n"); i += 2; } 
                else { printf("Token: ARITHMETIC_OP, Lexeme: *\n"); i++; }
                break;
            case '/': 
                if (i + 1 < len && input[i + 1] == '=') { printf("Token: ASSIGNMENT_OP, Lexeme: /=\n"); i += 2; } 
                else { printf("Token: ARITHMETIC_OP, Lexeme: /\n"); i++; }
                break;
            case '%': 
                if (i + 1 < len && input[i + 1] == '=') { printf("Token: ASSIGNMENT_OP, Lexeme: %%= \n"); i += 2; } 
                else { printf("Token: ARITHMETIC_OP, Lexeme: %%\n"); i++; }
                break;
            case '<': 
                if (i + 1 < len && input[i + 1] == '=') { printf("Token: RELATIONAL_OP, Lexeme: <=\n"); i += 2; } 
                else { printf("Token: RELATIONAL_OP, Lexeme: <\n"); i++; }
                break;
            case '>': 
                if (i + 1 < len && input[i + 1] == '=') { printf("Token: RELATIONAL_OP, Lexeme: >=\n"); i += 2; } 
                else { printf("Token: RELATIONAL_OP, Lexeme: >\n"); i++; }
                break;
            case '=': 
                if (i + 1 < len && input[i + 1] == '=') { printf("Token: RELATIONAL_OP, Lexeme: ==\n"); i += 2; } 
                else { printf("Token: ASSIGNMENT_OP, Lexeme: =\n"); i++; }
                break;
            case '!': 
                if (i + 1 < len && input[i + 1] == '=') { printf("Token: RELATIONAL_OP, Lexeme: !=\n"); i += 2; } 
                else { printf("Token: LOGICAL_OP, Lexeme: !\n"); i++; }
                break;
            case '&': 
                if (i + 1 < len && input[i + 1] == '&') { printf("Token: LOGICAL_OP, Lexeme: &&\n"); i += 2; } 
                else { printf("Token: BITWISE_OP, Lexeme: &\n"); i++; }
                break;
            case '|': 
                if (i + 1 < len && input[i + 1] == '|') { printf("Token: LOGICAL_OP, Lexeme: ||\n"); i += 2; } 
                else { printf("Token: BITWISE_OP, Lexeme: |\n"); i++; }
                break;

            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case ';':
                printf("Token: DELIMITER, Lexeme: %c\n", currentChar);
                i++;
                break;

            case ' ':
            case '\t':
            case '\n':
                // Ignore whitespace
                i++;
                break;

            // --- DEFAULT: HANDLES WORDS (NOISE WORDS/IDENTIFIERS) AND NUMBERS ---
            default:
                if (isalpha(currentChar) || currentChar == '_') {
                    // IDENTIFIER/KEYWORD (Noise Word) Logic
                    char buffer[50]; 
                    int j = 0;
                    // Consume the full word
                    while (i < len && (isalnum(input[i]) || input[i] == '_')) {
                        buffer[j++] = input[i++];
                    }
                    buffer[j] = '\0'; 

                    if (isKeyword(buffer)) {
                        printf("Token: KEYWORD, Lexeme: %s\n", buffer); // Identified as NOISE WORD
                    } else {
                        printf("Token: IDENTIFIER, Lexeme: %s\n", buffer); // Identified as variable
                    }

                } else if (isdigit(currentChar)) {
                    // NUMBER (INT/FLOAT) Logic
                    char buffer[50]; 
                    int j = 0;
                    int is_float = 0;

                    // Consume integers and check for decimal point
                    while (i < len && isdigit(input[i])) {
                        buffer[j++] = input[i++];
                    }
                    if (i < len && input[i] == '.') {
                        is_float = 1;
                        buffer[j++] = input[i++];
                        while (i < len && isdigit(input[i])) {
                            buffer[j++] = input[i++];
                        }
                    }
                    buffer[j] = '\0';
                    
                    if (is_float) {
                        printf("Token: FLOAT_LITERAL, Lexeme: %s\n", buffer);
                    } else {
                        printf("Token: INT_LITERAL, Lexeme: %s\n", buffer);
                    }
                }
                
                else {
                    // Catch-all for truly unknown symbols
                    printf("Token: UNKNOWN_CHAR, Lexeme: %c\n", currentChar);
                    i++;
                }
                break;
        }
    }
    printf("=== Lexical Analysis End ===\n");
}

// --- MAIN FUNCTION to test the analyzer ---
int main() {
    char testCode[] = "int main() { \n"
                      "  if (x >= 10) { \n"
                      "    return 0; \n"
                      "  } \n"
                      "  float data = 1.0; \n"
                      "  while (data < 5) { \n"
                      "    data++; \n"
                      "  } \n"
                      "  return 0; \n"
                      "}";

    printf("Analyzing the following code:\n%s\n\n", testCode);

    lexicalAnalyzer(testCode);

    return 0;
}