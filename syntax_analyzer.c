#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOKEN_LEN 1000
#define MAX_ERRORS 100

typedef enum {
    IDENTIFIER, OPERATION, KEYWORDS, RESERVED_WORDS,
    STRING_LITERAL, CHAR_LITERAL, CONSTANT, NOISE_WORDS,
    COMMENT, WHITE_SPACE, DELIMITER, QUOTATION,
    OPEN_CURLY_BRACKET, CLOSE_CURLY_BRACKET,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    OPEN_BRACKET, CLOSE_BRACKET, SLASH, INVALID_TOKEN,
    END_OF_FILE
} TokenType;

typedef struct Token {
    TokenType type;
    char lexeme[MAX_TOKEN_LEN];
    struct Token* next;
} Token;

typedef struct {
    Token* head;
    Token* current;
    FILE* output;
    int error_count;
    bool panic_mode;
    char error_list[MAX_ERRORS][256];
} Parser;

// Token management
Token* createToken(TokenType type, const char* lexeme);
void addToken(Token** head, TokenType type, const char* lexeme);
void readTokensFromFile(const char* filename, Token** head);
void freeTokens(Token* head);

// Parser functions
void initParser(Parser* p, Token* tokens, FILE* output);
Token* getCurrentToken(Parser* p);
bool match(Parser* p, const char* lexeme);
bool matchType(Parser* p, TokenType type);
bool checkType(Parser* p, TokenType type);
bool check(Parser* p, const char* lexeme);
void advance(Parser* p);
void reportError(Parser* p, const char* message);
void synchronize(Parser* p);
void skipWhitespace(Parser* p);

// Grammar rules
void parseProgram(Parser* p);
void parseBlock(Parser* p);
void parseStatement(Parser* p);
void parseDeclarationStatement(Parser* p);
void parseAssignmentStatement(Parser* p);
void parseInputStatement(Parser* p);
void parseOutputStatement(Parser* p);
void parseConditionalStatement(Parser* p);
void parseIfStatement(Parser* p);
void parseCompareStatement(Parser* p);
void parseIterativeStatement(Parser* p);
void parseBreakStatement(Parser* p);
void parseExpression(Parser* p);
void parseAssignExpression(Parser* p);
void parseConditionalExpression(Parser* p);
void parseLogicalOrExpression(Parser* p);
void parseLogicalAndExpression(Parser* p);
void parseBitwiseOrExpression(Parser* p);
void parseBitwiseXorExpression(Parser* p);
void parseBitwiseAndExpression(Parser* p);
void parseEqualityExpression(Parser* p);
void parseRelationalExpression(Parser* p);
void parseAdditiveExpression(Parser* p);
void parseTerm(Parser* p);
void parseUnaryExpression(Parser* p);
void parsePostfixExpression(Parser* p);
void parseFactor(Parser* p);
void parseExpressionList(Parser* p);

int main() {
    Token* tokens = NULL;
    
    readTokensFromFile("SymbolTable.txt", &tokens);
    
    FILE* output = fopen("ParseOutput.txt", "w");
    if (!output) {
        fprintf(stderr, "Error: Cannot create output file\n");
        return 1;
    }
    
    Parser parser;
    initParser(&parser, tokens, output);
    
    fprintf(output, "=== LEX-C SYNTAX ANALYZER ===\n");
    fprintf(output, "Starting syntax analysis...\n\n");
    
    parseProgram(&parser);
    
    fprintf(output, "\n=== SYNTAX ANALYSIS COMPLETE ===\n");
    
    if (parser.error_count == 0) {
        fprintf(output, "✓ No syntax errors found.\n");
        printf("✓ Parsing completed successfully! No errors found.\n");
    } else {
        fprintf(output, "✗ Found %d syntax error(s):\n", parser.error_count);
        
        // Print the numbered list of errors
        for (int i = 0; i < parser.error_count && i < MAX_ERRORS; i++) {
            fprintf(output, "  %d. %s\n", i + 1, parser.error_list[i]);
        }
        
        if (parser.error_count > MAX_ERRORS) {
            fprintf(output, "  ... and %d more errors.\n", parser.error_count - MAX_ERRORS);
        }

        printf("✗ Parsing completed with %d error(s). Check ParseOutput.txt for the list.\n", 
               parser.error_count);
    }
    
    fclose(output);
    freeTokens(tokens);
    
    return 0;
}

