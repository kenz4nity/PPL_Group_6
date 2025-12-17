int currentLine = 1;
int currentColumn = 1;

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
int indent_level = 0;

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
void printIndent();
void logParsing(const char* rule);
void logConsumed(Token* tok);
void logEntering(const char* rule);
void logExiting(const char* rule, bool success);
void logError(const char* message);

void advance_token(void) {
    if (current_token != NULL) {
        current_token = current_token->next;
    }

    /* Skip whitespace tokens */
    while (current_token != NULL &&
           current_token->type == WHITE_SPACE) {
        current_token = current_token->next;
    }
}

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
void parseAssignExpr();
void parseConditionalExpr();
void parseLogicalOrExpr();
void parseLogicalAndExpr();
void parseBitwiseOrExpr();
void parseBitwiseXorExpr();
void parseBitwiseAndExpr();
void parseEqualityExpr();
void parseRelationalExpr();
void parseShiftExpr();
void parseAdditiveExpr();
void parseMultiplicativeExpr();
void parseUnaryExpr();
void parsePostfixExpr();
void parsePrimaryExpr();
void parseIdList();
void parseExprList();
bool isDataType();
bool isScopeModifier();
bool isAssignmentOp();

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
    fprintf(parse_output, "=== PARSING PROGRAM ===\n");
    printf("Starting syntax analysis...\n");
    printf("Using: Recursive Descent Parser with Detailed Token-by-Token Parsing\n\n");
    
    current_token = token_list;

    while (current_token != NULL &&
        current_token->type == WHITE_SPACE) {
        advance_token();
    }

    skipWhitespace();
    
    parseProgram();
    
    // Display summary
    fprintf(parse_output,
            "\n===== SYNTAX ANALYSIS COMPLETE =====\n"
            "Result: %s\n\n",
            error_count > 0 ? "FAILED (With Errors)" : "SUCCESS");

    if (error_count == 0) {
        fprintf(parse_output, "No syntax errors found.\n");
        printf("\nSUCCESS: Your code has no syntax errors.\n");
    } else {
        fprintf(parse_output,
                "Total syntax errors: %d\n\n",
                error_count);

        for (int i = 0; i < error_count; i++) {
            fprintf(parse_output,
                    "[%d] Line %d, Column %d\n"
                    "    %s\n\n",
                    i + 1,
                    errors[i].line,
                    errors[i].column,
                    errors[i].message);
        }

        printf("\nTotal syntax errors found: %d\n", error_count);
    }

    fclose(parse_output);
    printf("\nResults saved to 'ParseOutput.txt'\n");
    
    return error_count > 0 ? 1 : 0;
}

void printIndent() {
    for (int i = 0; i < indent_level; i++) {
        fprintf(parse_output, "  ");
    }
}

void logParsing(const char* rule) {
    printIndent();
    fprintf(parse_output, "[Parsing] %s\n", rule);
}

void logEntering(const char* rule) {
    printIndent();
    fprintf(parse_output, "Entering %s\n", rule);
    indent_level++;
}

void logExiting(const char* rule, bool success) {
    indent_level--;
    printIndent();
    fprintf(parse_output, "Exiting %s (%s)\n", rule, success ? "Success" : "Failure");
}

void logConsumed(Token* tok) {
    if (tok == NULL) return;
    printIndent();
    fprintf(parse_output, "Consumed: <%s, ", tok->lexeme);
    
    // Print token type
    switch(tok->type) {
        case IDENTIFIER: fprintf(parse_output, "identifier"); break;
        case OPERATION: fprintf(parse_output, "operation"); break;
        case KEYWORDS: fprintf(parse_output, "keyword"); break;
        case RESERVED_WORDS: fprintf(parse_output, "reserved"); break;
        case CONSTANT: fprintf(parse_output, "constant"); break;
        case DELIMITER: fprintf(parse_output, "delimiter"); break;
        default: fprintf(parse_output, "token"); break;
    }
    
    fprintf(parse_output, "> (line %d, col %d)\n", tok->line, tok->column);
}

void syntax_error(const char* message) {
    if (current_token == NULL) return;

    printIndent();
    fprintf(parse_output,
            ">> SYNTAX ERROR at line %d, col %d: %s (found '%s')\n",
            current_token->line,
            current_token->column,
            message,
            current_token->lexeme);

    /* Store error */
    if (error_count < MAX_ERRORS) {
        errors[error_count].line = current_token->line;
        errors[error_count].column = current_token->column;
        strncpy(errors[error_count].message, message, 255);
        error_count++;
    }

    panic_recover();
}

