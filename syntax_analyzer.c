#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKEN_LEN 1000
#define MAX_ERRORS 100

// Token types from lexical analyzer
typedef enum {
    IDENTIFIER,
    OPERATION,
    KEYWORDS,
    RESERVED_WORDS,
    CONSTANT,
    NOISE_WORDS,
    COMMENT,
    WHITE_SPACE,
    DELIMITER
} TokenType;

// Token structure
typedef struct Token {
    TokenType type;
    char lexeme[MAX_TOKEN_LEN];
    int line;
    int column;
    struct Token* next;
} Token;

// Error storage structure
typedef struct ErrorInfo {
    char message[500];
    int line;
    int column;
    char found[MAX_TOKEN_LEN];
} ErrorInfo;

// Global variables for parsing
Token* current_token = NULL;
Token* token_list = NULL;
FILE* parse_output = NULL;
ErrorInfo errors[MAX_ERRORS];
int error_count = 0;

// Function prototypes
void readTokensFromFile(const char* filename);
void syntaxAnalyzer();
void advance();
bool match(TokenType type, const char* lexeme);
bool matchType(TokenType type);
bool check(TokenType type, const char* lexeme);
bool checkType(TokenType type);
void recordError(const char* message);
void skipWhitespace();
void synchronize();
void skipToSemicolon();
void skipToCloseBrace();

// Grammar rule functions
void parseProgram();
void parseBlock();
void parseStatement();
void parseDecStmt();
void parseAssStmt();
void parseConditionalStmt();
void parseIterativeStmt();
void parseOutputStmt();
void parseInputStmt();
void parseBreakStmt();
void parseExpr();
void parseLogicalOrExpr();
void parseLogicalAndExpr();
void parseEqualityExpr();
void parseRelationalExpr();
void parseAdditiveExpr();
void parseMultiplicativeExpr();
void parseUnaryExpr();
void parsePostfixExpr();
void parsePrimaryExpr();
void parseIdList();
void parseExprList();
bool isDataType();
bool isScopeModifier();

int main() {
    // Read tokens from symbol table
    readTokensFromFile("SymbolTable.txt");
    
    // Open output file for parse results
    parse_output = fopen("ParseOutput.txt", "w");
    if (parse_output == NULL) {
        fprintf(stderr, "Error: Cannot create ParseOutput.txt\n");
        return 1;
    }
       
    // Start syntax analysis
    fprintf(parse_output, "=== SYNTAX ANALYSIS ===\n\n");
    printf("Starting syntax analysis...\n");
    printf("Using: Recursive Descent Parser with Panic Mode Recovery\n\n");
    
    current_token = token_list;
    skipWhitespace();
    
    parseProgram();
    
    // Display all errors at the end
    fprintf(parse_output, "\n=== ANALYSIS COMPLETE ===\n\n");
    
    if (error_count == 0) {
        fprintf(parse_output, "SUCCESS: Your code has no syntax errors.\n");
        printf("\n SUCCESS: Your code has no syntax errors.\n");
    } else {
        fprintf(parse_output, " FOUND %d ERROR(S):\n\n", error_count);
        printf("\n FOUND %d ERROR(S):\n\n", error_count);
        
        for (int i = 0; i < error_count; i++) {
            fprintf(parse_output, "Error %d (Line %d, Column %d):\n", 
                    i + 1, errors[i].line, errors[i].column);
            fprintf(parse_output, "  Problem: %s\n", errors[i].message);
            fprintf(parse_output, "  Found: '%s'\n\n", errors[i].found);
            
            printf("Error %d (Line %d, Column %d):\n", 
                   i + 1, errors[i].line, errors[i].column);
            printf("  Problem: %s\n", errors[i].message);
            printf("  Found: '%s'\n\n", errors[i].found);
        }
        
        fprintf(parse_output, "Total: %d error(s) need to be fixed\n", error_count);
        printf("Total: %d error(s) need to be fixed\n", error_count);
    }
    
    fclose(parse_output);
    printf("\nResults saved to 'ParseOutput.txt'\n");
    
    return error_count > 0 ? 1 : 0;
}

void readTokensFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        exit(1);
    }
    
    char type_str[50];
    char lexeme[MAX_TOKEN_LEN];
    int line, column;
    Token* tail = NULL;
    
    bool in_string = false;
    char string_buffer[MAX_TOKEN_LEN] = "";
    int string_line = 0, string_column = 0;
    
    while (fscanf(file, "%s", type_str) == 1) {
        Token* tok = (Token*)malloc(sizeof(Token));
        
        // Parse token type
        if (strcmp(type_str, "IDENTIFIER") == 0) tok->type = IDENTIFIER;
        else if (strcmp(type_str, "OPERATION") == 0) tok->type = OPERATION;
        else if (strcmp(type_str, "KEYWORDS") == 0) tok->type = KEYWORDS;
        else if (strcmp(type_str, "RESERVED_WORDS") == 0) tok->type = RESERVED_WORDS;
        else if (strcmp(type_str, "CONSTANT") == 0) tok->type = CONSTANT;
        else if (strcmp(type_str, "NOISE_WORDS") == 0) tok->type = NOISE_WORDS;
        else if (strcmp(type_str, "COMMENT") == 0) tok->type = COMMENT;
        else if (strcmp(type_str, "WHITE_SPACE") == 0) tok->type = WHITE_SPACE;
        else if (strcmp(type_str, "DELIMITER") == 0) tok->type = DELIMITER;
        
        fscanf(file, "%s", lexeme);
        
        if (strcmp(lexeme, "\\n") == 0) strcpy(tok->lexeme, "\n");
        else if (strcmp(lexeme, "\\t") == 0) strcpy(tok->lexeme, "\t");
        else if (strcmp(lexeme, "_") == 0) strcpy(tok->lexeme, " ");
        else strcpy(tok->lexeme, lexeme);
        
        fscanf(file, "%d %d", &line, &column);
        tok->line = line;
        tok->column = column;
        
        // Handle string literal reconstruction
        if (tok->type == RESERVED_WORDS && strcmp(tok->lexeme, "\"") == 0) {
            if (!in_string) {
                in_string = true;
                strcpy(string_buffer, "\"");
                string_line = line;
                string_column = column;
                free(tok);
                continue;
            } else {
                strcat(string_buffer, "\"");
                strcpy(tok->lexeme, string_buffer);
                tok->line = string_line;
                tok->column = string_column;
                in_string = false;
                string_buffer[0] = '\0';
            }
        } else if (in_string) {
            if (tok->type == IDENTIFIER) {
                strcat(string_buffer, tok->lexeme);
            }
            free(tok);
            continue;
        }
        
        tok->next = NULL;
        
        if (token_list == NULL) {
            token_list = tok;
            tail = tok;
        } else {
            tail->next = tok;
            tail = tok;
        }
    }
    
    fclose(file);
}

void skipWhitespace() {
    while (current_token != NULL && 
           (current_token->type == WHITE_SPACE || current_token->type == COMMENT)) {
        current_token = current_token->next;
    }
}

void advance() {
    if (current_token != NULL) {
        current_token = current_token->next;
        skipWhitespace();
    }
}

bool match(TokenType type, const char* lexeme) {
    if (check(type, lexeme)) {
        advance();
        return true;
    }
    if (type == KEYWORDS && current_token != NULL && 
        current_token->type == IDENTIFIER && strcmp(current_token->lexeme, lexeme) == 0) {
        advance();
        return true;
    }
    return false;
}

bool matchType(TokenType type) {
    if (checkType(type)) {
        advance();
        return true;
    }
    return false;
}

bool check(TokenType type, const char* lexeme) {
    if (current_token == NULL) return false;
    return current_token->type == type && strcmp(current_token->lexeme, lexeme) == 0;
}

bool checkType(TokenType type) {
    if (current_token == NULL) return false;
    return current_token->type == type;
}

void recordError(const char* message) {
    if (error_count >= MAX_ERRORS) return;
    
    strcpy(errors[error_count].message, message);
    if (current_token != NULL) {
        errors[error_count].line = current_token->line;
        errors[error_count].column = current_token->column;
        strcpy(errors[error_count].found, current_token->lexeme);
    } else {
        errors[error_count].line = -1;
        errors[error_count].column = -1;
        strcpy(errors[error_count].found, "end of file");
    }
    error_count++;
}

