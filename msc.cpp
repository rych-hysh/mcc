#include <cstdio>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv){
  if(argc != 2){
    fprintf(stderr, "invalid number of args");
    return 1;
  }

  //プログラムを１文字ずつ読み込む
  char *p = argv[1];

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  //strtolは数値を読み込んだ後、第二引数のポインタをアップデートし読み込んだ最後の文字の次の文字を指すように値を更新する
  printf("  mov rax, %ld\n", strtol(p, &p, 10));

  while(*p){
    if(*p == '+'){
      p++;
      printf("  add rax, %ld\n", strtol(p, &p, 10));
      continue;
    }
    if(*p == '-'){
      p++;
      printf("  sub rax, %ld\n", strtol(p, &p, 10));
      continue;
    }
    fprintf(stderr, "unexpected character detected: '%c'\n", *p);
    return 1;
  }

  printf("  ret\n");

  return 0;
}