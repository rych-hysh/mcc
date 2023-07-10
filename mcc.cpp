#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdbool>
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>

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
  Node **statement = parser->parse(head_token);

  if (debug_flag)
  {
    int n = 0;
    ofstream writing_file;
    string filename = "parse_result.md";
    writing_file.open(filename, ios::out);
    writing_file << "```mermaid" << endl;
    writing_file << "flowchart TD" << endl;
    while (statement[n])
    {
      Node *db = statement[n];
      int id = n*100;
      print_node(statement[n++], id, &writing_file);
    }
    writing_file << "```" << endl;
    writing_file.close();
  }

  Generator *generator = new Generator();

  generator->gen_prologue();

  int i = 0;
  while (statement[i])
  {
    // 各文のトップノードを渡す
    generator->gen(statement[i++]);
    // printf("  pop rax\n");
  }

  generator->gen_epilogue();

  return 0;
}