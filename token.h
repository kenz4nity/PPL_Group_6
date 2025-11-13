#ifndef TOKEN_H
#define TOKEN_H

typedef enum tokenType {
    IDENTIFIER,
    OPERATION,
    KEYWORDS,
    RESERVED_WORDS,
    NOISE_WORDS,
    COMMENT,
    WHITE_SPACE,
    DELIMITER
} TokenType;
    
struct Node {
    TokenType tokentype;
    char lexeme [1000];
    int line;
    int column;
    struct Node* next;
};

struct LexemeNode {
    char lexeme [1000];
    int line;
    int column;
    struct LexemeNode* next;
};

#endif 