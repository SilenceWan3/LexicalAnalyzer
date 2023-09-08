#define _CRT_SECURE_NO_WARNINGS
#include "scanner.h"
#define MAX 1024
static void run(const char* source) {
    initScanner(source);
    int line = -1;
    // 打印Token, 遇到TOKEN_EOF为止
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
    // 与用户交互，用户每输入一行代码，分析一行代码，并将结果输出
    // repl是"read evaluate print loop"的缩写
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
    // 用户输入文件名，将整个文件的内容读入内存，并在末尾添加'\0'
    // 注意: 这里应该使用动态内存分配，因此应该事先确定文件的大小。
}
static void runFile(const char* path) {
    char *files=readFile(path);
    run(files);
    free(files);
    // 处理'.c'文件:用户输入文件名，分析整个文件，并将结果输出
}
int main(int argc, const char* argv[]) {
    if (argc == 1) {
            // ./scanner 没有参数,则进入交互式界面
            repl();
    }
    else if (argc == 2) {
        // ./scanner file 后面跟一个参数,则分析整个文件
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: scanner [path]\n");
        exit(1);
    }

    return 0;
}
