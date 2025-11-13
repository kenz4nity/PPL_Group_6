#include <stdio.h>
#include <string.h>
#include <ctype.h>   // For isdigit(), isalpha(), isalnum()
#include <stdbool.h> // For bool type
#include <stdlib.h>  // For exit()
#include "token.h"
#include "node_implementation.h"


// --- NEW: Logic from Keyword.c ---
// Define all states with meaningful names
enum States {
    STATE_START = 0,
    
    // Keywords
    STATE_A, STATE_AR, STATE_ARR, STATE_ARRA, STATE_ARRAY,
    STATE_B, STATE_BA, STATE_BAC, STATE_BACK, STATE_BO, STATE_BOO, STATE_BOOL,
    STATE_BR, STATE_BRE, STATE_BREA, STATE_BREAK,
    STATE_C, STATE_CA, STATE_CAT, STATE_CATC, STATE_CATCH,
    STATE_CH, STATE_CHA, STATE_CHAR,
    STATE_CL, STATE_CLA, STATE_CLAS, STATE_CLASS,
    STATE_CO, STATE_COM, STATE_COMP, STATE_COMPA, STATE_COMPAR, STATE_COMPARE,
    STATE_CON, STATE_CONS, STATE_CONT, STATE_CONTI, STATE_CONTIN, STATE_CONTINU, STATE_CONTINUE,
    STATE_D, STATE_DA, STATE_DAT, STATE_DATE,
    STATE_DI, STATE_DIS, STATE_DISP, STATE_DISPL, STATE_DISPLA, STATE_DISPLAY,
    STATE_DO,
    STATE_E, STATE_EX, STATE_EXC, STATE_EXCL, STATE_EXCLU, STATE_EXCLUS,
    STATE_EXCLUSI, STATE_EXCLUSIV, STATE_EXCLUSIVE,
    STATE_F, STATE_FL, STATE_FLO, STATE_FLOA, STATE_FLOAT,
    STATE_FU, STATE_FUN, STATE_FUNC,
    STATE_G, STATE_GO,
    STATE_H, STATE_HA, STATE_HAL, STATE_HALT,
    STATE_I, STATE_IN, STATE_INT,
    STATE_INC, STATE_INCL, STATE_INCLU, STATE_INCLUS, STATE_INCLUSI,
    STATE_INCLUSIV, STATE_INCLUSIVE,
    STATE_L, STATE_LE, STATE_LET, STATE_LI, STATE_LIS, STATE_LIST,
    STATE_O, STATE_OU, STATE_OUT, STATE_ON, STATE_ONL, STATE_ONLY,
    STATE_P, STATE_PR, STATE_PRI, STATE_PRIV, STATE_PU, STATE_PUB, STATE_PUT,
    STATE_R, STATE_RE, STATE_RET, STATE_RETU, STATE_RETUR, STATE_RETURN,
    STATE_S, STATE_ST, STATE_STO, STATE_STOP,
    STATE_T, STATE_TE, STATE_TES, STATE_TEST, STATE_TEX, STATE_TEXT,
    STATE_TH, STATE_THI, STATE_THIS,
    STATE_TI, STATE_TIM, STATE_TIME, STATE_TIMES, STATE_TIMEST, STATE_TIMESTA,
    STATE_TIMESTAM, STATE_TIMESTAMP,
    STATE_TR, STATE_TRY,
    STATE_V, STATE_VA, STATE_VAR,
    STATE_W, STATE_WH, STATE_WHA, STATE_WHAT, STATE_WHE, STATE_WHEN,
    STATE_WHI, STATE_WHIL, STATE_WHILE,
    
