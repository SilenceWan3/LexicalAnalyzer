#include "scanner.h"
/*����������ǵ���ͷϷ����ʵ�ִʷ����������һ����һЩ���õĸ����������Ա��
    �Ҳ��ա�*/
typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;
// ȫ�ֱ���
Scanner scanner;
void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
    // ��ʼ��scanner
}
/***********************************************************************
****************
*                                   ��������
*
***********************************************************************
****************/
static bool isAlpha(char c) {//�ж��ǲ�����ĸ���»���
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}
static bool isDigit(char c) {//�ж��ǲ�������
    return c >= '0' && c <= '9';
}
static bool isAtEnd() {//�ж��ǲ����ַ���ĩβ
    return *scanner.current == '\0';
}
static char advance() {
    return *scanner.current++;
}
static char peek() {
    return *scanner.current;
}
static char peekNext() {
    if (isAtEnd()) return '\0';
    return *(scanner.current + 1);
}
static bool match(char expected) {
    if (isAtEnd()) return false;
    if (peek() != expected) return false;
    scanner.current++;
    return true;
}
// ����TokenType, ������Ӧ���͵�Token�������ء�
static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}
// �������ܽ��������ʱ�����Ǵ���һ��ERROR Token. ���磺����@��$�ȷ���ʱ��������
// �������ַ�û�ж�Ӧ��������ʱ��
static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}
// �����հ��ַ�: ' ', '\r', '\t', '\n'��ע��
// ע����'//'��ͷ, һֱ����β
// ע�����scanner.line��
static void skipWhitespace() {
    while(peek()==' '||peek()=='\r'||peek()=='\t'||peek()=='\n'||peek()=='/'){
        if(peek()==' '){
            while(peek()==' '){
                advance();
            }
        }
        if(peek()=='\t'){
            while(peek()=='\t'){
                advance();
            }
        }
        if (peek() == '\r'||peek()=='\n') {
    		while (peek() == '\n') {
        		advance();
        		scanner.line++;
    		}
    		while (peek() == '\r' && peekNext() == '\n') {
        		advance();
        		advance();
        		scanner.line++;
    		}
    	}
        if(peek()=='/'){
            if(peekNext()=='/'){
                while(peek()!='\n'){
                    advance();
                }
            }
        }
    }
}
static TokenType identifierType();
    // ȷ��identifier������Ҫ�����ַ�ʽ��
    // 1. �����еĹؼ��ַ����ϣ���У�Ȼ����ȷ��
    // 2. �����еĹؼ��ַ���Trie���У�Ȼ����ȷ��
    // Trie���ķ�ʽ�����ǿռ��ϻ���ʱ���϶����ڹ�ϣ��ķ�ʽ
static Token identifier() {
    // IDENTIFIER����: ��ĸ�����ֺ��»���
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }
    // ������Token�����Ǳ�ʶ��, Ҳ�����ǹؼ���, identifierType()������ȷ��
    // Token���͵�
    return makeToken(identifierType());
}
static Token number() {
    int count = 0;
    while (isDigit(peek()) || peek() == '.') {
        if (peek() == '.') {
            count++;
            scanner.current--;
            if (isDigit(peek())) {
                advance();
                if(isDigit(peekNext())){
                    advance();
                    while (isDigit(peek())||peek()=='.') {
                    	if(peek()=='.') count++;
                        advance();
                        if (isDigit(peek())) {
                            advance();
                        }
                        else break;
                    }
                    if(count>1) return errorToken("Erroe Number");
                    if(peek()=='\n')
                    return makeToken(TOKEN_NUMBER);
                }
                else return makeToken(TOKEN_NUMBER);
            }
        }
        advance();
    }
    return makeToken(TOKEN_NUMBER);
// ����������ǽ�NUMBER�Ĺ���������:
// 1. NUMBER���԰������ֺ����һ��'.'��
// 2. '.'��ǰ��Ҫ������
// 3. '.'�ź���ҲҪ������
// ��Щ���ǺϷ���NUMBER: 123, 3.14
// ��Щ���ǲ��Ϸ���NUMBER: 123., .14
}
static Token string() {
    int count = 1;
    for (;;) {
        if (peek() == '\"'){
            count++;
            if(count==2) break;
        }
        else if(peek()==' ')
            advance();
        else if (peek() == '\n'||peek()=='\0') break;
        advance();
        }
    if(count==2){
        advance();
        return makeToken(TOKEN_STRING);
    }
    else return errorToken("Error String");
    // �ַ�����"��ͷ����"��β�����Ҳ��ܿ���
}

