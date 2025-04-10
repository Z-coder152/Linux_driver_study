#include <stdio.h>

int main(int argc, char **argv) {
    // 输出命令行参数的数量
    printf("命令行参数的数量: %d\n", argc);

    // 输出每个命令行参数
    for (int i = 0; i < argc; i++) {
        printf("参数 %d: %s\n", i, argv[i]);
    }

    return 0;
}