void panic_recover(void) {
    printIndent();
    fprintf(parse_output, ">> PANIC MODE: recovering...\n");

    while (current_token != NULL) {

        /* Synchronization tokens */
        if (current_token->type == DELIMITER) {
            char ch = current_token->lexeme[0];

            if (ch == ';' || ch == '}' || ch == ')') {
                advance();   // consume sync token
                return;
            }
        }

        advance();
    }
}

void consume_current(void) {
    logConsumed(current_token);
    advance();
}

void logError(const char* message) {
    printIndent();
    fprintf(parse_output, "\nERROR: %s", message);
    if (current_token != NULL) {
        fprintf(parse_output, " at line %d col %d\n", current_token->line, current_token->column);
    } else {
        fprintf(parse_output, " at end of file\n");
    }
}

void readTokensFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        exit(1);
    }
    
    char type_str[50];
    char lexeme[MAX_TOKEN_LEN];
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
        
        /*fscanf(file, "%d %d", &line, &column);
        tok->line = line;
        tok->column = column;*/

        tok->line = currentLine;
        tok->column = currentColumn;

        if (tok->type == WHITE_SPACE) {

        char ch = tok->lexeme[0];

            if (ch == '\n') {
                currentLine++;
                currentColumn = 1;
            }
            else if (ch == '\t') {
                currentColumn += 4;
            }
            else {
                currentColumn += 1;  // space or other whitespace
            }

        } else {
            currentColumn += strlen(tok->lexeme);
        }


        // Handle string literal reconstruction
        if (tok->type == RESERVED_WORDS && strcmp(tok->lexeme, "\"") == 0) {
            if (!in_string) {
                    in_string = true;
                    strcpy(string_buffer, "\"");

                    string_line = tok->line;
                    string_column = tok->column;

                    free(tok);
                    continue;
            }
            else {
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
        advance_token();
    }
}

void advance() {
    if (current_token != NULL) {
        advance_token();
        skipWhitespace();
    }
}

bool match(TokenType type, const char* lexeme) {
    if (check(type, lexeme)) {
        Token* matched = current_token;
        advance();
        logConsumed(matched);
        return true;
    }
    return false;
}

bool expect(TokenType type, const char* lexeme, const char* message) {
    if (match(type, lexeme)) return true;
    syntax_error(message);
    return false;
}


