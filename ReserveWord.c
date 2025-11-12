#include <stdio.h>

// Define meaningful states for all keywords
enum States {
    STATE_START = 0,
    
    // true
    STATE_T,
    STATE_TR,
    STATE_TRU,
    STATE_TRUE,
    
    // false
    STATE_F,
    STATE_FA,
    STATE_FAL,
    STATE_FALS,
    STATE_FALSE,
    
    // exit
    STATE_E,
    STATE_EX,
    STATE_EXI,
    STATE_EXIT,
    
    // cease
    STATE_C,
    STATE_CE,
    STATE_CEA,
    STATE_CEAS,
    STATE_CEASE,
  
    
    // system
    STATE_SY,
    STATE_SYS,
    STATE_SYST,
    STATE_SYSTE,
    STATE_SYSTEM,
    
    // goto
    STATE_G,
    STATE_GO,
    STATE_GOT,
    STATE_GOTO
};

const char* res_word(const char* word) {
    int state = STATE_START;
    char ch;

    for (int i = 0; (ch = word[i]) != '\0'; i++) {
        switch (state) {
            case STATE_START:
                if (ch == 't') state = STATE_T;
                else if (ch == 'f') state = STATE_F;
                else if (ch == 'e') state = STATE_E;
                else if (ch == 's') state = STATE_C;
                else if (ch == 'g') state = STATE_G;
                else return "Identifier or not a keyword";
                break;

            // --- true ---
            case STATE_T:
                if (ch == 'r') state = STATE_TR;
                else return "Identifier or not a keyword";
                break;
            case STATE_TR:
                if (ch == 'u') state = STATE_TRU;
                else return "Identifier or not a keyword";
                break;
            case STATE_TRU:
                if (ch == 'e') state = STATE_TRUE;
                else return "Identifier or not a keyword";
                break;

            // --- false ---
            case STATE_F:
                if (ch == 'a') state = STATE_FA;
                else return "Identifier or not a keyword";
                break;
            case STATE_FA:
                if (ch == 'l') state = STATE_FAL;
                else return "Identifier or not a keyword";
                break;
            case STATE_FAL:
                if (ch == 's') state = STATE_FALS;
                else return "Identifier or not a keyword";
                break;
            case STATE_FALS:
                if (ch == 'e') state = STATE_FALSE;
                else return "Identifier or not a keyword";
                break;

            // --- exit ---
            case STATE_E:
                if (ch == 'x') state = STATE_EX;
                else return "Identifier or not a keyword";
                break;
            case STATE_EX:
                if (ch == 'i') state = STATE_EXI;
                else return "Identifier or not a keyword";
                break;
            case STATE_EXI:
                if (ch == 't') state = STATE_EXIT;
                else return "Identifier or not a keyword";
                break;

            // --- cease ---
            case STATE_C:
                if (ch == 'e') state = STATE_CE;
                else return "Identifier or not a keyword";
                break;
            case STATE_CE:
                if (ch == 'a') state = STATE_CEA;
                else return "Identifier or not a keyword";
                break;
            case STATE_CEA:
                if (ch == 's') state = STATE_CEAS;
                else return "Identifier or not a keyword";
                break;
            case STATE_CEAS:
                if (ch == 'e') state = STATE_CEASE;
                else return "Identifier or not a keyword";
                break;   

            // --- system ---
            case STATE_SY:
                if (ch == 's') state = STATE_SYS;
                else return "Identifier or not a keyword";
                break;
            case STATE_SYS:
                if (ch == 't') state = STATE_SYST;
                else return "Identifier or not a keyword";
                break;
            case STATE_SYST:
                if (ch == 'e') state = STATE_SYSTE;
                else return "Identifier or not a keyword";
                break;
            case STATE_SYSTE:
                if (ch == 'm') state = STATE_SYSTEM;
                else return "Identifier or not a keyword";
                break;

            // --- goto ---
            case STATE_G:
                if (ch == 'o') state = STATE_GO;
                else return "Identifier or not a keyword";
                break;
            case STATE_GO:
                if (ch == 't') state = STATE_GOT;
                else return "Identifier or not a keyword";
                break;
            case STATE_GOT:
                if (ch == 'o') state = STATE_GOTO;
                else return "Identifier or not a keyword";
                break;

            default:
                return "Identifier or not a keyword";
        }
    }

    // ✅ Check final state after full word is read
    switch (state) {
        case STATE_TRUE:    return "Reserved Word";
        case STATE_FALSE:   return "Reserved Word";
        case STATE_EXIT:    return "Reserved Word";
        case STATE_CEASE:   return "Reserved Word";
        case STATE_SYSTEM:  return "Reserved Word";
        case STATE_GOTO:    return "Reserved Word";
        default:            return "Reserved Word";
    }
}

int main() {
    const char* words[] = {"true", "false", "exit", "system", "stop", "goto", "int"};

    int n = sizeof(words) / sizeof(words[0]);
    for (int i = 0; i < n; i++) {
        printf("%s → %s\n", words[i], res_word(words[i]));
    }

    return 0;
}