static Token character() {
    int count = 1;
    while(isAlpha(peek())||peek()=='\''){
        if(peek()=='\n') return errorToken("Error Character");
        if(peek()=='\''){
            count++;
            advance();
            if(count==2) break;
        }
        advance();
    }
    return makeToken(TOKEN_CHARACTER);
    // �ַ�'��ͷ����'��β�����Ҳ��ܿ���
}
/***********************************************************************
****************
*                                   �ִ�
*
***********************************************************************
****************/
Token scanToken() {
    // ����ǰ�ÿհ��ַ���ע��
    skipWhitespace();
    // ��¼��һ��Token����ʼλ��
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);
        char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        // single-character tokens
    case '(':return makeToken(TOKEN_LEFT_PAREN);
    case ')':return makeToken(TOKEN_RIGHT_PAREN);
    case '[':return makeToken(TOKEN_LEFT_BRACKET);
    case ']':return makeToken(TOKEN_RIGHT_BRACKET);
    case '{':return makeToken(TOKEN_LEFT_BRACE);
    case '}':return makeToken(TOKEN_RIGHT_BRACE);
    case ',':return makeToken(TOKEN_COMMA);
    case '.':return makeToken(TOKEN_DOT);
    case ';':return makeToken(TOKEN_SEMICOLON);
    case '~':return makeToken(TOKEN_TILDE);
    case '#':return makeToken(TOKEN_SHARP);
        // one or two characters tokens
    case '+':
        if (match('+')) return makeToken(TOKEN_PLUS_PLUS);
        else if (match('=')) return makeToken(TOKEN_PLUS_EQUAL);
        else return makeToken(TOKEN_PLUS);
    case '-':
        if (match('-'))return makeToken(TOKEN_MINUS_MINUS);
        else if (match('='))return makeToken(TOKEN_MINUS_EQUAL);
        else if (match('>'))return makeToken(TOKEN_MINUS_GREATER);
        else return makeToken(TOKEN_MINUS);
    case '*':
        if (match('='))return makeToken(TOKEN_STAR_EQUAL);
        else return makeToken(TOKEN_STAR);
    case '/':
        if (match('='))return makeToken(TOKEN_SLASH_EQUAL);
        else return makeToken(TOKEN_SLASH);
    case '%':
        if (match('='))return makeToken(TOKEN_PERCENT_EQUAL);
        else return makeToken(TOKEN_PERCENT);
    case '&':
        if (match('='))return makeToken(TOKEN_AMPER_EQUAL);
        else if (match('&'))return makeToken(TOKEN_AMPER_AMPER);
        else return makeToken(TOKEN_AMPER);
    case '|':
        if (match('='))return makeToken(TOKEN_PIPE_EQUAL);
        else if (match('|'))return makeToken(TOKEN_PIPE_PIPE);
        else return makeToken(TOKEN_PIPE);
    case '^':
        if (match('='))return makeToken(TOKEN_HAT_EQUAL);
        else return makeToken(TOKEN_HAT);
    case '=':
        if (match('='))return makeToken(TOKEN_EQUAL_EQUAL);
        else return makeToken(TOKEN_EQUAL);
    case '!':
        if (match('='))return makeToken(TOKEN_BANG_EQUAL);
        else return makeToken(TOKEN_BANG);
    case '<':
        if (match('='))return makeToken(TOKEN_LESS_EQUAL);
        else if (match('<'))return makeToken(TOKEN_LESS_LESS);
        else return makeToken(TOKEN_LESS);
    case '>':
        if (match('='))return makeToken(TOKEN_GREATER_EQUAL);
        else if (match('>'))return makeToken(TOKEN_GREAER_GREATER);
        else return makeToken(TOKEN_GREATER);
         // various-character tokens
    case '\"': return string();
    case '\'': return character();
    }

    return errorToken("Unexpected character.");
}
// �����������ص㿴һ�£�����ж�һ��Token�����Ǳ�ʶ�����ǹؼ��֡��������ǲ�
// ��Trie���ķ�ʽ�����жϣ���Ϊ�����Ǵӿռ��ϻ���ʱ���ϣ�Trie���ķ�ʽ�����ڹ�ϣ
// ��ķ�ʽ��
// ����˵����
// start: ���ĸ�����λ�ÿ�ʼ�Ƚ�
// length: Ҫ�Ƚ��ַ��ĳ���
// rest: Ҫ�Ƚϵ�����
// type: �����ȫƥ�䣬��˵����type���͵Ĺؼ���
static TokenType checkKeyword(int start, int length, const char* rest,
    TokenType type) {
    int len = (int)(scanner.current - scanner.start);
    if (start + length == len && memcmp(scanner.start + start, rest,
        length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}
// �жϵ�ǰToken�����Ǳ�ʶ�����ǹؼ���
static TokenType identifierType() {
    char c = scanner.start[0];
    // ��switch���ʵ��Trie��
    switch (c) {
    case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);

        case 'c':switch (scanner.start[1]) {
            case 'a':return checkKeyword(2, 2, "se", TOKEN_CASE);
            case 'h':return checkKeyword(2, 2, "ar", TOKEN_CHAR);
            case 'o':switch (scanner.start[2]) {
                case 'n':switch (scanner.start[3]) {
                    case 't':return checkKeyword(4, 4,"inue", TOKEN_CONTINUE);
                    case 's':return checkKeyword(4, 1, "t", TOKEN_CONST);
                }
            }
        }

        case 'd':switch (scanner.start[1]) {
            case 'e':return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
            case 'o':if (isAlpha(scanner.start[2])) {
                if (scanner.start[2] == 'u')
                    return checkKeyword(3, 3, "ble", TOKEN_DOUBLE);
            }
                    else return TOKEN_DO;
        }

        case 'e':switch (scanner.start[1]) {
            case'n':return(2, 2, "un", TOKEN_ENUM);
            case'l':return(2, 2, "se", TOKEN_ELSE);
        }

        case 'f':switch (scanner.start[1]) {
            case 'l':return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
            case 'o':return checkKeyword(2, 1, "r", TOKEN_FOR);
        }

        case 'g':return checkKeyword(1, 3, "oto", TOKEN_GOTO);

        case 'i':switch (scanner.start[1]) {
            case 'f':return TOKEN_IF;
            case 'n':return checkKeyword(2, 1, "t", TOKEN_INT);
        }

        case 'l':return checkKeyword(1, 3, "ong", TOKEN_LONG);

        case 'r':return checkKeyword(1, 5, "eturn", TOKEN_RETURN);

        case 's':switch (scanner.start[1]){
            case 'h':return checkKeyword(2, 3, "ort", TOKEN_SHORT);
            case 'i':switch (scanner.start[2]) {
                case 'g':return checkKeyword(3, 3, "ned", TOKEN_SIGNED);
                case 'z':return checkKeyword(3, 3, "eof", TOKEN_SIZEOF);
            }
            case 't':return checkKeyword(2, 4, "ruct", TOKEN_STRUCT);
            case 'w':return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
        }

        case 't':return checkKeyword(1, 6, "ypedef", TOKEN_TYPEDEF);

        case 'u':switch (scanner.start[1]) {
            case 'n':switch (scanner.start[2]) {
                case 'i':return checkKeyword(3, 2, "on", TOKEN_UNION);
                case 's':return checkKeyword(3, 5, "igned", TOKEN_UNSIGNED);
            }
        }

        case 'v':return checkKeyword(1, 3, "oid", TOKEN_VOID);

        case 'w':return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    // identifier
    return TOKEN_IDENTIFIER;
}