Token* createToken(TokenType type, const char* lexeme) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    strncpy(token->lexeme, lexeme, MAX_TOKEN_LEN - 1);
    token->lexeme[MAX_TOKEN_LEN - 1] = '\0';
    token->next = NULL;
    return token;
}

void addToken(Token** head, TokenType type, const char* lexeme) {
    Token* newToken = createToken(type, lexeme);
    if (*head == NULL) {
        *head = newToken;
    } else {
        Token* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newToken;
    }
}

TokenType stringToTokenType(const char* str) {
    if (strcmp(str, "IDENTIFIER") == 0) return IDENTIFIER;
    if (strcmp(str, "OPERATION") == 0) return OPERATION;
    if (strcmp(str, "KEYWORDS") == 0) return KEYWORDS;
    if (strcmp(str, "RESERVED_WORDS") == 0) return RESERVED_WORDS;
    if (strcmp(str, "STRING_LITERAL") == 0) return STRING_LITERAL;
    if (strcmp(str, "CHAR_LITERAL") == 0) return CHAR_LITERAL;
    if (strcmp(str, "CONSTANT") == 0) return CONSTANT;
    if (strcmp(str, "NOISE_WORDS") == 0) return NOISE_WORDS;
    if (strcmp(str, "COMMENT") == 0) return COMMENT;
    if (strcmp(str, "WHITE_SPACE") == 0) return WHITE_SPACE;
    if (strcmp(str, "DELIMITER") == 0) return DELIMITER;
    if (strcmp(str, "OPEN_CURLY_BRACKET") == 0) return OPEN_CURLY_BRACKET;
    if (strcmp(str, "CLOSE_CURLY_BRACKET") == 0) return CLOSE_CURLY_BRACKET;
    if (strcmp(str, "LEFT_PARENTHESIS") == 0) return LEFT_PARENTHESIS;
    if (strcmp(str, "RIGHT_PARENTHESIS") == 0) return RIGHT_PARENTHESIS;
    if (strcmp(str, "OPEN_BRACKET") == 0) return OPEN_BRACKET;
    if (strcmp(str, "CLOSE_BRACKET") == 0) return CLOSE_BRACKET;
    if (strcmp(str, "SLASH") == 0) return SLASH;
    if (strcmp(str, "INVALID_TOKEN") == 0) return INVALID_TOKEN;
    return INVALID_TOKEN;
}

void readTokensFromFile(const char* filename, Token** head) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        exit(1);
    }
    
    char line[MAX_TOKEN_LEN * 2];
    while (fgets(line, sizeof(line), file)) {
        char typeStr[100];
        char lexeme[MAX_TOKEN_LEN];
        
        if (sscanf(line, "%s %[^\n]", typeStr, lexeme) == 2) {
            // Process lexeme - convert escape sequences
            char processed[MAX_TOKEN_LEN];
            int j = 0;
            for (int i = 0; lexeme[i] != '\0' && j < MAX_TOKEN_LEN - 1; i++) {
               if (lexeme[i] == '_' && stringToTokenType(typeStr) == WHITE_SPACE) {
                processed[j++] = ' ';
                } else {
                    processed[j++] = lexeme[i];
                }
            }
            processed[j] = '\0';
            
            TokenType type = stringToTokenType(typeStr);
            addToken(head, type, processed);
        }
    }
    
    addToken(head, END_OF_FILE, "EOF");
    fclose(file);
}

