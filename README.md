# 王道C/C++ 项目一 词法分析器

#### 使用：将main.c，scanner.h，scanner.c放在同一个文件夹。

如果是在Windows下使用，建议在visual studio创建一个新的解决方案来存放三个文件进行编译运行

如果是在Linux下使用，在存放三个文件的文件夹内打开终端输入:

```
gcc main.c scanner.c scanner.h -o LexicalAnalyzer
```

```
./LexicalAnalyzer [path]
```

path为目标文件路径.

注：如果无文件读入，将进入人机对话，需要人为关闭:

Windows下关闭窗口；

Linux下Ctrl+Z。