bool matchType(TokenType type) {
    if (checkType(type)) {
        Token* matched = current_token;
        advance();
        logConsumed(matched);
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
    logError(message);
}

void skipToSemicolon() {
    printIndent();
    fprintf(parse_output, ">> PANIC MODE: Skipping tokens until synchronization point...\n");
    
    while (current_token != NULL && !check(DELIMITER, ";")) {
        if (check(DELIMITER, "}")) {
            printIndent();
            fprintf(parse_output, ">> Recovered at line %d col %d (token: <%s>)\n\n", 
                    current_token->line, current_token->column, current_token->lexeme);
            return;
        }
        advance();
    }
    if (check(DELIMITER, ";")) {
        Token* semi = current_token;
        advance();
        logConsumed(semi);
        printIndent();
        fprintf(parse_output, ">> Recovered at line %d col %d (token: <%s>)\n\n", 
                current_token ? current_token->line : -1, 
                current_token ? current_token->column : -1,
                current_token ? current_token->lexeme : "EOF");
    }
}

void skipToCloseBrace() {
    int brace_count = 1;
    while (current_token != NULL && brace_count > 0) {
        if (check(DELIMITER, "{")) {
            brace_count++;
        } else if (check(DELIMITER, "}")) {
            brace_count--;
            if (brace_count == 0) {
                return;
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

bool isAssignmentOp() {
    if (current_token == NULL || current_token->type != OPERATION) return false;
    return strcmp(current_token->lexeme, "=") == 0 ||
           strcmp(current_token->lexeme, "+=") == 0 ||
           strcmp(current_token->lexeme, "-=") == 0 ||
           strcmp(current_token->lexeme, "*=") == 0 ||
           strcmp(current_token->lexeme, "/=") == 0 ||
           strcmp(current_token->lexeme, "%=") == 0 ||
           strcmp(current_token->lexeme, "<<=") == 0 ||
           strcmp(current_token->lexeme, ">>=") == 0 ||
           strcmp(current_token->lexeme, "&=") == 0 ||
           strcmp(current_token->lexeme, "^=") == 0 ||
           strcmp(current_token->lexeme, "|=") == 0;
}

void parseProgram() {
    logEntering("PROGRAM");
    
    // Optional 'func' keyword
    if (current_token != NULL && 
        (check(KEYWORDS, "func") || 
         (current_token->type == IDENTIFIER && strcmp(current_token->lexeme, "func") == 0))) {
        match(current_token->type, "func");
    }
    
    // 'main' keyword
    if (current_token != NULL && strcmp(current_token->lexeme, "main") == 0) {
        match(current_token->type, "main");
    } else {
        recordError("Missing 'main' at the start of your program");
        while (current_token != NULL && !check(DELIMITER, ":")) {
            advance();
        }
    }
    
    if (!match(DELIMITER, ":")) {
        recordError("Missing ':' after 'main'");
    }
    
    // Parse block or statements
    if (check(DELIMITER, "{")) {
        parseBlock();
    } else {
        while (current_token != NULL) {
            Token* before = current_token;
            parseStatement();
            
            if (current_token == before && current_token != NULL) {
                printIndent();
                fprintf(parse_output, "Warning: Skipping stuck token\n");
                advance();
            }
        }
    }
    
    logExiting("PROGRAM", error_count == 0);
}

void parseBlock() {
    logEntering("BLOCK");
    
    if (!match(DELIMITER, "{")) {
        recordError("Missing '{' to start a block");
    }
    
    while (current_token != NULL && !check(DELIMITER, "}")) {
        Token* before = current_token;
        parseStatement();
        
        if (current_token == before && current_token != NULL) {
            advance();
        }
    }
    
    if (!match(DELIMITER, "}")) {
        recordError("Missing '}' to close a block");
        logExiting("BLOCK", false);
    } else {
        logExiting("BLOCK", true);
    }
}

void parseStatement() {

    logEntering("STATEMENT");

    if (current_token == NULL) {
        recordError("Unexpected end of code");
        logExiting("STATEMENT", false);
        return;
    }
    
    bool success = true;
    
    // Declaration statement
    if (check(KEYWORDS, "cons") || isScopeModifier() || isDataType()) {
        parseDecStmt();
    }
    // Conditional statement
    else if (check(KEYWORDS, "do") || check(KEYWORDS, "compare")) {
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
    // Break/control flow statement
    else if (check(KEYWORDS, "break") || check(KEYWORDS, "back")) {
        parseBreakStmt();
    }
    // Assignment statement
    else if (checkType(IDENTIFIER)) {
        parseAssStmt();
    }
    else {
        recordError("This doesn't look like a valid statement");
        skipToSemicolon();
        success = false;
    }
    
    logExiting("STATEMENT", success && error_count == 0);
}

void parseDecStmt() {
    logEntering("DEC_STMT");
    logParsing("DATA_TYPE");
    
    bool success = true;
    
    // Handle 'cons' (constant)
    if (match(KEYWORDS, "cons")) {
        if (!isDataType()) {
            recordError("Missing data type after 'cons'");
            skipToSemicolon();
            logExiting("DEC_STMT", false);
            return;
        }
        matchType(KEYWORDS);
        
        if (!matchType(IDENTIFIER)) {
            recordError("Missing variable name");
            skipToSemicolon();
            logExiting("DEC_STMT", false);
            return;
        }
        
        if (!match(OPERATION, "=")) {
            recordError("Constant needs '=' and a value");
            skipToSemicolon();
            logExiting("DEC_STMT", false);
            return;
        }
        
        parseExpr();
        
        if (!match(DELIMITER, ";")) {
            recordError("Missing ';' at end of statement");
            skipToSemicolon();
            success = false;
        }
        
        logExiting("DEC_STMT", success);
        return;
    }
    
    // Optional scope modifier
    if (isScopeModifier()) {
        matchType(KEYWORDS);
    }
    
    // Data type (required)
    if (!isDataType()) {
        recordError("Missing data type");
        skipToSemicolon();
        logExiting("DEC_STMT", false);
        return;
    }
    matchType(KEYWORDS);
    
    // Variable name
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name");
        skipToSemicolon();
        logExiting("DEC_STMT", false);
        return;
    }
    
    // Check for initialization or list
    if (match(OPERATION, "=")) {
        parseExpr();
        
        while (match(DELIMITER, ",")) {
            if (!matchType(IDENTIFIER)) {
                recordError("Missing variable name after ','");
                skipToSemicolon();
                logExiting("DEC_STMT", false);
                return;
            }
            if (match(OPERATION, "=")) {
                parseExpr();
            }
        }
        
        if (!match(DELIMITER, ";")) {
            recordError("Missing ';' at end of statement");
            skipToSemicolon();
            success = false;
        }
    }
    else if (match(DELIMITER, ",")) {
        parseIdList();
        if (!match(DELIMITER, ";")) {
            recordError("Missing ';' at end of statement");
            skipToSemicolon();
            success = false;
        }
    }
    else if (match(DELIMITER, ";")) {
        // Simple declaration is fine
    }
    else {
        recordError("Expected ';', '=', or ',' after variable name");
        skipToSemicolon();
        success = false;
    }
    
    logExiting("DEC_STMT", success);
}

void parseIdList() {
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name in list");
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
    logEntering("ASSIGN_STMT");
    
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name");
        skipToSemicolon();
        logExiting("ASSIGN_STMT", false);
        return;
    }
    
    if (!isAssignmentOp()) {
        recordError("Expects an assignment operator (e.g., '=', '+=', '*=')");
        skipToSemicolon();
        logExiting("ASSIGN_STMT", false);
        return;
    }
    matchType(OPERATION);
    
    parseExpr();
    
    if (!match(DELIMITER, ";")) {
        recordError("Missing ';' at end of statement");
        skipToSemicolon();
        logExiting("ASSIGN_STMT", false);
        return;
    }
    
    logExiting("ASSIGN_STMT", true);
}

void parseConditionalStmt() {
    logEntering("CONDITIONAL");
    
    // 'do if' statement
    if (match(KEYWORDS, "do")) {
        if (!(match(KEYWORDS, "if"))) {
            recordError("Missing 'if' after 'do'");
            skipToSemicolon();
            logExiting("CONDITIONAL", false);
            return;
        }

        if (!match(DELIMITER, "(")) {
            recordError("Missing '(' after 'if'");
            skipToSemicolon();
            logExiting("CONDITIONAL", false);
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

    // 'compare' statement
    else if (match(KEYWORDS, "compare")) {
        parseExpr();
        
        if (!match(DELIMITER, "{")) {
            recordError("Missing '{' after compare");
            logExiting("CONDITIONAL", false);
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
            
            while (current_token != NULL && 
                   !check(KEYWORDS, "break") && 
                   !check(KEYWORDS, "what") && 
                   !check(KEYWORDS, "then") &&
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
    
    logExiting("CONDITIONAL", true);
}

void parseIterativeStmt() {
    logEntering("LOOP (for, while, do-while)");
    
    if (match(KEYWORDS, "continue")) {
        logEntering("FOR/WHILE_LOOP");
        
        if (!match(KEYWORDS, "until")) {
            recordError("Missing 'until' after 'continue'");
            skipToCloseBrace();
            logExiting("FOR/WHILE_LOOP", false);
            logExiting("LOOP (for, while, do-while)", false);
            return;
        }
        
        if (!match(DELIMITER, "(")) {
            recordError("Missing '(' after 'until'");
            skipToCloseBrace();
            logExiting("FOR/WHILE_LOOP", false);
            logExiting("LOOP (for, while, do-while)", false);
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
        
        logExiting("FOR/WHILE_LOOP", true);
    }
    else if (match(KEYWORDS, "stop")) {
        logEntering("DO_WHILE_LOOP");
        
        if (!match(KEYWORDS, "when")) {
            recordError("Missing 'when' after 'stop'");
            skipToCloseBrace();
            logExiting("DO_WHILE_LOOP", false);
            logExiting("LOOP (for, while, do-while)", false);
            return;
        }
        
        if (!match(DELIMITER, "(")) {
            recordError("Missing '(' after 'when'");
            skipToCloseBrace();
            logExiting("DO_WHILE_LOOP", false);
            logExiting("LOOP (for, while, do-while)", false);
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
        
        logExiting("DO_WHILE_LOOP", true);
    }
    
    logExiting("LOOP (for, while, do-while)", true);
}

void parseOutputStmt() {
    logEntering("OUTPUT_STMT");
    
    if (!match(KEYWORDS, "display")) {
        recordError("Missing 'display' keyword");
        skipToSemicolon();
        logExiting("OUTPUT_STMT", false);
        return;
    }
    
    parseExprList();
    
    if (!match(DELIMITER, ";")) {
        recordError("Missing ';' at end of display");
        skipToSemicolon();
        logExiting("OUTPUT_STMT", false);
        return;
    }
    
    logExiting("OUTPUT_STMT", true);
}

void parseInputStmt() {
    logEntering("INPUT_STMT");
    
    if (!match(KEYWORDS, "put")) {
        recordError("Missing 'put' keyword");
        skipToSemicolon();
        logExiting("INPUT_STMT", false);
        return;
    }
    
    if (!matchType(IDENTIFIER)) {
        recordError("Missing variable name after 'put'");
        skipToSemicolon();
        logExiting("INPUT_STMT", false);
        return;
    }
    
    if (!match(DELIMITER, ";")) {
        recordError("Missing ';' at end of put");
        skipToSemicolon();
        logExiting("INPUT_STMT", false);
        return;
    }
    
    logExiting("INPUT_STMT", true);
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
    logParsing("EXPRESSION");
    parseAssignExpr();
}

void parseAssignExpr() {
    parseConditionalExpr();
    
    if (isAssignmentOp()) {
        matchType(OPERATION);
        parseAssignExpr();
    }
}

void parseConditionalExpr() {
    parseLogicalOrExpr();
    
    if (match(OPERATION, "?")) {
        parseExpr();
        if (!match(DELIMITER, ":")) {
            recordError("Missing ':' in ternary expression");
        }
        parseExpr();
    }
}

void parseLogicalOrExpr() {
    logParsing("LOGICAL_OR_EXPR");
    parseLogicalAndExpr();
    
    while (match(OPERATION, "||")) {
        parseLogicalAndExpr();
    }
}

void parseLogicalAndExpr() {
    logParsing("LOGICAL_AND_EXPR");
    parseBitwiseOrExpr();
    
    while (match(OPERATION, "&&")) {
        parseBitwiseOrExpr();
    }
}

void parseBitwiseOrExpr() {
    parseBitwiseXorExpr();
    
    while (match(OPERATION, "|")) {
        parseBitwiseXorExpr();
    }
}

void parseBitwiseXorExpr() {
    parseBitwiseAndExpr();
    
    while (match(OPERATION, "^")) {
        parseBitwiseAndExpr();
    }
}

void parseBitwiseAndExpr() {
    parseEqualityExpr();
    
    while (match(OPERATION, "&")) {
        parseEqualityExpr();
    }
}

void parseEqualityExpr() {
    logParsing("EQUALITY_EXPR");
    parseRelationalExpr();
    
    while (match(OPERATION, "==") || match(OPERATION, "!=")) {
        parseRelationalExpr();
    }
}

void parseRelationalExpr() {
    logParsing("RELATIONAL_EXPR");
    parseShiftExpr();
    
    while (match(OPERATION, "<") || match(OPERATION, ">") || 
           match(OPERATION, "<=") || match(OPERATION, ">=")) {
        parseShiftExpr();
    }
}

void parseShiftExpr() {
    parseAdditiveExpr();
    
    while (match(OPERATION, "<<") || match(OPERATION, ">>")) {
        parseAdditiveExpr();
    }
}

void parseAdditiveExpr() {
    logParsing("ADDITIVE_EXPR");
    parseMultiplicativeExpr();
    
    while (match(OPERATION, "+") || match(OPERATION, "-")) {
        parseMultiplicativeExpr();
    }
}

void parseMultiplicativeExpr() {
    logParsing("MULTI_EXPR");
    parseUnaryExpr();
    
    while (match(OPERATION, "*") || match(OPERATION, "/") || 
           match(OPERATION, "%") || match(OPERATION, "//")) {
        parseUnaryExpr();
    }
}

void parseUnaryExpr() {
    logParsing("UNARY_EXPR");
    
    if (match(OPERATION, "+") || match(OPERATION, "-") || 
        match(OPERATION, "!") || match(OPERATION, "++") || match(OPERATION, "--")) {
        parseUnaryExpr();
    } else {
        parsePostfixExpr();
    }
}

void parsePostfixExpr() {
    logParsing("POSTFIX_EXPR");
    parsePrimaryExpr();
    
    while (match(OPERATION, "++") || match(OPERATION, "--")) {
        // Postfix consumed
    }
}

void parsePrimaryExpr() {
    logParsing("PRIMARY_EXPR");
    
    if (matchType(IDENTIFIER)) {
        return;
    }
    else if (matchType(CONSTANT)) {
        return;
    }
    else if (checkType(RESERVED_WORDS)) {
        matchType(RESERVED_WORDS);
        return;
    }
    else if (match(DELIMITER, "(")) {
        parseExpr();
        if (!match(DELIMITER, ")")) {
            recordError("Missing ')' in expression");
        }
        return;
    }
    else if (check(KEYWORDS, "true") || check(KEYWORDS, "false")) {
        matchType(KEYWORDS);
        return;
    }
    else {
        recordError("Expects literal, identifier, or '(' in expression");
        if (current_token != NULL) {
            advance();
        }
    }
}
