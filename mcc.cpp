#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdbool>
#include <cstring>
#include <cstdarg>
#include <iostream>

#include "parser.hpp"
#include "tokenizer.hpp"
#include "generator.hpp"

using namespace std;

char *reading_program;
// head token
Token *head_token;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "invalid number of args");
    return 1;
  }

  reading_program = argv[1];

  Tokenizer *tokenizer = new Tokenizer();
  head_token = tokenizer->tokenize(reading_program);

  Parser *parser = new Parser();
  //入力をtoken(単語)に分割し、最初のtokenをtoken_proccessingに代入
  Node *top_node = parser->parse(head_token);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  Generator *generator = new Generator();
  generator->gen(top_node);

  printf("  pop rax\n");
  printf("  ret\n");

  return 0;
}