#include <stdio.h>
#include <string.h>
#include <ctype.h> /

// 1. Define the list of "Noise Words" (Keywords)
const char* keywords[] = {
    "then", "end"
};
const int KEYWORD_COUNT = sizeof(keywords) / sizeof(keywords[0]);

// Function to check if a word is a keyword
int isKeyword(char* word) {
    for (int k = 0; k < KEYWORD_COUNT; k++) {
        if (strcmp(keywords[k], word) == 0) {
            return 1; // It is a keyword (noise word)
        }
    }
    return 0; // It is a regular identifier
}

// --- MAIN ANALYZER FUNCTION  ---
void lexicalAnalyzer(char* input) {
    int i = 0;
    int len = strlen(input);
    
    printf("=== Lexical Analysis Start === \n");

    while (i < len) {
        char currentChar = input[i];

        // 1. Check for a word (Keyword or Identifier)
        if (isalpha(currentChar) || currentChar == '_') {
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
        } 
        
        // 2. Check for whitespace (handles ' ', '\t', '\n')
        else if (isspace(currentChar)) {
            i++; // Ignore whitespace
        } 
        
        // 3. Handle everything else (operators, numbers, punctuation)
        else {
            printf("Token: OTHER_CHAR, Lexeme: %c\n", currentChar);
            i++;
        }
    }
    printf("=== Lexical Analysis End ===\n");
}

// --- MAIN FUNCTION to test the new analyzer ---
int main() {
    char testCode[] = "if x > 10 then \n"
                      "  y = 20 \n"
                      "end \n"
                      "int 55 + (end)";

    printf("Analyzing the following code:\n%s\n\n", testCode);

    lexicalAnalyzer(testCode);

    return 0;
}