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

using namespace std;

char *reading_program;
// head token
Token *head_token;

bool debug_flag = true;
int print_node(Node* _n, int _i, ofstream* _f);

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
      writing_file << "', " << flush;
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
string get_ND_type(int id){
  switch (id)
  {
  case 0:
    return "ND_ADD";
  case 1:
    return "ND_SUB";
  case 2:
    return "ND_MUL";
  case 3:
    return "ND_DIV";
  case 4:
    return "ND_EQ";
  case 5:
    return "ND_NE";
  case 6:
    return "ND_LT";
  case 7:
    return "ND_LEQ";
  case 8:
    return "ND_GT";
  case 9:
    return "ND_GEQ";
  case 10:
    return "ND_ASSIGN";
  case 11:
    return "ND_LVAL";
  case 12:
    return "ND_RETURN";
  case 13:
    return "ND_IF";
  case 14:
    return "ND_NUMBER";
  default:
    return "ND_TYPE_ERROR";
  }
}
int print_node(Node *_node, int _id, ofstream *_file)
{
  int this_id = _id;
  int tmp_id;
  *_file << this_id << "([" << get_ND_type(_node->type) << "])" << endl;
  if (_node->leftHandSideNode)
  {
    tmp_id = print_node(_node->leftHandSideNode, ++_id, _file);
    *_file << this_id << "-- left ---" << tmp_id << endl; 
    _id += tmp_id;
  }
  if (_node->rightHandSideNode)
  {
    tmp_id = print_node(_node->rightHandSideNode, ++_id, _file);
    *_file << this_id << "-- right ---" << tmp_id << endl; 
    _id += tmp_id;
  }
  if (_node->thenNode)
  {
    tmp_id = print_node(_node->thenNode, ++_id, _file);
    *_file << this_id << "-- then ---" << tmp_id << endl; 
    _id += tmp_id;
  }
    if (_node->elseNode)
  {
    tmp_id = print_node(_node->elseNode, ++_id, _file);
    *_file << this_id << "-- else ---" << tmp_id << endl; 
    _id += tmp_id;
  }
  return this_id;
}