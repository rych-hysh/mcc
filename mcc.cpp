#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdbool>
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "parser.hpp"
#include "tokenizer.hpp"
#include "generator.hpp"
#include "util.hpp"

using namespace std;

char *reading_program;
// head token
Token *head_token;

bool debug_flag = true;

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

  if (debug_flag)
  {
    Token *head = head_token;
    ofstream writing_file;
    string filename = "tokenize_result.log";
    writing_file.open(filename, ios::out);
    while (head_token)
    {
      writing_file << "'";
      for (int i = 0; i < head_token->length; i++)
      {
        writing_file << head_token->str[i] << flush;
      }
      writing_file << "'\t" << ":" << "\t" << get_TK_type(head_token->type) << endl;
      head_token = head_token->next;
    }
    writing_file << "EOF" << endl;
    writing_file.close();
    head_token = head;
  }

  // エラー箇所表示のためプログラム本文をparserにも渡す
  Parser *parser = new Parser(reading_program);
  // 入力をtoken(単語)に分割し、最初のtokenをtoken_proccessingに代入
  vector<Function*> funcs = parser->parse(head_token);

  if (debug_flag)
  {
    std::vector<Function*>::size_type n = 0;
    ofstream writing_file;
    string filename = "parse_result.md";
    writing_file.open(filename, ios::out);
    writing_file << "```mermaid" << endl;
    writing_file << "flowchart TD" << endl;
    while (n < funcs.size()){
      int id = n*100;
      print_node(funcs[n++]->Func_top_node, id, &writing_file);

    }
    writing_file << "```" << endl;
    writing_file.close();
  }

  Generator *generator = new Generator(true);

  // TODO: 以下の処理をgenerator内に移す
  std::vector<Function*>::size_type i = 0;
  while ( i< funcs.size())
  {
    // 各関数のトップノードを渡す
    generator->start_gen(funcs[i++]);
    // printf("  pop rax\n");
  }

  delete tokenizer;
  delete parser;
  delete generator;
  return 0;
}