// Panic mode recovery: skip to semicolon
void skipToSemicolon() {
    while (current_token != NULL && !check(DELIMITER, ";")) {
        // Also stop at closing brace to avoid skipping too much
        if (check(DELIMITER, "}")) {
            return;
        }
        advance();
    }
    if (check(DELIMITER, ";")) {
        advance(); // consume the semicolon
    }
}

// Panic mode recovery: skip to closing brace
void skipToCloseBrace() {
    int brace_count = 1;
    while (current_token != NULL && brace_count > 0) {
        if (check(DELIMITER, "{")) {
            brace_count++;
        } else if (check(DELIMITER, "}")) {
            brace_count--;
            if (brace_count == 0) {
                return; // Don't consume the closing brace
            }
        }
        advance();
    }
}

bool isDataType() {
    if (current_token == NULL || current_token->type != KEYWORDS) return false;
    return strcmp(current_token->lexeme, "int") == 0 ||
           strcmp(current_token->lexeme, "float") == 0 ||
           strcmp(current_token->lexeme, "char") == 0 ||
           strcmp(current_token->lexeme, "text") == 0 ||
           strcmp(current_token->lexeme, "bool") == 0 ||
           strcmp(current_token->lexeme, "time") == 0 ||
           strcmp(current_token->lexeme, "date") == 0 ||
           strcmp(current_token->lexeme, "timestamp") == 0;
}

bool isScopeModifier() {
    if (current_token == NULL || current_token->type != KEYWORDS) return false;
    return strcmp(current_token->lexeme, "let") == 0 ||
           strcmp(current_token->lexeme, "var") == 0 ||
           strcmp(current_token->lexeme, "out") == 0 ||
           strcmp(current_token->lexeme, "in") == 0 ||
           strcmp(current_token->lexeme, "only") == 0;
}

void parseProgram() {
    fprintf(parse_output, "Parsing PROGRAM...\n");
    
    // Check for 'func' keyword (optional)
    if (current_token != NULL && strcmp(current_token->lexeme, "func") == 0) {
        advance();
    }
    
    // 'main' keyword - might be IDENTIFIER or KEYWORDS depending on lexer
    if (current_token != NULL && strcmp(current_token->lexeme, "main") == 0) {
        advance();
    } else {
        recordError("Missing 'main' at the start of your program");
        // Try to recover by looking for ':'
        while (current_token != NULL && !check(DELIMITER, ":")) {
            advance();
        }
    }
    
    if (!match(DELIMITER, ":")) {
        recordError("Missing ':' after 'main'");
        // Continue anyway to find more errors
    }
    
    // Check if there's a block with braces or just statements
    if (check(DELIMITER, "{")) {
        // Traditional block with braces
        parseBlock();
    } else {
        // No braces - parse all statements until end of file
        fprintf(parse_output, "  Parsing statements without block braces...\n");
        while (current_token != NULL) {
            Token* before = current_token;
            parseStatement();
            
            // If we're stuck on the same token, skip it to prevent infinite loop
            if (current_token == before && current_token != NULL) {
                fprintf(parse_output, "  Warning: Skipping stuck token '%s'\n", current_token->lexeme);
                advance();
            }
        }
        fprintf(parse_output, "  All statements parsed.\n");
    }
    
    fprintf(parse_output, "PROGRAM parsing done.\n");
}

void parseBlock() {
    fprintf(parse_output, "  Parsing BLOCK...\n");
    
    if (!match(DELIMITER, "{")) {
        recordError("Missing '{' to start a block");
        // Try to continue parsing statements
    }
    
    while (current_token != NULL && !check(DELIMITER, "}")) {
        Token* before = current_token;
        parseStatement();
        
        // If we're stuck on the same token, skip it to prevent infinite loop
        if (current_token == before && current_token != NULL) {
            fprintf(parse_output, "  Warning: Skipping stuck token '%s'\n", current_token->lexeme);
            advance();
        }
    }
    
    if (!match(DELIMITER, "}")) {
        recordError("Missing '}' to close a block");
    } else {
        fprintf(parse_output, "  BLOCK closed properly.\n");
    }
    
    fprintf(parse_output, "  BLOCK parsing done.\n");
}