    // Reserved Words
    STATE_T_RES, STATE_TR_RES, STATE_TRU_RES, STATE_TRUE,
    STATE_F_RES, STATE_FA, STATE_FAL, STATE_FALS, STATE_FALSE,
    STATE_E_RES, STATE_EX_RES, STATE_EXI, STATE_EXIT,
    STATE_C_RES, STATE_CE, STATE_CEA, STATE_CEAS, STATE_CEASE,
    STATE_S_RES, STATE_SY, STATE_SYS, STATE_SYST, STATE_SYSTE, STATE_SYSTEM,
    STATE_G_RES, STATE_GO_RES, STATE_GOT, STATE_GOTO
};


const char* check_keyword_or_reserved(const char* word) {
    int state = STATE_START;
    char ch;

    for (int i = 0; (ch = word[i]) != '\0'; i++) {
        switch (state) {
            case STATE_START:
                if (ch == 'a') state = STATE_A;
                else if (ch == 'b') state = STATE_B;
                else if (ch == 'c') state = STATE_C;
                else if (ch == 'd') state = STATE_D;
                else if (ch == 'e') state = STATE_E;
                else if (ch == 'f') state = STATE_F;
                else if (ch == 'g') state = STATE_G;
                else if (ch == 'h') state = STATE_H;
                else if (ch == 'i') state = STATE_I;
                else if (ch == 'l') state = STATE_L;
                else if (ch == 'o') state = STATE_O;
                else if (ch == 'p') state = STATE_P;
                else if (ch == 'r') state = STATE_R;
                else if (ch == 's') state = STATE_S;
                else if (ch == 't') state = STATE_T;
                else if (ch == 'v') state = STATE_V;
                else if (ch == 'w') state = STATE_W;
                else return "IDENTIFIER";
                break;

            // --- array ---
            case STATE_A: if (ch == 'r') state = STATE_AR; else return "IDENTIFIER"; break;
            case STATE_AR: if (ch == 'r') state = STATE_ARR; else return "IDENTIFIER"; break;
            case STATE_ARR: if (ch == 'a') state = STATE_ARRA; else return "IDENTIFIER"; break;
            case STATE_ARRA: if (ch == 'y') state = STATE_ARRAY; else return "IDENTIFIER"; break;

            // --- back, bool, break ---
            case STATE_B:
                if (ch == 'a') state = STATE_BA;
                else if (ch == 'o') state = STATE_BO;
                else if (ch == 'r') state = STATE_BR;
                else return "IDENTIFIER";
                break;
            case STATE_BA: if (ch == 'c') state = STATE_BAC; else return "IDENTIFIER"; break;
            case STATE_BAC: if (ch == 'k') state = STATE_BACK; else return "IDENTIFIER"; break;
            case STATE_BO: if (ch == 'o') state = STATE_BOO; else return "IDENTIFIER"; break;
            case STATE_BOO: if (ch == 'l') state = STATE_BOOL; else return "IDENTIFIER"; break;
            case STATE_BR: if (ch == 'e') state = STATE_BRE; else return "IDENTIFIER"; break;
            case STATE_BRE: if (ch == 'a') state = STATE_BREA; else return "IDENTIFIER"; break;
            case STATE_BREA: if (ch == 'k') state = STATE_BREAK; else return "IDENTIFIER"; break;

            // --- catch, char, class, compare, cons, continue, cease ---
            case STATE_C:
                if (ch == 'a') state = STATE_CA;
                else if (ch == 'h') state = STATE_CH;
                else if (ch == 'l') state = STATE_CL;
                else if (ch == 'o') state = STATE_CO;
                else if (ch == 'e') state = STATE_CE;
                else return "IDENTIFIER";
                break;
            case STATE_CA: if (ch == 't') state = STATE_CAT; else return "IDENTIFIER"; break;
            case STATE_CAT: if (ch == 'c') state = STATE_CATC; else return "IDENTIFIER"; break;
            case STATE_CATC: if (ch == 'h') state = STATE_CATCH; else return "IDENTIFIER"; break;
            case STATE_CH: if (ch == 'a') state = STATE_CHA; else return "IDENTIFIER"; break;
            case STATE_CHA: if (ch == 'r') state = STATE_CHAR; else return "IDENTIFIER"; break;
            case STATE_CL: if (ch == 'a') state = STATE_CLA; else return "IDENTIFIER"; break;
            case STATE_CLA: if (ch == 's') state = STATE_CLAS; else return "IDENTIFIER"; break;
            case STATE_CLAS: if (ch == 's') state = STATE_CLASS; else return "IDENTIFIER"; break;
            case STATE_CO:
                if (ch == 'm') state = STATE_COM;
                else if (ch == 'n') state = STATE_CON;
                else return "IDENTIFIER";
                break;
            case STATE_COM: if (ch == 'p') state = STATE_COMP; else return "IDENTIFIER"; break;
            case STATE_COMP: if (ch == 'a') state = STATE_COMPA; else return "IDENTIFIER"; break;
            case STATE_COMPA: if (ch == 'r') state = STATE_COMPAR; else return "IDENTIFIER"; break;
            case STATE_COMPAR: if (ch == 'e') state = STATE_COMPARE; else return "IDENTIFIER"; break;
            case STATE_CON:
                if (ch == 's') state = STATE_CONS;
                else if (ch == 't') state = STATE_CONT;
                else return "IDENTIFIER";
                break;
            case STATE_CONT: if (ch == 'i') state = STATE_CONTI; else return "IDENTIFIER"; break;
            case STATE_CONTI: if (ch == 'n') state = STATE_CONTIN; else return "IDENTIFIER"; break;
            case STATE_CONTIN: if (ch == 'u') state = STATE_CONTINU; else return "IDENTIFIER"; break;
            case STATE_CONTINU: if (ch == 'e') state = STATE_CONTINUE; else return "IDENTIFIER"; break;
            case STATE_CE: if (ch == 'a') state = STATE_CEA; else return "IDENTIFIER"; break;
            case STATE_CEA: if (ch == 's') state = STATE_CEAS; else return "IDENTIFIER"; break;
            case STATE_CEAS: if (ch == 'e') state = STATE_CEASE; else return "IDENTIFIER"; break;

            // --- date, display, do ---
            case STATE_D:
                if (ch == 'a') state = STATE_DA;
                else if (ch == 'i') state = STATE_DI;
                else if (ch == 'o') state = STATE_DO;
                else return "IDENTIFIER";
                break;
            case STATE_DA: if (ch == 't') state = STATE_DAT; else return "IDENTIFIER"; break;
            case STATE_DAT: if (ch == 'e') state = STATE_DATE; else return "IDENTIFIER"; break;
            case STATE_DI: if (ch == 's') state = STATE_DIS; else return "IDENTIFIER"; break;
            case STATE_DIS: if (ch == 'p') state = STATE_DISP; else return "IDENTIFIER"; break;
            case STATE_DISP: if (ch == 'l') state = STATE_DISPL; else return "IDENTIFIER"; break;
            case STATE_DISPL: if (ch == 'a') state = STATE_DISPLA; else return "IDENTIFIER"; break;
            case STATE_DISPLA: if (ch == 'y') state = STATE_DISPLAY; else return "IDENTIFIER"; break;

            // --- exclusive, exit ---
            case STATE_E:
                if (ch == 'x') state = STATE_EX;
                else return "IDENTIFIER";
                break;
            case STATE_EX:
                if (ch == 'c') state = STATE_EXC;
                else if (ch == 'i') state = STATE_EXI;
                else return "IDENTIFIER";
                break;
            case STATE_EXC: if (ch == 'l') state = STATE_EXCL; else return "IDENTIFIER"; break;
            case STATE_EXCL: if (ch == 'u') state = STATE_EXCLU; else return "IDENTIFIER"; break;
            case STATE_EXCLU: if (ch == 's') state = STATE_EXCLUS; else return "IDENTIFIER"; break;
            case STATE_EXCLUS: if (ch == 'i') state = STATE_EXCLUSI; else return "IDENTIFIER"; break;
            case STATE_EXCLUSI: if (ch == 'v') state = STATE_EXCLUSIV; else return "IDENTIFIER"; break;
            case STATE_EXCLUSIV: if (ch == 'e') state = STATE_EXCLUSIVE; else return "IDENTIFIER"; break;
            case STATE_EXI: if (ch == 't') state = STATE_EXIT; else return "IDENTIFIER"; break;

            // --- float, func, false ---
            case STATE_F:
                if (ch == 'l') state = STATE_FL;
                else if (ch == 'u') state = STATE_FU;
                else if (ch == 'a') state = STATE_FA;
                else return "IDENTIFIER";
                break;
            case STATE_FL: if (ch == 'o') state = STATE_FLO; else return "IDENTIFIER"; break;
            case STATE_FLO: if (ch == 'a') state = STATE_FLOA; else return "IDENTIFIER"; break;
            case STATE_FLOA: if (ch == 't') state = STATE_FLOAT; else return "IDENTIFIER"; break;
            case STATE_FU: if (ch == 'n') state = STATE_FUN; else return "IDENTIFIER"; break;
            case STATE_FUN: if (ch == 'c') state = STATE_FUNC; else return "IDENTIFIER"; break;
            case STATE_FA: if (ch == 'l') state = STATE_FAL; else return "IDENTIFIER"; break;
            case STATE_FAL: if (ch == 's') state = STATE_FALS; else return "IDENTIFIER"; break;
            case STATE_FALS: if (ch == 'e') state = STATE_FALSE; else return "IDENTIFIER"; break;

            // --- go, goto ---
            case STATE_G:
                if (ch == 'o') state = STATE_GO;
                else return "IDENTIFIER";
                break;
            case STATE_GO:
                if (ch == 't') state = STATE_GOT;
                else return "KEYWORD";
                break;
            case STATE_GOT: if (ch == 'o') state = STATE_GOTO; else return "IDENTIFIER"; break;

            // --- halt ---
            case STATE_H: if (ch == 'a') state = STATE_HA; else return "IDENTIFIER"; break;
            case STATE_HA: if (ch == 'l') state = STATE_HAL; else return "IDENTIFIER"; break;
            case STATE_HAL: if (ch == 't') state = STATE_HALT; else return "IDENTIFIER"; break;

            // --- in, int, inclusive ---
            case STATE_I: if (ch == 'n') state = STATE_IN; else return "IDENTIFIER"; break;
            case STATE_IN:
                if (ch == 't') state = STATE_INT;
                else if (ch == 'c') state = STATE_INC;
                else return "KEYWORD";
                break;
            case STATE_INC: if (ch == 'l') state = STATE_INCL; else return "IDENTIFIER"; break;
            case STATE_INCL: if (ch == 'u') state = STATE_INCLU; else return "IDENTIFIER"; break;
            case STATE_INCLU: if (ch == 's') state = STATE_INCLUS; else return "IDENTIFIER"; break;
            case STATE_INCLUS: if (ch == 'i') state = STATE_INCLUSI; else return "IDENTIFIER"; break;
            case STATE_INCLUSI: if (ch == 'v') state = STATE_INCLUSIV; else return "IDENTIFIER"; break;
            case STATE_INCLUSIV: if (ch == 'e') state = STATE_INCLUSIVE; else return "IDENTIFIER"; break;

            // --- let, list ---
            case STATE_L:
                if (ch == 'e') state = STATE_LE;
                else if (ch == 'i') state = STATE_LI;
                else return "IDENTIFIER";
                break;
            case STATE_LE: if (ch == 't') state = STATE_LET; else return "IDENTIFIER"; break;
            case STATE_LI: if (ch == 's') state = STATE_LIS; else return "IDENTIFIER"; break;
            case STATE_LIS: if (ch == 't') state = STATE_LIST; else return "IDENTIFIER"; break;

            // --- out, only ---
            case STATE_O:
                if (ch == 'u') state = STATE_OU;
                else if (ch == 'n') state = STATE_ON;
                else return "IDENTIFIER";
                break;
            case STATE_OU: if (ch == 't') state = STATE_OUT; else return "IDENTIFIER"; break;
            case STATE_ON: if (ch == 'l') state = STATE_ONL; else return "IDENTIFIER"; break;
            case STATE_ONL: if (ch == 'y') state = STATE_ONLY; else return "IDENTIFIER"; break;

            // --- priv, pub, put ---
            case STATE_P:
                if (ch == 'r') state = STATE_PR;
                else if (ch == 'u') state = STATE_PU;
                else return "IDENTIFIER";
                break;
            case STATE_PR: if (ch == 'i') state = STATE_PRI; else return "IDENTIFIER"; break;
            case STATE_PRI: if (ch == 'v') state = STATE_PRIV; else return "IDENTIFIER"; break;
            case STATE_PU:
                if (ch == 'b') state = STATE_PUB;
                else if (ch == 't') state = STATE_PUT;
                else return "IDENTIFIER";
                break;

            // --- return ---
            case STATE_R: if (ch == 'e') state = STATE_RE; else return "IDENTIFIER"; break;
            case STATE_RE: if (ch == 't') state = STATE_RET; else return "IDENTIFIER"; break;
            case STATE_RET: if (ch == 'u') state = STATE_RETU; else return "IDENTIFIER"; break;
            case STATE_RETU: if (ch == 'r') state = STATE_RETUR; else return "IDENTIFIER"; break;
            case STATE_RETUR: if (ch == 'n') state = STATE_RETURN; else return "IDENTIFIER"; break;

            // --- stop, system ---
            case STATE_S:
                if (ch == 't') state = STATE_ST;
                else if (ch == 'y') state = STATE_SY;
                else return "IDENTIFIER";
                break;
            case STATE_ST: if (ch == 'o') state = STATE_STO; else return "IDENTIFIER"; break;
            case STATE_STO: if (ch == 'p') state = STATE_STOP; else return "IDENTIFIER"; break;
            case STATE_SY: if (ch == 's') state = STATE_SYS; else return "IDENTIFIER"; break;
            case STATE_SYS: if (ch == 't') state = STATE_SYST; else return "IDENTIFIER"; break;
            case STATE_SYST: if (ch == 'e') state = STATE_SYSTE; else return "IDENTIFIER"; break;
            case STATE_SYSTE: if (ch == 'm') state = STATE_SYSTEM; else return "IDENTIFIER"; break;

            // --- test, text, this, time, timestamp, try, true ---
            case STATE_T:
                if (ch == 'e') state = STATE_TE;
                else if (ch == 'h') state = STATE_TH;
                else if (ch == 'i') state = STATE_TI;
                else if (ch == 'r') state = STATE_TR;
                else return "IDENTIFIER";
                break;
            case STATE_TE:
                if (ch == 's') state = STATE_TES;
                else if (ch == 'x') state = STATE_TEX;
                else return "IDENTIFIER";
                break;
            case STATE_TES: if (ch == 't') state = STATE_TEST; else return "IDENTIFIER"; break;
            case STATE_TEX: if (ch == 't') state = STATE_TEXT; else return "IDENTIFIER"; break;
            case STATE_TH: if (ch == 'i') state = STATE_THI; else return "IDENTIFIER"; break;
            case STATE_THI: if (ch == 's') state = STATE_THIS; else return "IDENTIFIER"; break;
            case STATE_TI: if (ch == 'm') state = STATE_TIM; else return "IDENTIFIER"; break;
            case STATE_TIM: if (ch == 'e') state = STATE_TIME; else return "IDENTIFIER"; break;
            case STATE_TIME:
                if (ch == 's') state = STATE_TIMES;
                else return "KEYWORD";
                break;
            case STATE_TIMES: if (ch == 't') state = STATE_TIMEST; else return "IDENTIFIER"; break;
            case STATE_TIMEST: if (ch == 'a') state = STATE_TIMESTA; else return "IDENTIFIER"; break;
            case STATE_TIMESTA: if (ch == 'm') state = STATE_TIMESTAM; else return "IDENTIFIER"; break;
            case STATE_TIMESTAM: if (ch == 'p') state = STATE_TIMESTAMP; else return "IDENTIFIER"; break;
            case STATE_TR:
                if (ch == 'y') state = STATE_TRY;
                else if (ch == 'u') state = STATE_TRU_RES;
                else return "IDENTIFIER";
                break;
            case STATE_TRU_RES: if (ch == 'e') state = STATE_TRUE; else return "IDENTIFIER"; break;

            // --- var ---
            case STATE_V: if (ch == 'a') state = STATE_VA; else return "IDENTIFIER"; break;
            case STATE_VA: if (ch == 'r') state = STATE_VAR; else return "IDENTIFIER"; break;

            // --- what, when, while ---
            case STATE_W: if (ch == 'h') state = STATE_WH; else return "IDENTIFIER"; break;
            case STATE_WH:
                if (ch == 'a') state = STATE_WHA;
                else if (ch == 'e') state = STATE_WHE;
                else if (ch == 'i') state = STATE_WHI;
                else return "IDENTIFIER";
                break;
            case STATE_WHA: if (ch == 't') state = STATE_WHAT; else return "IDENTIFIER"; break;
            case STATE_WHE: if (ch == 'n') state = STATE_WHEN; else return "IDENTIFIER"; break;
            case STATE_WHI: if (ch == 'l') state = STATE_WHIL; else return "IDENTIFIER"; break;
            case STATE_WHIL: if (ch == 'e') state = STATE_WHILE; else return "IDENTIFIER"; break;

            default:
                return "IDENTIFIER";
        }
    }

    // Check final state
    switch (state) {
        // Keywords
        case STATE_ARRAY: case STATE_BACK: case STATE_BOOL: case STATE_BREAK:
        case STATE_CATCH: case STATE_CHAR: case STATE_CLASS: case STATE_COMPARE:
        case STATE_CONS: case STATE_CONTINUE: case STATE_DO: case STATE_DATE:
        case STATE_DISPLAY: case STATE_EXCLUSIVE: case STATE_FLOAT: case STATE_FUNC:
        case STATE_GO: case STATE_HALT: case STATE_IN: case STATE_INT:
        case STATE_INCLUSIVE: case STATE_LET: case STATE_LIST: case STATE_OUT:
        case STATE_ONLY: case STATE_PRIV: case STATE_PUB: case STATE_PUT:
        case STATE_RETURN: case STATE_STOP: case STATE_TEST: case STATE_TEXT:
        case STATE_THIS: case STATE_TIME: case STATE_TIMESTAMP: case STATE_TRY:
        case STATE_VAR: case STATE_WHAT: case STATE_WHEN: case STATE_WHILE:
            return "KEYWORD";
        
        // Reserved Words
        case STATE_TRUE: case STATE_FALSE: case STATE_EXIT:
        case STATE_CEASE: case STATE_SYSTEM: case STATE_GOTO:
            return "RESERVED_WORD";
        
        default:
            return "IDENTIFIER";
    }
}

