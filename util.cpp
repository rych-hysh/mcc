#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <fstream>

#include "types.hpp"

void error(const char* _message){
  fprintf(stderr, _message, NULL);
  exit(-1);
};
void error_at(const char* _raw_input, char *_location, const char *_fmt, ...){
  va_list ap;
  va_start(ap, _fmt);

  int pos = _location - _raw_input;
  fprintf(stderr, "%s\n", _raw_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^");
  vfprintf(stderr, _fmt, ap);
  fprintf(stderr, "\n");

  exit(-1);
}
std::string get_TK_type(int _code){
  switch (_code)
  {
  case 0:
    return "TK_SYMBOL";
  case 1:
    return "TK_NUMBER";
  case 2:
    return "TK_IDENTIFIER";
  case 3:
    return "TK_RETURN";
  case 4:
    return "TK_IF";
  case 5:
    return "TK_RESERVED";
  case 6:
    return "TK_EOF";
  default:
    break;
  }
}
std::string get_ND_type(int _code){
  switch (_code)
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
    return "ND_WHILE";
  case 15:
    return "ND_FOR";
  case 16:
    return "ND_BLOCK";
  case 17:
    return "ND_FUNCCALL";
  case 18:
    return "ND_NUMBER";
  default:
    return "ND_TYPE_ERROR";
  }
}
int print_node(Node *_node, int _id, std::ofstream *_file)
{
  int this_id = _id;
  int tmp_id;
  *_file << this_id << "([" << get_ND_type(_node->type) << "])" << std::endl;
  if (_node->leftHandSideNode)
  {
    tmp_id = print_node(_node->leftHandSideNode, ++_id, _file);
    *_file << this_id << "-- left ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  if (_node->rightHandSideNode)
  {
    tmp_id = print_node(_node->rightHandSideNode, ++_id, _file);
    *_file << this_id << "-- right ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  if (_node->initNode)
  {
    tmp_id = print_node(_node->initNode, ++_id, _file);
    *_file << this_id << "-- init ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  if (_node->condNode)
  {
    tmp_id = print_node(_node->condNode, ++_id, _file);
    *_file << this_id << "-- cond ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  if (_node->thenNode)
  {
    tmp_id = print_node(_node->thenNode, ++_id, _file);
    *_file << this_id << "-- then ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  if (_node->elseNode)
  {
    tmp_id = print_node(_node->elseNode, ++_id, _file);
    *_file << this_id << "-- else ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  if (_node->loopNode)
  {
    tmp_id = print_node(_node->loopNode, ++_id, _file);
    *_file << this_id << "-- loop ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  return this_id;
}