void freeTokens(Token* head) {
    Token* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void initParser(Parser* p, Token* tokens, FILE* output) {
    p->head = tokens;
    p->current = tokens;
    p->output = output;
    p->error_count = 0;
    p->panic_mode = false;
}

Token* getCurrentToken(Parser* p) {
    return p->current;
}

void skipWhitespace(Parser* p) {
    while (p->current && (p->current->type == WHITE_SPACE || 
                          p->current->type == COMMENT)) {
        p->current = p->current->next;
    }
}

bool checkType(Parser* p, TokenType type) {
    skipWhitespace(p);
    return p->current && p->current->type == type;
}

bool check(Parser* p, const char* lexeme) {
    skipWhitespace(p);
    return p->current && strcmp(p->current->lexeme, lexeme) == 0;
}

bool matchType(Parser* p, TokenType type) {
    if (checkType(p, type)) {
        if (p->current->type != WHITE_SPACE && p->current->type != COMMENT) {
            fprintf(p->output, "Matched: %s\n", p->current->lexeme);
        }
        p->current = p->current->next;
        skipWhitespace(p);
        return true;
    }
    return false;
}

bool match(Parser* p, const char* lexeme) {
    skipWhitespace(p);
    if (p->current && strcmp(p->current->lexeme, lexeme) == 0) {
        fprintf(p->output, "Matched: '%s'\n", lexeme);
        p->current = p->current->next;
        skipWhitespace(p);
        return true;
    }
    return false;
}

void advance(Parser* p) {
    if (p->current && p->current->type != END_OF_FILE) {
        p->current = p->current->next;
        skipWhitespace(p);
    }
}

void reportError(Parser* p, const char* message) {
    char errorMsg[256];
    if (p->current) {
        snprintf(errorMsg, sizeof(errorMsg), "%s. Found: '%s'", message, p->current->lexeme);
    } else {
        snprintf(errorMsg, sizeof(errorMsg), "%s. (End of File)", message);
    }

    fprintf(p->output, "\n[ERROR] %s\n", errorMsg);

    if (p->error_count < MAX_ERRORS) {
        strncpy(p->error_list[p->error_count], errorMsg, 255);
        p->error_list[p->error_count][255] = '\0'; 

    p->error_count++;
    p->panic_mode = true;
    }
}

void synchronize(Parser* p) {
    if (!p->panic_mode) return;
    
    p->panic_mode = false;
    skipWhitespace(p);
    
    // Synchronize at statement boundaries
    while (p->current && p->current->type != END_OF_FILE) {
        if (check(p, ";")) {
            advance(p); // consume semicolon
            return;
        }
        
        skipWhitespace(p);
        
        // Check for statement start keywords
        if (check(p, "let") || check(p, "var") || check(p, "cons") ||
            check(p, "put") || check(p, "display") || check(p, "do") ||
            check(p, "compare") || check(p, "continue") || check(p, "stop") ||
            check(p, "break") || check(p, "back") || check(p, "}")) {
            return;
        }
        
        advance(p);
    }
}

void parseProgram(Parser* p) {
    fprintf(p->output, "\n--- Parsing PROGRAM ---\n");
    skipWhitespace(p);
    
    if (!match(p, "main")) {
        reportError(p, "Expected 'main' keyword at program start");
        synchronize(p);
    }
    
    if (!match(p, ":")) {
        reportError(p, "Expected ':' after 'main'");
        synchronize(p);
    }
    
    parseBlock(p);
    
    skipWhitespace(p);
    if (!checkType(p, END_OF_FILE)) {
        reportError(p, "Unexpected tokens after program end");
    }
}

void parseBlock(Parser* p) {
    fprintf(p->output, "\n--- Parsing BLOCK ---\n");
    
    if (!match(p, "{")) {
        reportError(p, "Expected '{' to begin block");
        synchronize(p);
        return;
    }
    
    while (!check(p, "}") && !checkType(p, END_OF_FILE)) {
        parseStatement(p);
        if (p->panic_mode) {
            synchronize(p);
        }
    }
    
    if (!match(p, "}")) {
        reportError(p, "Expected '}' to end block");
    }
}

void parseStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing STATEMENT ---\n");
    skipWhitespace(p);
    
    if (check(p, "let") || check(p, "var") || check(p, "cons") || check(p, "list")) {
        parseDeclarationStatement(p);
    } else if (checkType(p, IDENTIFIER)) {
        parseAssignmentStatement(p);
    } else if (check(p, "put")) {
        parseInputStatement(p);
    } else if (check(p, "display")) {
        parseOutputStatement(p);
    } else if (check(p, "do") || check(p, "compare")) {
        parseConditionalStatement(p);
    } else if (check(p, "continue") || check(p, "stop")) {
        parseIterativeStatement(p);
    } else if (check(p, "break") || check(p, "back")) {
        parseBreakStatement(p);
    } else if (checkType(p, COMMENT)) {
        matchType(p, COMMENT);
    } else if (check(p, "}")) {
        return; // End of block
    } else {
        reportError(p, "Invalid statement");
        synchronize(p);
    }
}

void parseDeclarationStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing DECLARATION STATEMENT ---\n");
    
    // Storage class
    if (!(match(p, "let") || match(p, "var") || match(p, "cons") || match(p, "list"))) {
        reportError(p, "Expected storage class (let/var/cons/list)");
        synchronize(p);
        return;
    }
    
    // Data type
    if (!(match(p, "int") || match(p, "float") || match(p, "char") || 
          match(p, "text") || match(p, "bool") || match(p, "time") || 
          match(p, "date") || match(p, "timestamp"))) {
        reportError(p, "Expected data type");
        synchronize(p);
        return;
    }
    
    // Identifier
    if (!matchType(p, IDENTIFIER)) {
        reportError(p, "Expected identifier");
        synchronize(p);
        return;
    }
    
    // Optional initialization
    if (match(p, "=")) {
        parseExpression(p);
    }
    
    if (!match(p, ";")) {
        reportError(p, "Expected ';' after declaration");
        synchronize(p);
    }
}

void parseAssignmentStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing ASSIGNMENT STATEMENT ---\n");
    
    if (!matchType(p, IDENTIFIER)) {
        reportError(p, "Expected identifier");
        synchronize(p);
        return;
    }
    
    // Assignment operator
    if (!(match(p, "=") || match(p, "+=") || match(p, "-=") || 
          match(p, "*=") || match(p, "/=") || match(p, "%="))) {
        reportError(p, "Expected assignment operator");
        synchronize(p);
        return;
    }
    
    parseExpression(p);
    
    if (!match(p, ";")) {
        reportError(p, "Expected ';' after assignment");
        synchronize(p);
    }
}

void parseInputStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing INPUT STATEMENT ---\n");
    
    if (!match(p, "put")) {
        reportError(p, "Expected 'put' keyword");
        synchronize(p);
        return;
    }
    
    if (!matchType(p, IDENTIFIER)) {
        reportError(p, "Expected identifier after 'put'");
        synchronize(p);
        return;
    }
    
    if (!match(p, ";")) {
        reportError(p, "Expected ';' after input statement");
        synchronize(p);
    }
}

void parseOutputStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing OUTPUT STATEMENT ---\n");
    
    if (!match(p, "display")) {
        reportError(p, "Expected 'display' keyword");
        synchronize(p);
        return;
    }
    
    parseExpressionList(p);
    
    if (!match(p, ";")) {
        reportError(p, "Expected ';' after output statement");
        synchronize(p);
    }
}

void parseConditionalStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing CONDITIONAL STATEMENT ---\n");
    
    if (check(p, "do")) {
        parseIfStatement(p);
    } else if (check(p, "compare")) {
        parseCompareStatement(p);
    } else {
        reportError(p, "Expected 'do' or 'compare'");
        synchronize(p);
    }
}

void parseIfStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing IF STATEMENT ---\n");
    
    // 1. Parse "do if ( expression ) { block }"
    if (!match(p, "do")) {
        reportError(p, "Expected 'do'");
        return;
    }
    
    if (!match(p, "if")) {
        reportError(p, "Expected 'if' after 'do'");
        synchronize(p);
        return;
    }
    
    if (!match(p, "(")) {
        reportError(p, "Expected '(' after 'if'");
        synchronize(p);
        return;
    }
    
    parseExpression(p);
    
    if (!match(p, ")")) {
        reportError(p, "Expected ')' after condition");
        synchronize(p);
        return;
    }
    
    parseBlock(p);
    
    // 2. [FIX] Handle "what if" (Else If) blocks
    // We loop here because there can be multiple "what if" blocks
    while (check(p, "what")) {
        fprintf(p->output, "\n--- Parsing ELSE IF (WHAT) ---\n");
        match(p, "what");
        
        if (!match(p, "if")) {
            reportError(p, "Expected 'if' after 'what'");
            synchronize(p);
        }
        
        if (!match(p, "(")) {
            reportError(p, "Expected '(' after 'what if'");
            synchronize(p);
        }
        
        parseExpression(p);
        
        if (!match(p, ")")) {
            reportError(p, "Expected ')' after condition");
            synchronize(p);
        }
        
        parseBlock(p);
    }

    // 3. Handle "then do" (Else) block
    if (check(p, "then")) {
        fprintf(p->output, "\n--- Parsing ELSE (THEN) ---\n");
        match(p, "then");
        if (!match(p, "do")) {
            reportError(p, "Expected 'do' after 'then'");
            synchronize(p);
            return;
        }
        parseBlock(p);
    }
}

void parseCompareStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing COMPARE STATEMENT ---\n");
    
    if (!match(p, "compare")) {
        reportError(p, "Expected 'compare'");
        return;
    }
    
    parseExpression(p);
    
    if (!match(p, "{")) {
        reportError(p, "Expected '{' after compare expression");
        synchronize(p);
        return;
    }
    
    // Parse case list
    while (check(p, "what")) {
        match(p, "what");
        if (!match(p, "if")) {
            reportError(p, "Expected 'if' after 'what'");
            synchronize(p);
            continue;
        }
        
        parseExpression(p);
        
        if (!match(p, ":")) {
            reportError(p, "Expected ':' after case expression");
            synchronize(p);
            continue;
        }
        
        // Parse statements until break
        while (!check(p, "break") && !check(p, "what") && 
               !check(p, "then") && !check(p, "}") && 
               !checkType(p, END_OF_FILE)) {
            parseStatement(p);
            if (p->panic_mode) {
                synchronize(p);
                break;
            }
        }
        
        if (match(p, "break")) {
            if (!match(p, ";")) {
                reportError(p, "Expected ';' after 'break'");
                synchronize(p);
            }
        }
    }
    
    // Optional default case
    if (check(p, "then")) {
        match(p, "then");
        if (!match(p, "do")) {
            reportError(p, "Expected 'do' after 'then'");
            synchronize(p);
        }
        if (!match(p, ":")) {
            reportError(p, "Expected ':' after 'then do'");
            synchronize(p);
        }
        
        while (!check(p, "}") && !checkType(p, END_OF_FILE)) {
            parseStatement(p);
            if (p->panic_mode) {
                synchronize(p);
                break;
            }
        }
    }
    
    if (!match(p, "}")) {
        reportError(p, "Expected '}' to end compare statement");
    }
}

void parseIterativeStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing ITERATIVE STATEMENT ---\n");
    
    if (check(p, "continue")) {
        match(p, "continue");
        if (!match(p, "until")) {
            reportError(p, "Expected 'until' after 'continue'");
            synchronize(p);
            return;
        }
        
        if (!match(p, "(")) {
            reportError(p, "Expected '(' after 'until'");
            synchronize(p);
            return;
        }
        
        // Parse loop header (can be single expr or three exprs)
        parseExpression(p);
        
        if (match(p, ";")) {
            // For-loop style
            parseExpression(p);
            if (!match(p, ";")) {
                reportError(p, "Expected ';' in for-loop header");
                synchronize(p);
            }
            parseExpression(p);
        }
        
        if (!match(p, ")")) {
            reportError(p, "Expected ')' after loop header");
            synchronize(p);
            return;
        }
        
        parseBlock(p);
        
    } else if (check(p, "stop")) {
        match(p, "stop");
        if (!match(p, "when")) {
            reportError(p, "Expected 'when' after 'stop'");
            synchronize(p);
            return;
        }
        
        if (!match(p, "(")) {
            reportError(p, "Expected '(' after 'when'");
            synchronize(p);
            return;
        }
        
        parseExpression(p);
        
        if (!match(p, ")")) {
            reportError(p, "Expected ')' after condition");
            synchronize(p);
            return;
        }
        
        parseBlock(p);
    } else {
        reportError(p, "Expected 'continue' or 'stop'");
        synchronize(p);
    }
}

