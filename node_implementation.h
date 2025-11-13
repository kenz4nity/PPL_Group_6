    #ifndef NODE_IMPLEMENTATION_H
    #define NODE_IMPLEMENTATION_H
    #include "token.h"

    struct Node* createNode(TokenType tokentype, char lexeme[], int line, int column);
    struct LexemeNode* createNodeLexeme(char lexeme[], int line, int column);
    void insertAtBeginning(struct Node** head, TokenType tokentype, char lexeme[], int line, int column);
    void insertAtBeginningLexeme(struct LexemeNode** head, char lexeme[], int line, int column);
    void insertAtEnd(struct Node** head, TokenType tokentype, char lexeme[], int line, int column);
    void insertAtEndLexeme(struct LexemeNode** head, char lexeme[], int line, int column);
    void displayList(struct Node* head);
    void displayListLexeme(struct LexemeNode* head);
    void writeSymbolTableToFile(struct Node* head, const char* filename);

    #endif 