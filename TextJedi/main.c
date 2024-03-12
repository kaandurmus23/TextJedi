#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_COUNT 100
#define MAX_TOKEN_SIZE 100

typedef struct {
    char token[MAX_TOKEN_SIZE];
} Token;
typedef struct {
    char name[MAX_TOKEN_SIZE];
    char type[MAX_TOKEN_SIZE];
    char value[MAX_TOKEN_SIZE];
} Variable;
typedef struct {
    Variable variables[MAX_TOKEN_COUNT];
    int count;
} VariableList;

void parseInput(const char* input, Token* tokens, int* tokenCount) {
    int tokenIndex = 0;
    const char* currentChar = input;
    const char* start = NULL;
    int tokenLength = 0;

    while (*currentChar != '\0') {
        if (*currentChar == '/' && *(currentChar + 1) == '*') {
            currentChar += 2;
            while (!(*currentChar == '*' && *(currentChar + 1) == '/')) {
                currentChar++;
            }
            currentChar += 2;
            continue;
        } else if (strncmp(currentChar, "\"", 1) == 0) {
            currentChar++;
            char word[MAX_TOKEN_SIZE];
            int i = 0;
            while (*currentChar != '\0') {
                if (strncmp(currentChar, "\"", 1) == 0) {
                    currentChar++;
                    break;
                }
                word[i++] = *currentChar++;
            }
            word[i] = '\0';
            strncpy(tokens[tokenIndex].token, word, i);
            tokens[tokenIndex].token[i] = '\0';
            tokenIndex++;
            continue;
        } else if (isdigit(*currentChar)) {
            start = currentChar;
            while (isdigit(*currentChar)) {
                currentChar++;
            }
            tokenLength = currentChar - start;
            strncpy(tokens[tokenIndex].token, start, tokenLength);
            tokens[tokenIndex].token[tokenLength] = '\0';
            tokenIndex++;
            continue;
        } else if (*currentChar == ';' || *currentChar == '+' || *currentChar == '-' || (*currentChar == ':' && *(currentChar + 1) == '=')) {
            start = currentChar++;
            if (*currentChar == '=') {
                currentChar++;
            }
            tokenLength = currentChar - start;
            strncpy(tokens[tokenIndex].token, start, tokenLength);
            tokens[tokenIndex].token[tokenLength] = '\0';
            tokenIndex++;
            continue;
        } else if (isalpha(*currentChar)) {
            start = currentChar;
            while (isalnum(*currentChar) || *currentChar == '\"' || *currentChar == ',' || *currentChar == '_' || *currentChar == '(' || *currentChar == ')' || *currentChar == ')') {
                currentChar++;
            }
            tokenLength = currentChar - start;
            strncpy(tokens[tokenIndex].token, start, tokenLength);
            tokens[tokenIndex].token[tokenLength] = '\0';
            tokenIndex++;
            continue;
        }
        currentChar++;
    }

    *tokenCount = tokenIndex;
}
void updateTokenValues(Token* tokens, int tokenCount, const VariableList* variables) {
    for (int i = 1; i < tokenCount; i++) {
        for (int j = 0; j < variables->count; j++) {
            if (strcmp(tokens[i].token, variables->variables[j].name) == 0) {
                strncpy(tokens[i].token, variables->variables[j].value, MAX_TOKEN_SIZE);
                tokens[i].token[MAX_TOKEN_SIZE - 1] = '\0';
                break; // Deðiþken bulundu, döngüyü sonlandýrabiliriz
            }
        }
    }
}
void valueAssignment(const Token* tokens, int tokenCount, VariableList* variables) {
    if (strcmp(tokens[1].token, ":=") == 0) {
        char* name = tokens[0].token;
        char* value = tokens[2].token;

        for (int i = 0; i < variables->count; i++) {
            if (strcmp(variables->variables[i].name, name) == 0) {
                if (strcmp(variables->variables[i].type, "int") == 0) {
                    int isInt = 1;
                    for (int j = 0; value[j] != '\0'; j++) {
                        if (!isdigit(value[j])) {
                            isInt = 0;
                            break;
                        }
                    }
                    if (isInt) {
                        strncpy(variables->variables[i].value, value, MAX_TOKEN_SIZE);
                        variables->variables[i].value[MAX_TOKEN_SIZE - 1] = '\0';
                        printf("Int veri tipinde değer ataması yapılmıştır.\n");
                    } else {
                        printf("Hata: %s deðiþkenine int tipinde deðer atanmalýdýr.\n", name);
                        exit(1);
                    }
                } else if (strcmp(variables->variables[i].type, "text") == 0) {
                    strncpy(variables->variables[i].value, value, MAX_TOKEN_SIZE);
                    variables->variables[i].value[MAX_TOKEN_SIZE - 1] = '\0';
                    printf("Text veri tipinde değer ataması yapılmıştır.\n");
                } else {
                    printf("Hata: Bilinmeyen deðiþken tipi: %s\n", variables->variables[i].type);
                    exit(1);
                }
                break;
            }
        }
    }
}
void saveNewVariables(const Token* tokens, int tokenCount, VariableList* variables) {
    if (strncmp(tokens[0].token, "new", 3) == 0) {
        if (tokenCount >= 3 && (strcmp(tokens[1].token, "text") == 0 || strcmp(tokens[1].token, "int") == 0)) {
            if (strlen(tokens[2].token) <= 30 && isalpha(tokens[2].token[0])) {
            Variable variable;
            strncpy(variable.type, tokens[1].token, MAX_TOKEN_SIZE);
            variable.type[MAX_TOKEN_SIZE - 1] = '\0';

            strncpy(variable.name, tokens[2].token, MAX_TOKEN_SIZE);
            variable.name[MAX_TOKEN_SIZE - 1] = '\0';

            variables->variables[variables->count] = variable;
            variables->count++;
            printf("%s veri tipinde yeni bir değişken oluşturuldu.\n", tokens[1].token);
            }else{
            printf("Değişken oluşturulamadı...\n");
            }
        } else {
            printf("Hata: 'new' anahtar kelimesinden sonra geçerli bir deðiþken tipi belirtilmelidir.\n");
            exit(1);
        }
    }
}
void addProcess(const Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 1; i < tokenCount - 1; i++) {
        if (strcmp(tokens[i].token, "+") == 0) {
            const char* prevToken = tokens[i - 1].token;
            const char* nextToken = tokens[i + 1].token;

            int prevTokenLength = strlen(prevToken);
            int nextTokenLength = strlen(nextToken);

            int isPrevTokenDigit = 1;
            int isNextTokenDigit = 1;

            for (int j = 0; j < prevTokenLength; j++) {
                if (!isdigit(prevToken[j])) {
                    isPrevTokenDigit = 0;
                    break;
                }
            }

            for (int j = 0; j < nextTokenLength; j++) {
                if (!isdigit(nextToken[j])) {
                    isNextTokenDigit = 0;
                    break;
                }
            }

            if (isPrevTokenDigit && isNextTokenDigit) {
                int num1 = atoi(prevToken);
                int num2 = atoi(nextToken);
                int result = num1 + num2;

                snprintf(tokens[i - 1].token, MAX_TOKEN_SIZE, "%d", result);
                memset(tokens[i].token, 0, MAX_TOKEN_SIZE);
                memset(tokens[i + 1].token, 0, MAX_TOKEN_SIZE);
                printf("Int veri tipindeki elemanlar başarılı bir şekilde toplanmıştır.\n");
            } else if (!isPrevTokenDigit) {
                size_t prevTokenLen = strlen(prevToken);
                size_t nextTokenLen = strlen(nextToken);
                size_t totalLen = prevTokenLen + nextTokenLen + 1;

                char* concatenatedToken = (char*)malloc(totalLen);
                snprintf(concatenatedToken, totalLen, "%s%s", prevToken, nextToken);

                strncpy(tokens[i - 1].token, concatenatedToken, MAX_TOKEN_SIZE);
                memset(tokens[i].token, 0, MAX_TOKEN_SIZE);
                memset(tokens[i + 1].token, 0, MAX_TOKEN_SIZE);
                printf("Text veri tipindeki elemanlar başarılı bir şekilde birleştirilmiştir.\n");
                free(concatenatedToken);
            }
        }
    }
}
void subProcess(const Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 1; i < tokenCount - 1; i++) {
        if (strcmp(tokens[i].token, "-") == 0) {
            const char* prevToken = tokens[i - 1].token;
            const char* nextToken = tokens[i + 1].token;

            int prevTokenLength = strlen(prevToken);
            int nextTokenLength = strlen(nextToken);

            int isPrevTokenDigit = 1;
            int isNextTokenDigit = 1;

            for (int j = 0; j < prevTokenLength; j++) {
                if (!isdigit(prevToken[j])) {
                    isPrevTokenDigit = 0;
                    break;
                }
            }

            for (int j = 0; j < nextTokenLength; j++) {
                if (!isdigit(nextToken[j])) {
                    isNextTokenDigit = 0;
                    break;
                }
            }

            if (isPrevTokenDigit && isNextTokenDigit) {
                int num1 = atoi(prevToken);
                int num2 = atoi(nextToken);

                if (num1 > num2) {
                    int result = num1 - num2;
                    snprintf(tokens[i - 1].token, MAX_TOKEN_SIZE, "%d", result);
                    memset(tokens[i].token, 0, MAX_TOKEN_SIZE);
                    memset(tokens[i + 1].token, 0, MAX_TOKEN_SIZE);
                    printf("Int veri tipindeki elemanlar başarılı bir şekilde çıkartılmıştır.\n");
                } else {
                    // Hata: Ýkinci sayý birinciden büyük veya eþit olduðunda
                    printf("Hata: Ýkinci sayý birinciden büyük veya eþit.\n");
                    exit(1);
                }
            } else if (!isPrevTokenDigit) {
                if (prevTokenLength > nextTokenLength) {
                    size_t prevTokenLen = strlen(prevToken);
                    size_t nextTokenLen = strlen(nextToken);
                    size_t totalLen = prevTokenLen + nextTokenLen + 1;

                    char* concatenatedToken = (char*)malloc(totalLen);
                    snprintf(concatenatedToken, totalLen, "%s%s", prevToken, nextToken);

                    // Ýçerisinde birebir next kelimesine benzeyen bir parçayý çýkar
                    char* match = strstr(concatenatedToken, nextToken);
                    while (match != NULL) {
                        size_t matchLen = strlen(match);
                        memmove(match, match + nextTokenLen, matchLen - nextTokenLen + 1);
                        match = strstr(concatenatedToken, nextToken);
                    }

                    strncpy(tokens[i - 1].token, concatenatedToken, MAX_TOKEN_SIZE);
                    memset(tokens[i].token, 0, MAX_TOKEN_SIZE);
                    memset(tokens[i + 1].token, 0, MAX_TOKEN_SIZE);

                    free(concatenatedToken);
                    printf("Text veri tipindeki elemanlar başarılı bir şekilde çıkartılmıştır.\n");
                } else {
                    // Hata: Ýkinci kelimenin eleman sayýsý birinciden büyük veya eþit olduðunda
                    printf("Hata: Ýkinci kelimenin eleman sayýsý birinciden büyük veya eþit.\n");
                    exit(1);
                }
            }
        }
    }
}
void checkLastToken(Token* tokens, int* tokenCount) {
    if (*tokenCount >= 1) {
        Token* lastToken = &tokens[*tokenCount - 1];
        if (strcmp(lastToken->token, ";") == 0) {
            (*tokenCount)--;
            memset(lastToken, 0, sizeof(Token));
        } else {
            printf("Hata: Beklenen ';' karakteri bulunamadý.\n");
            exit(1);
        }
    } else {
        printf("Hata: Token listesi boþ.\n");
    }
}
void asString_method(const Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 0; i < tokenCount; i++) {
        const Token* token = &tokens[i];
        if (strncmp(tokens[i].token, "asString(", 9) == 0 && tokens[i].token[strlen(tokens[i].token) - 1] == ')') {
            char variableName[MAX_TOKEN_SIZE];
            strncpy(variableName, tokens[i].token + 9, strlen(tokens[i].token) - 10);
            variableName[strlen(tokens[i].token) - 10] = '\0';

            printf("Token: %s\n", tokens[i].token);
            printf("Variable: %s\n", variableName);

            for (int j = 0; j < variables->count; j++) {
                if (strcmp(variables->variables[j].name, variableName) == 0) {
                    if (strcmp(variables->variables[j].type, "text") != 0) {
                        // Yeni referans oluþturma
                        Variable* newVariable = (Variable*)malloc(sizeof(Variable));
                        strcpy(newVariable->name, variables->variables[j].name);
                        strcpy(newVariable->value, variables->variables[j].value);
                        strcpy(newVariable->type, "text");

                        // Eski referansý deðiþtirme
                        variables->variables[j] = *newVariable;

                        printf("Updated variable: %s\n", variableName);
                    } else {
                        printf("Variable is already of type 'text': %s\n", variableName);
                    }

                    break;
                }
            }

            memset(tokens[i].token, 0, MAX_TOKEN_SIZE);
        }
    }
}
void asText_method(const Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 0; i < tokenCount; i++) {
        const Token* token = &tokens[i];
        if (strncmp(tokens[i].token, "asText(", 7) == 0 && tokens[i].token[strlen(tokens[i].token) - 1] == ')') {
            char variableName[MAX_TOKEN_SIZE];
            strncpy(variableName, tokens[i].token + 7, strlen(tokens[i].token) - 8);
            variableName[strlen(tokens[i].token) - 8] = '\0';

            printf("Token: %s\n", tokens[i].token);
            printf("Variable: %s\n", variableName);

            for (int j = 0; j < variables->count; j++) {
                if (strcmp(variables->variables[j].name, variableName) == 0) {
                    if (strcmp(variables->variables[j].type, "int") != 0) {
                        // Yeni referans oluþturma
                        Variable* newVariable = (Variable*)malloc(sizeof(Variable));
                        strcpy(newVariable->name, variables->variables[j].name);
                        strcpy(newVariable->value, variables->variables[j].value);
                        strcpy(newVariable->type, "int");

                        // Eski referansý deðiþtirme
                        variables->variables[j] = *newVariable;

                        printf("Updated variable: %s\n", variableName);
                    } else {
                        printf("Variable is already of type 'text': %s\n", variableName);
                    }

                    break;
                }
            }

            memset(tokens[i].token, 0, MAX_TOKEN_SIZE);
        }
    }
}
void size_method(Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 0; i < tokenCount; i++) {
        Token* token = &tokens[i];
        if (strncmp(token->token, "size(", 5) == 0 && token->token[strlen(token->token) - 1] == ')') {
            char variableName[MAX_TOKEN_SIZE];
            strncpy(variableName, token->token + 5, strlen(token->token) - 6);
            variableName[strlen(token->token) - 6] = '\0';

            printf("Token: %s\n", token->token);
            printf("Variable: %s\n", variableName);

            for (int j = 0; j < variables->count; j++) {
                if (strcmp(variables->variables[j].name, variableName) == 0) {
                    int charCount = strlen(variables->variables[j].value);  // Mevcut tokenýn karakter sayýsý

                    // Karakter sayýsýný string olarak dönüþtürme
                    char charCountStr[MAX_TOKEN_SIZE];
                    sprintf(charCountStr, "%d", charCount);

                    // Tokenýn deðerini güncelleme
                    strncpy(token->token, charCountStr, MAX_TOKEN_SIZE - 1);
                    token->token[MAX_TOKEN_SIZE - 1] = '\0';

                    printf("Updated token: %s\n", token->token);
                    break;
                }
            }
        }
    }
}
void subs_method(Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 0; i < tokenCount; i++) {
        Token* token = &tokens[i];
        if (strncmp(token->token, "subs(", 5) == 0 && token->token[strlen(token->token) - 1] == ')') {
            char variableName[MAX_TOKEN_SIZE];
            strncpy(variableName, token->token + 5, strlen(token->token) - 6);
            variableName[strlen(token->token) - 6] = '\0';

            printf("Token: %s\n", token->token);
            printf("Variable: %s\n", variableName);

            char* girdi1 = strtok(variableName, ",");
            char* girdi2 = strtok(NULL, ",");
            char* girdi3 = strtok(NULL, ")");

            printf("Girdi1: %s\n", girdi1);
            printf("Girdi2: %s\n", girdi2);
            printf("Girdi3: %s\n", girdi3);

            for (int j = 0; j < variables->count; j++) {
                if (strcmp(variables->variables[j].name, girdi1) == 0) {
                    int start = atoi(girdi2);
                    int end = atoi(girdi3);

                    if (start >= 0 && start < strlen(variables->variables[j].value) && end > start && end <= strlen(variables->variables[j].value)) {
                        char substring[MAX_TOKEN_SIZE];
                        strncpy(substring, variables->variables[j].value + start, end - start);
                        substring[end - start] = '\0';

                        strncpy(token->token, substring, MAX_TOKEN_SIZE - 1);
                        token->token[MAX_TOKEN_SIZE - 1] = '\0';

                        printf("Updated token: %s\n", token->token);
                    } else {
                        printf("Invalid range!\n");
                    }
                    break;
                }
            }
            break;
        }
    }
}
void locate_method(Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 0; i < tokenCount; i++) {
        Token* token = &tokens[i];
        if (strncmp(token->token, "locate(", 7) == 0 && token->token[strlen(token->token) - 1] == ')') {
            char variableName[MAX_TOKEN_SIZE];
            strncpy(variableName, token->token + 7, strlen(token->token) - 8);
            variableName[strlen(token->token) - 8] = '\0';

            printf("Token: %s\n", token->token);
            printf("Variable: %s\n", variableName);

            char* girdi1 = strtok(variableName, ",");
            char* girdi2 = strtok(NULL, ",");
            char* girdi3 = strtok(NULL, ")");

            printf("Girdi1: %s\n", girdi1);
            printf("Girdi2: %s\n", girdi2);
            printf("Girdi3: %s\n", girdi3);

            for (int j = 0; j < variables->count; j++) {
                if (strcmp(variables->variables[j].name, girdi1) == 0) {
                    // Baþýnda ve sonunda çift týrnak iþareti varsa çýkar
                    if (girdi2[0] == '\"' && girdi2[strlen(girdi2) - 1] == '\"') {
                        girdi2++;
                        girdi2[strlen(girdi2) - 1] = '\0';
                    }

                    char* foundPosition = strstr(variables->variables[j].value, girdi2);

                    if (foundPosition != NULL) {
                        int position = foundPosition - variables->variables[j].value + 1;

                        if (position >= 1 && position <= strlen(variables->variables[j].value)) {
                            int end = atoi(girdi3);

                            if (end > position) {
                                strncpy(token->token, "0", MAX_TOKEN_SIZE - 1);
                                token->token[MAX_TOKEN_SIZE - 1] = '\0';
                            } else {
                                char positionStr[MAX_TOKEN_SIZE];
                                sprintf(positionStr, "%d", position);

                                strncpy(token->token, positionStr, MAX_TOKEN_SIZE - 1);
                                token->token[MAX_TOKEN_SIZE - 1] = '\0';
                            }

                            printf("Updated token: %s\n", token->token);
                        } else {
                            strncpy(token->token, "0", MAX_TOKEN_SIZE - 1);
                            token->token[MAX_TOKEN_SIZE - 1] = '\0';

                            printf("Updated token: %s\n", token->token);
                        }
                    } else {
                        strncpy(token->token, "0", MAX_TOKEN_SIZE - 1);
                        token->token[MAX_TOKEN_SIZE - 1] = '\0';

                        printf("Updated token: %s\n", token->token);
                    }
                    break;
                }
            }
            break;
        }
    }
}
void insert_method(Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 0; i < tokenCount; i++) {
        Token* token = &tokens[i];
        if (strncmp(token->token, "insert(", 7) == 0 && token->token[strlen(token->token) - 1] == ')') {
            char variableName[MAX_TOKEN_SIZE];
            strncpy(variableName, token->token + 7, strlen(token->token) - 8);
            variableName[strlen(token->token) - 8] = '\0';

            printf("Token: %s\n", token->token);
            printf("Variable: %s\n", variableName);

            char* girdi1 = strtok(variableName, ",");
            char* girdi2 = strtok(NULL, ",");
            char* girdi3 = strtok(NULL, ")");

            printf("Girdi1: %s\n", girdi1);
            printf("Girdi2: %s\n", girdi2);
            printf("Girdi3: %s\n", girdi3);

            for (int j = 0; j < variables->count; j++) {
                if (strcmp(variables->variables[j].name, girdi1) == 0) {
                    int position = atoi(girdi2);

                    // Baþýnda ve sonunda çift týrnak iþareti varsa çýkar
                    if (girdi3[0] == '\"' && girdi3[strlen(girdi3) - 1] == '\"') {
                        girdi3++;
                        girdi3[strlen(girdi3) - 1] = '\0';
                    }

                    int length = strlen(girdi3);

                    if (position >= 0 && position <= strlen(variables->variables[j].value)) {
                        char newValue[MAX_TOKEN_SIZE];
                        strncpy(newValue, variables->variables[j].value, position);
                        newValue[position] = '\0';
                        strncat(newValue, girdi3, MAX_TOKEN_SIZE - 1);
                        strncat(newValue, variables->variables[j].value + position, MAX_TOKEN_SIZE - 1);

                        strncpy(variables->variables[j].value, newValue, MAX_TOKEN_SIZE - 1);
                        variables->variables[j].value[MAX_TOKEN_SIZE - 1] = '\0';

                        strncpy(token->token, variables->variables[j].value, MAX_TOKEN_SIZE - 1);
                        token->token[MAX_TOKEN_SIZE - 1] = '\0';

                        printf("Updated token: %s\n", token->token);
                    } else {
                        printf("Invalid position!\n");
                    }
                    break;
                }
            }
            break;
        }
    }
}
void override_method(Token* tokens, int tokenCount, VariableList* variables) {
    for (int i = 0; i < tokenCount; i++) {
        Token* token = &tokens[i];
        if (strncmp(token->token, "override(", 9) == 0 && token->token[strlen(token->token) - 1] == ')') {
            char variableName[MAX_TOKEN_SIZE];
            strncpy(variableName, token->token + 9, strlen(token->token) - 10);
            variableName[strlen(token->token) - 10] = '\0';

            printf("Token: %s\n", token->token);
            printf("Variable: %s\n", variableName);

            char* girdi1 = strtok(variableName, ",");
            char* girdi2 = strtok(NULL, ",");
            char* girdi3 = strtok(NULL, ")");

            printf("Girdi1: %s\n", girdi1);
            printf("Girdi2: %s\n", girdi2);
            printf("Girdi3: %s\n", girdi3);

            for (int j = 0; j < variables->count; j++) {
                if (strcmp(variables->variables[j].name, girdi1) == 0) {
                    int position = atoi(girdi2);

                    // Baþýnda ve sonunda çift týrnak iþareti varsa çýkar
                    if (girdi3[0] == '\"' && girdi3[strlen(girdi3) - 1] == '\"') {
                        girdi3++;
                        girdi3[strlen(girdi3) - 1] = '\0';
                    }

                    int length = strlen(girdi3);

                    if (position >= 0 && position <= strlen(variables->variables[j].value)) {
                        char newValue[MAX_TOKEN_SIZE];
                        strncpy(newValue, variables->variables[j].value, position);
                        newValue[position] = '\0';
                        strncat(newValue, girdi3, MAX_TOKEN_SIZE - 1);

                        strncpy(variables->variables[j].value, newValue, MAX_TOKEN_SIZE - 1);
                        variables->variables[j].value[MAX_TOKEN_SIZE - 1] = '\0';

                        strncpy(token->token, variables->variables[j].value, MAX_TOKEN_SIZE - 1);
                        token->token[MAX_TOKEN_SIZE - 1] = '\0';

                        printf("Updated token: %s\n", token->token);
                    } else {
                        printf("Invalid position!\n");
                    }
                    break;
                }
            }
            break;
        }
    }
}
void write_file_command(const Token* tokens, int tokenCount, VariableList* variables) {
    if (tokenCount >= 4 && strcmp(tokens[0].token, "write") == 0 && strcmp(tokens[2].token, "to") == 0) {
        const char* fileName = tokens[3].token;
        const char* variableName = tokens[1].token;
        const char* variableValue;
        // Deðiþkenin deðerini bul
        int variableIndex = -1;
        for (int i = 0; i < variables->count; i++) {
            if (strcmp(variables->variables[i].name, variableName) == 0) {
                variableIndex = i;
                break;
            }
        }

        // Deðiþkenin deðerini dosyaya yaz
        if (variableIndex != -1) {
            FILE* file = fopen(fileName, "w");
            if (file != NULL) {
                fprintf(file, "%s", variables->variables[variableIndex].value);
                fclose(file);
                printf("Dosya olusturuldu ve deger yazildi.\n");
            } else {
                printf("Dosya olusturulamadi.\n");
            }
        } else {
            printf("Belirtilen isimde bir degisken bulunamadi.\n");
        }
    }
}
void read_file_command(const Token* tokens, int tokenCount, VariableList* variables) {
    if (tokenCount >= 4 && strcmp(tokens[0].token, "read") == 0 && strcmp(tokens[2].token, "from") == 0) {
        const char* fileName = tokens[3].token;
        const char* variableName = tokens[1].token;

        // Dosyayý oku
        FILE* file = fopen(fileName, "r");
        if (file != NULL) {
            // Dosyanýn boyutunu bul
            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            // Bellekte yer ayýr ve dosya içeriðini oku
            char* fileContent = (char*)malloc(fileSize + 1);
            if (fileContent != NULL) {
                fread(fileContent, 1, fileSize, file);
                fileContent[fileSize] = '\0';

                // Deðiþkenin deðerini güncelle
                int variableIndex = -1;
                for (int i = 0; i < variables->count; i++) {
                    if (strcmp(variables->variables[i].name, variableName) == 0) {
                        variableIndex = i;
                        break;
                    }
                }

                if (variableIndex != -1) {
                    strncpy(variables->variables[variableIndex].value, fileContent, sizeof(variables->variables[variableIndex].value) - 1);
                    variables->variables[variableIndex].value[sizeof(variables->variables[variableIndex].value) - 1] = '\0';
                    printf("Dosya okundu ve deger guncellendi.\n");
                } else {
                    printf("Belirtilen isimde bir degisken bulunamadi.\n");
                }

                free(fileContent);
            } else {
                printf("Bellek ayirilamadi.\n");
            }

            fclose(file);
        } else {
            printf("Dosya bulunamadi.\n");
        }
    }
}
void output_command(const Token* tokens, int tokenCount, const VariableList* variables) {
    if (tokenCount >= 2 && strcmp(tokens[0].token, "output") == 0) {
        const char* variableName = tokens[1].token;

        // Deðiþkeni bul
        int variableIndex = -1;
        for (int i = 0; i < variables->count; i++) {
            if (strcmp(variables->variables[i].name, variableName) == 0) {
                variableIndex = i;
                break;
            }
        }

        if (variableIndex != -1) {
            printf("Deger: %s\n", variables->variables[variableIndex].value);
        } else {
            printf("Belirtilen isimde bir degisken bulunamadi.\n");
        }
    }
}
void input_command(const Token* tokens, int tokenCount, VariableList* variables) {
    if (tokenCount >= 3 && strcmp(tokens[0].token, "input") == 0 && strcmp(tokens[2].token, "prompt") == 0) {
        const char* variableName = tokens[1].token;
        const char* promptMessage = tokens[3].token;

        // Prompt mesajını ekrana yazdır
        printf("%s\n", promptMessage);

        // Kullanıcıdan giriş al
        char input[100];
        scanf("%s", input);

        // Çift tırnak işaretlerini görmezden gel
        if (input[0] == '"' && input[strlen(input) - 1] == '"') {
            // Başlangıç ve bitiş işaretlerini atla
            strcpy(input, input + 1);
            input[strlen(input) - 1] = '\0';
        }

        // Değişkeni bul
        int variableIndex = -1;
        for (int i = 0; i < variables->count; i++) {
            if (strcmp(variables->variables[i].name, variableName) == 0) {
                variableIndex = i;
                break;
            }
        }

        if (variableIndex != -1) {
            strcpy(variables->variables[variableIndex].value, input);
        } else {
            printf("Belirtilen isimde bir degisken bulunamadi.\n");
        }
    }
}
void check(const Token* tokens, int tokenCount, VariableList* variables){
    checkLastToken(tokens, &tokenCount); // ; Son karakter kontrolü yapýlýr
    write_file_command(tokens, tokenCount, variables); // write to komutunu iþler.
    read_file_command(tokens, tokenCount, variables); // read from komutu iþler.
    output_command(tokens, tokenCount, variables); // output komutu iþler.
    input_command(tokens, tokenCount, variables); // input komutu iþler.
    asString_method(tokens, tokenCount, variables); // asString() methodunu iþler int girilen parametreyi text haline getirir.
    asText_method(tokens, tokenCount, variables); // asText() methodunu iþler text girilen parametreyi int haline getirir.
    size_method(tokens, tokenCount, variables); // size() methodunu iþler içine girilen deðiþkenin eleman sayýsýný döndürür.
    subs_method(tokens, tokenCount, variables); // subs() methodunu iþler ilk parametre olan deðiþkenin 2. parametreden 3. parametreye kadar olan kýsmýný döndürür.
    locate_method(tokens, tokenCount, variables); // locate() methodunu iþler ilk parametre olan deðiþkenin içinde 2. parametre var mý diye bakar eðer var ise ve 3. parametreden büyük ise bunu yoksa 0 deðerini döndürür.
    insert_method(tokens, tokenCount, variables); //insert() methodunu iþler ilk parametre olan deðiþkenin 3. parametre olan kýsmýna 2. parametreyi koyar ve döndürür.
    override_method(tokens, tokenCount, variables); // override() methodunu iþler ilk parametre olan deðiþkenin içeriðinin 2. parametreden sonraki halini override edip 3. parametre yapar ve döndürür.
    updateTokenValues(tokens, tokenCount, variables); // Tokenlar içinde deðiþkenler var ise bu deðiþkenlerin deðeri yazýlýr
    addProcess(tokens, tokenCount, variables); // + elemaný var ise toplama ve concat iþlemini yapar
    subProcess(tokens, tokenCount, variables); // - eleman var ise çýkarma iþlemi yapar
    saveNewVariables(tokens, tokenCount, variables); // new ile yeni eleman tanýmý var mý
    valueAssignment(tokens, tokenCount, variables); // := þeklinde mevcut elemanýn deðiþimi mi var


}
void printVariables(const Token* tokens, int tokenCount, VariableList* variables) {
    printf("Değişkenler:\n");
    for (int i = 0; i < variables->count; i++) {
        printf("Name: %s, Type: %s", variables->variables[i].name, variables->variables[i].type);
        if (strlen(variables->variables[i].value) != NULL) {
            printf(", Value: %s", variables->variables[i].value);
        }
        printf("\n");
    }
}
int main() {
    Token tokens[MAX_TOKEN_COUNT];
    int tokenCount = 0;

    VariableList variables;
    variables.count = 0;

    while (1) {
        char input[MAX_TOKEN_SIZE];
        printf("Enter a TextJedi program (or 'q' to quit):\n");
        fgets(input, MAX_TOKEN_SIZE, stdin);

        // 'q' girildiğinde döngüyü sonlandır
        if (input[0] == 'q')
            break;

        // Token listesini sıfırla
        memset(tokens, 0, sizeof(tokens));
        tokenCount = 0;

        parseInput(input, tokens, &tokenCount);

        Token newTokenList[MAX_TOKEN_COUNT];
        int newTokenListCount = 0;

        int i = 0;
        while (i < tokenCount) {
            while (i < tokenCount && strcmp(tokens[i].token, ";") != 0) {
                strcpy(newTokenList[newTokenListCount].token, tokens[i].token);
                newTokenListCount++;
                i++;
            }

            if (i < tokenCount && strcmp(tokens[i].token, ";") == 0) {
                strcpy(newTokenList[newTokenListCount].token, tokens[i].token);
                newTokenListCount++;
                i++;
            }

            check(newTokenList, newTokenListCount, &variables);
            printVariables(newTokenList, newTokenListCount, &variables);
            memset(newTokenList, 0, sizeof(newTokenList));
            newTokenListCount = 0;
        }

    }

    return 0;
}