void parseStatement() {
    if (current_token == NULL) {
        recordError("Unexpected end of code");
        return;
    }
    
    // Save position to check for 'if' keyword that might be classified as IDENTIFIER
    bool is_if_keyword = (current_token->type == IDENTIFIER && 
                          strcmp(current_token->lexeme, "if") == 0);
    
    // Declaration statement
    if (isScopeModifier() || isDataType() || check(KEYWORDS, "cons")) {
        parseDecStmt();
    }
    // Conditional statement - check for 'if' as IDENTIFIER too
    else if (check(KEYWORDS, "do") || check(KEYWORDS, "compare") || 
             (check(KEYWORDS, "if") || is_if_keyword)) {
        parseConditionalStmt();
    }
    // Iterative statement
    else if (check(KEYWORDS, "continue") || check(KEYWORDS, "stop")) {
        parseIterativeStmt();
    }
    // Output statement
    else if (check(KEYWORDS, "display")) {
        parseOutputStmt();
    }
    // Input statement
    else if (check(KEYWORDS, "put")) {
        parseInputStmt();
    }
    // Break statement
    else if (check(KEYWORDS, "break") || check(KEYWORDS, "back")) {
        parseBreakStmt();
    }
    // Assignment statement
    else if (checkType(IDENTIFIER)) {
        parseAssStmt();
    }
    else {
        recordError("This doesn't look like a valid statement");
        skipToSemicolon(); // Recovery: skip to next statement
    }
}

void parseDecStmt() {
    // Check for 'cons' (constant)
    if (match(KEYWORDS, "cons")) {
        if (!isDataType()) {
            recordError("Missing data type after 'cons' (like int, float, text)");
            skipToSemicolon();
            return;
        }
        advance();
        
        if (!matchType(IDENTIFIER)) {
            recordError("Missing variable name after data type");
            skipToSemicolon();
            return;
        }
        
        if (!match(OPERATION, "=")) {
            recordError("Constant needs '=' and a value");
            skipToSemicolon();
            return;
        }
        
        parseExpr();
        
        if (!match(DELIMITER, ";")) {
            recordError("Missing ';' at the end of this line");
            skipToSemicolon();
        }
        return;
    }
    
    // Optional scope modifier
    if (isScopeModifier()) {
        advance();
    }
    
    // Data type (required)
    if (!isDataType()) {
        recordError("Missing data type (like int, float, text)");
        skipToSemicolon();
        return;
    }
    advance();
    
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name");
        skipToSemicolon();
        return;
    }
    
    // Check what comes next
    if (match(OPERATION, "=")) {
        parseExpr();
        
        while (match(DELIMITER, ",")) {
            if (!matchType(IDENTIFIER)) {
                recordError("Missing variable name after ','");
                skipToSemicolon();
                return;
            }
            if (match(OPERATION, "=")) {
                parseExpr();
            }
        }
        
        if (!match(DELIMITER, ";")) {
            recordError("Missing ';' at the end of this line");
            skipToSemicolon();
        }
    }
    else if (match(DELIMITER, ",")) {
        parseIdList();
        if (!match(DELIMITER, ";")) {
            recordError("Missing ';' at the end of this line");
            skipToSemicolon();
        }
    }
    else if (match(DELIMITER, ";")) {
        // Simple declaration is fine
    }
    else {
        recordError("Expected ';', '=', or ',' after variable name");
        skipToSemicolon();
    }
}

void parseIdList() {
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name in the list");
        return;
    }
    
    if (match(OPERATION, "=")) {
        parseExpr();
    }
    
    while (match(DELIMITER, ",")) {
        if (!matchType(IDENTIFIER)) {
            recordError("Missing variable name after ','");
            return;
        }
        if (match(OPERATION, "=")) {
            parseExpr();
        }
    }
}

void parseAssStmt() {
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name");
        skipToSemicolon();
        return;
    }
    
    // Check for assignment operators
    if (!(check(OPERATION, "=") || check(OPERATION, "+=") || check(OPERATION, "-=") ||
          check(OPERATION, "*=") || check(OPERATION, "/=") || check(OPERATION, "%="))) {
        recordError("Missing '=' for assignment");
        skipToSemicolon();
        return;
    }
    
    // Consume the assignment operator
    advance();
    
    parseExpr();
    
    if (!match(DELIMITER, ";")) {
        recordError("Missing ';' at the end of this line");
        skipToSemicolon();
    }
}