void parseBreakStatement(Parser* p) {
    fprintf(p->output, "\n--- Parsing BREAK/CONTROL STATEMENT ---\n");
    
    if (!(match(p, "break") || match(p, "back"))) {
        reportError(p, "Expected 'break' or 'back'");
        synchronize(p);
        return;
    }
    
    if (!match(p, ";")) {
        reportError(p, "Expected ';' after break/back");
        synchronize(p);
    }
}

void parseExpression(Parser* p) {
    fprintf(p->output, "--- Parsing EXPRESSION ---\n");
    parseAssignExpression(p);
}

void parseAssignExpression(Parser* p) {
    parseConditionalExpression(p);
    if (match(p, "=") || match(p, "+=") || match(p, "-=") || 
        match(p, "*=") || match(p, "/=") || match(p, "%=")) {
            parseExpression(p);
    }
}

void parseConditionalExpression(Parser* p) {
    parseLogicalOrExpression(p);
    
    if (match(p, "?")) {
        parseExpression(p);
        if (!match(p, ":")) {
            reportError(p, "Expected ':' in ternary expression");
        }
        parseExpression(p);
    }
}

void parseLogicalOrExpression(Parser* p) {
    parseLogicalAndExpression(p);
    
    while (match(p, "||")) {
        parseLogicalAndExpression(p);
    }
}

void parseLogicalAndExpression(Parser* p) {
    parseBitwiseOrExpression(p);
    
    while (match(p, "&&")) {
        parseBitwiseOrExpression(p);
    }
}

void parseBitwiseOrExpression(Parser* p) {
    parseBitwiseXorExpression(p);
    
    while (match(p, "|")) {
        parseBitwiseXorExpression(p);
    }
}

void parseBitwiseXorExpression(Parser* p) {
    parseBitwiseAndExpression(p);
    
    while (match(p, "^")) {
        parseBitwiseAndExpression(p);
    }
}

void parseBitwiseAndExpression(Parser* p) {
    parseEqualityExpression(p);
    
    while (match(p, "&")) {
        parseEqualityExpression(p);
    }
}

void parseEqualityExpression(Parser* p) {
    parseRelationalExpression(p);
    
    while (match(p, "==") || match(p, "!=")) {
        parseRelationalExpression(p);
    }
}

void parseRelationalExpression(Parser* p) {
    parseAdditiveExpression(p);
    
    while (match(p, "<") || match(p, ">") || match(p, "<=") || match(p, ">=")) {
        parseAdditiveExpression(p);
    }
}

void parseAdditiveExpression(Parser* p) {
    parseTerm(p);
    
    while (match(p, "+") || match(p, "-")) {
        parseTerm(p);
    }
}

void parseTerm(Parser* p) {
    parseUnaryExpression(p);
    
    while (match(p, "*") || match(p, "/") || match(p, "%") || match(p, "//")) {
        parseUnaryExpression(p);
    }
}

void parseUnaryExpression(Parser* p) {
    if (match(p, "+") || match(p, "-") || match(p, "!") || 
        match(p, "++") || match(p, "--")) {
        parseUnaryExpression(p);
    } else {
        parsePostfixExpression(p);
    }
}

void parsePostfixExpression(Parser* p) {
    parseFactor(p);
    
    while (match(p, "++") || match(p, "--")) {
        // Continue
    }
}

void parseFactor(Parser* p) {
    if (matchType(p, IDENTIFIER) || matchType(p, CONSTANT) || 
        matchType(p, STRING_LITERAL) || matchType(p, CHAR_LITERAL) ||
        match(p, "true") || match(p, "false")) {
        return;
    } else if (match(p, "(")) {
        parseExpression(p);
        if (!match(p, ")")) {
            reportError(p, "Expected ')' after expression");
        }
    } else {
        reportError(p, "Expected expression");
        synchronize(p);
    }
}

void parseExpressionList(Parser* p) {
    fprintf(p->output, "--- Parsing EXPRESSION LIST ---\n");
    parseExpression(p);
    
    while (match(p, ",")) {
        parseExpression(p);
    }
}