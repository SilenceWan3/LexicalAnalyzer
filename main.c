#define _CRT_SECURE_NO_WARNINGS
#include "scanner.h"
#define MAX 1024
static void run(const char* source) {
    initScanner(source);
    int line = -1;
    // ��ӡToken, ����TOKEN_EOFΪֹ
    for (;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        }
        else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length,token.start);

        if (token.type == TOKEN_EOF) break;
    }
}
static void repl() {
    while(1){
        printf(">");
        char s[MAX] = {0};
        fgets(s, MAX, stdin);
        //if (s == "exit") break;
        run(s);
    }
    exit(1);
    // ���û��������û�ÿ����һ�д��룬����һ�д��룬����������
    // repl��"read evaluate print loop"����д
}
static char* readFile(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
	    fprintf(stderr, "Open %s failed.\n", path);
	    exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    long int len = ftell(fp);
    rewind(fp);
    char* files = (char *)malloc(len + 1);
    if (files == NULL) {
	    fprintf(stderr, "malloc failed in readFile.\n");
	    fclose(fp);
	    exit(1);
    }
    size_t end = fread(files, sizeof(char), len+1, fp);
    files[end] = '\0';
    return files;
    fclose(fp);
    // �û������ļ������������ļ������ݶ����ڴ棬����ĩβ���'\0'
    // ע��: ����Ӧ��ʹ�ö�̬�ڴ���䣬���Ӧ������ȷ���ļ��Ĵ�С��
}
static void runFile(const char* path) {
    char *files=readFile(path);
    run(files);
    free(files);
    // ����'.c'�ļ�:�û������ļ��������������ļ�������������
}
int main(int argc, const char* argv[]) {
    if (argc == 1) {
            // ./scanner û�в���,����뽻��ʽ����
            repl();
    }
    else if (argc == 2) {
        // ./scanner file �����һ������,����������ļ�
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: scanner [path]\n");
        exit(1);
    }

    return 0;
}