void parseConditionalStmt() {
    if (match(KEYWORDS, "do")) {
        // Handle 'if' that might be classified as IDENTIFIER
        bool matched_if = false;
        if (match(KEYWORDS, "if")) {
            matched_if = true;
        } else if (current_token != NULL && current_token->type == IDENTIFIER && 
                   strcmp(current_token->lexeme, "if") == 0) {
            advance();
            matched_if = true;
        }
        
        if (!matched_if) {
            recordError("Missing 'if' after 'do'");
            skipToSemicolon();
            return;
        }
        
        if (!match(DELIMITER, "(")) {
            recordError("Missing '(' after 'if'");
            skipToSemicolon();
            return;
        }
        
        parseExpr();
        
        if (!match(DELIMITER, ")")) {
            recordError("Missing ')' after condition");
            // Don't skip, try to continue with block
        }
        
        if (check(DELIMITER, "{")) {
            parseBlock();
        } else {
            parseStatement();
        }
        
        if (match(KEYWORDS, "then")) {
            if (!match(KEYWORDS, "do")) {
                recordError("Missing 'do' after 'then'");
            }
            
            if (check(DELIMITER, "{")) {
                parseBlock();
            } else {
                parseStatement();
            }
        }
    }
    else if (match(KEYWORDS, "compare")) {
        parseExpr();
        
        if (!match(DELIMITER, "{")) {
            recordError("Missing '{' after compare");
            return;
        }
        
        while (match(KEYWORDS, "what")) {
            if (!match(KEYWORDS, "if")) {
                recordError("Missing 'if' after 'what'");
                continue;
            }
            
            parseExpr();
            
            if (!match(DELIMITER, ":")) {
                recordError("Missing ':' after case value");
            }
            
            while (current_token != NULL && !check(KEYWORDS, "break") && 
                   !check(KEYWORDS, "what") && !check(KEYWORDS, "then") &&
                   !check(DELIMITER, "}")) {
                parseStatement();
            }
            
            if (!match(KEYWORDS, "break")) {
                recordError("Missing 'break' at end of case");
            }
            if (!match(DELIMITER, ";")) {
                recordError("Missing ';' after 'break'");
            }
        }
        
        if (match(KEYWORDS, "then")) {
            if (!match(KEYWORDS, "do")) {
                recordError("Missing 'do' after 'then'");
            }
            if (!match(DELIMITER, ":")) {
                recordError("Missing ':' after 'then do'");
            }
            
            while (current_token != NULL && !check(DELIMITER, "}")) {
                parseStatement();
            }
        }
        
        if (!match(DELIMITER, "}")) {
            recordError("Missing '}' at end of compare");
        }
    }
    // Handle standalone 'if' (in case it's classified as IDENTIFIER)
    else if (current_token != NULL && current_token->type == IDENTIFIER && 
             strcmp(current_token->lexeme, "if") == 0) {
        // This is 'if' without 'do', treat as error or allow it
        recordError("Found 'if' without 'do' before it");
        advance();
        
        if (!match(DELIMITER, "(")) {
            recordError("Missing '(' after 'if'");
            skipToSemicolon();
            return;
        }
        
        parseExpr();
        
        if (!match(DELIMITER, ")")) {
            recordError("Missing ')' after condition");
        }
        
        if (check(DELIMITER, "{")) {
            parseBlock();
        } else {
            parseStatement();
        }
    }
}