/**
 * This is the fully synchronized lexical analyzer.
 * It combines the logic from all your files.
 *
 * This single function implements the complete DFA.
 */
void lexicalAnalyzer(char* word, struct Node** head, int line, int column) {
    int i = 0;
    int len = (int)strlen(word);
    int current_column = column;

    while (i < len) {
        char currentChar = word[i];

        // This switch statement handles the transitions from the START state
        switch (currentChar) {

            // --- Logic from operationSymbol.c ---
        case '+':
            if (i + 1 < len && word[i + 1] == '+') {
                insertAtEnd(head, OPERATION, "++", line, current_column);
                i += 2; current_column += 2;
            }
            else if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "+=", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "+", line, current_column);
                i++; current_column++;
            }
            break;
        case '-':
            if (i + 1 < len && word[i + 1] == '-') {
                insertAtEnd(head, OPERATION, "--", line, current_column);
                i += 2; current_column += 2;
            }
            else if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "-=", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "-", line, current_column);
                i++; current_column++;
            }
            break;
        case '*':
            // Check for multi-line comment end first
            if (i + 1 < len && word[i + 1] == '#') {
                insertAtEnd(head, COMMENT, "*#", line, current_column);
                i += 2; current_column += 2;
            }
            else if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "*=", line, current_column);
                i += 2; current_column += 2;
            }
            else if (i + 1 < len && word[i + 1] == '*') {
                insertAtEnd(head, OPERATION, "**", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "*", line, current_column);
                i++; current_column++;
            }
            break;
        case '/':
            if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "/=", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "/", line, current_column);
                i++; current_column++;
            }
            break;
        case '%':
            if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "%=", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "%", line, current_column);
                i++; current_column++;
            }
            break;
        case '<':
            if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "<=", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "<", line, current_column);
                i++; current_column++;
            }
            break;
        case '>':
            if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, ">=", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, ">", line, current_column);
                i++; current_column++;
            }
            break;
        case '=':
            if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "==", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "=", line, current_column);
                i++; current_column++;
            }
            break;
        case '!':
            if (i + 1 < len && word[i + 1] == '=') {
                insertAtEnd(head, OPERATION, "!=", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                insertAtEnd(head, OPERATION, "!", line, current_column);
                i++; current_column++;
            }
            break;
        case '&':
            if (i + 1 < len && word[i + 1] == '&') {
                insertAtEnd(head, OPERATION, "&&", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                i++; current_column++;
            }
            break;
        case '|':
            if (i + 1 < len && word[i + 1] == '|') {
                insertAtEnd(head, OPERATION, "||", line, current_column);
                i += 2; current_column += 2;
            }
            else {
                i++; current_column++;
            }
            break;

            // --- Logic from comment_analyzer.c ---
        case '#':
            if (i + 1 < len && word[i + 1] == '*') {
                // Multi-line comment - store the entire comment
                insertAtEnd(head, COMMENT, word, line, current_column);
                return; // Done with this lexeme
            }
            else if (i + 1 < len && word[i + 1] == '#') {
                // Single-line comment - store the entire comment
                insertAtEnd(head, COMMENT, word, line, current_column);
                return; // Done with this lexeme
            }
            else {
                i++; current_column++;
            }
            break;

            // --- Logic from Delimeters_and_Brackets.c ---
        case ';':
            insertAtEnd(head, DELIMITER, ";", line, current_column);
            i++; current_column++;
            break;
        case ':':
            insertAtEnd(head, DELIMITER, ":", line, current_column);
            i++; current_column++;
            break;
        case ',':
            insertAtEnd(head, DELIMITER, ",", line, current_column);
            i++; current_column++;
            break;
        case '(':
            insertAtEnd(head, DELIMITER, "(", line, current_column);
            i++; current_column++;
            break;
        case ')':
            insertAtEnd(head, DELIMITER, ")", line, current_column);
            i++; current_column++;
            break;
        case '{':
            insertAtEnd(head, DELIMITER, "{", line, current_column);
            i++; current_column++;
            break;
        case '}':
            insertAtEnd(head, DELIMITER, "}", line, current_column);
            i++; current_column++;
            break;
        case '[':
            insertAtEnd(head, DELIMITER, "[", line, current_column);
            i++; current_column++;
            break;
        case ']':
            insertAtEnd(head, DELIMITER, "]", line, current_column);
            i++; current_column++;
            break;

            // --- Logic from Constant.c (String and Char) ---
        case '"': {
            char lexeme[1000];
            int k = 0;
            int start_col = current_column;
            lexeme[k++] = word[i++]; current_column++; // Add the opening "
            while (i < len && word[i] != '"') {
                lexeme[k++] = word[i++];
                current_column++;
            }
            if (i < len && word[i] == '"') {
                lexeme[k++] = word[i++]; current_column++; // Add the closing "
            }
            lexeme[k] = '\0';
            insertAtEnd(head, RESERVED_WORDS, lexeme, line, start_col);
            break;
        }
        case '\'': {
            if (i + 2 < len && word[i + 2] == '\'') {
                char lexeme[4];
                lexeme[0] = '\'';
                lexeme[1] = word[i + 1];
                lexeme[2] = '\'';
                lexeme[3] = '\0';
                insertAtEnd(head, RESERVED_WORDS, lexeme, line, current_column);
                i += 3; current_column += 3;
            }
            else {
                i++; current_column++;
            }
            break;
        }

        // --- Whitespace Handling ---
        case ' ':
        case '\t':
        case '\n':
            insertAtEnd(head, WHITE_SPACE, word, line, current_column);
            i++; // Ignore whitespace for parsing
            return; // Each whitespace is its own lexeme
            break;

        // --- MODIFIED: Logic from Constant.c and Keyword.c ---
        default:
            // Rule for Numbers (Int/Float) -> CONSTANT
            if (isdigit(currentChar)) {
                char lexeme[100];
                int k = 0;
                int start_col = current_column;
                while (i < len && (isdigit(word[i]) || word[i] == '.')) {
                    lexeme[k++] = word[i++];
                    current_column++;
                }
                lexeme[k] = '\0';
                insertAtEnd(head, RESERVED_WORDS, lexeme, line, start_col);
            }
            // Rule for ALL "Words" -> Check if KEYWORD or IDENTIFIER
            else if (isalpha(currentChar)) {
                char lexeme[100];
                int k = 0;
                int start_col = current_column;
                while (i < len && isalnum(word[i])) {
                    lexeme[k++] = word[i++];
                    current_column++;
                }
                lexeme[k] = '\0';

                // Check if the lexeme is a keyword
                const char* keyword_result = check_keyword_or_reserved(lexeme);

                if (strncmp(keyword_result, "KEYWORD", 8) == 0) {
                    // It's a keyword
                    insertAtEnd(head, KEYWORDS, lexeme, line, start_col);
                } else if (strncmp(keyword_result, "RESERVED_WORD", 14) == 0){
                    insertAtEnd(head, RESERVED_WORDS, lexeme, line, start_col);
                }
                else {
                    // Check if it's a valid identifier
                    if (!isalpha(lexeme[0]) && lexeme[0] != '_') {
                        // Invalid identifier
                    } else {
                        int valid = 1;
                        for (int j = 1; lexeme[j] != '\0'; j++) {
                            if (!isalnum(lexeme[j]) && lexeme[j] != '_') {
                                valid = 0;
                                break;
                            }
                        }

                        if (valid) {
                            insertAtEnd(head, IDENTIFIER, lexeme, line, start_col);
                        }
                    }
                }
            }
            // Otherwise, it's an unknown symbol
            else {
                i++; current_column++;
            }
            break;
        }
    }
}


// --- Main function that prompts the user for word ---
/* int main() {
    char wordBuffer[1024];

    printf("=== C-Language Lexical Analyzer ===\n");
    printf("Enter code to analyze. Type 'exit' to quit.\n");

    // Loop forever to get user word
    while (1) {
        printf("\n> "); // Prompt

        // Get a line of word from the user
        if (fgets(wordBuffer, sizeof(wordBuffer), stdin) == NULL) {
            break; // Exit on EOF (Ctrl+D)
        }

        // Check if the user wants to exit
        if (strcmp(wordBuffer, "exit\n") == 0) {
            printf("Exiting analyzer. Goodbye!\n");
            break; // Exit the loop
        }

        // Run the synchronized analyzer on the user's word
        lexicalAnalyzer(wordBuffer);
    }

    return 0;
} */