void parseIterativeStmt() {
    if (match(KEYWORDS, "continue")) {
        if (!match(KEYWORDS, "until")) {
            recordError("Missing 'until' after 'continue'");
            skipToSemicolon();
            return;
        }
        
        if (!match(DELIMITER, "(")) {
            recordError("Missing '(' after 'until'");
            skipToSemicolon();
            return;
        }
        
        parseExpr();
        
        if (match(DELIMITER, ";")) {
            parseExpr();
            
            if (!match(DELIMITER, ";")) {
                recordError("Missing ';' in loop");
            }
            
            parseExpr();
        }
        
        if (!match(DELIMITER, ")")) {
            recordError("Missing ')' after loop condition");
        }
        
        if (check(DELIMITER, "{")) {
            parseBlock();
        } else {
            parseStatement();
        }
    }
    else if (match(KEYWORDS, "stop")) {
        if (!match(KEYWORDS, "when")) {
            recordError("Missing 'when' after 'stop'");
            skipToSemicolon();
            return;
        }
        
        if (!match(DELIMITER, "(")) {
            recordError("Missing '(' after 'when'");
            skipToSemicolon();
            return;
        }
        
        parseExpr();
        
        if (!match(DELIMITER, ")")) {
            recordError("Missing ')' after condition");
        }
        
        if (check(DELIMITER, "{")) {
            parseBlock();
        } else {
            parseStatement();
        }
    }
}

void parseOutputStmt() {
    if (!match(KEYWORDS, "display")) {
        recordError("Missing 'display' keyword");
        skipToSemicolon();
        return;
    }
    
    parseExprList();
    
    if (!match(DELIMITER, ";")) {
        recordError("Missing ';' at the end of display");
        skipToSemicolon();
    }
}

void parseInputStmt() {
    if (!match(KEYWORDS, "put")) {
        recordError("Missing 'put' keyword");
        skipToSemicolon();
        return;
    }
    
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name after 'put'");
        skipToSemicolon();
        return;
    }
    
    if (!match(DELIMITER, ";")) {
        recordError("Missing ';' at the end of put");
        skipToSemicolon();
    }
}

void parseBreakStmt() {
    if (match(KEYWORDS, "break") || match(KEYWORDS, "back")) {
        if (!match(DELIMITER, ";")) {
            recordError("Missing ';' after break/back");
            skipToSemicolon();
        }
    }
}

void parseExprList() {
    parseExpr();
    
    while (match(DELIMITER, ",")) {
        parseExpr();
    }
}

void parseExpr() {
    parseLogicalOrExpr();
}

void parseLogicalOrExpr() {
    parseLogicalAndExpr();
    while (match(OPERATION, "||")) {
        parseLogicalAndExpr();
    }
}

void parseLogicalAndExpr() {
    parseEqualityExpr();
    while (match(OPERATION, "&&")) {
        parseEqualityExpr();
    }
}

void parseEqualityExpr() {
    parseRelationalExpr();
    while (match(OPERATION, "==") || match(OPERATION, "!=")) {
        parseRelationalExpr();
    }
}

void parseRelationalExpr() {
    parseAdditiveExpr();
    while (match(OPERATION, "<") || match(OPERATION, ">") || 
           match(OPERATION, "<=") || match(OPERATION, ">=")) {
        parseAdditiveExpr();
    }
}

void parseAdditiveExpr() {
    parseMultiplicativeExpr();
    while (match(OPERATION, "+") || match(OPERATION, "-")) {
        parseMultiplicativeExpr();
    }
}

void parseMultiplicativeExpr() {
    parseUnaryExpr();
    while (match(OPERATION, "*") || match(OPERATION, "/") || match(OPERATION, "%")) {
        parseUnaryExpr();
    }
}

void parseUnaryExpr() {
    if (match(OPERATION, "+") || match(OPERATION, "-") || 
        match(OPERATION, "!") || match(OPERATION, "++") || match(OPERATION, "--")) {
        parseUnaryExpr();
    } else {
        parsePostfixExpr();
    }
}

void parsePostfixExpr() {
    parsePrimaryExpr();
    while (match(OPERATION, "++") || match(OPERATION, "--")) {
        // Postfix operators handled
    }
}

void parsePrimaryExpr() {
    if (matchType(IDENTIFIER)) {
        return;
    }
    else if (matchType(CONSTANT)) {
        return;
    }
    else if (checkType(RESERVED_WORDS)) {
        advance();
        return;
    }
    else if (match(DELIMITER, "(")) {
        parseExpr();
        if (!match(DELIMITER, ")")) {
            recordError("Missing ')' in expression");
        }
        return;
    }
    else {
        recordError("Invalid expression");
        // Try to recover
        advance();
    }
}