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
  case TK_SYMBOL:
    return "TK_SYMBOL";
  case TK_NUMBER:
    return "TK_NUMBER";
  case TK_IDENTIFIER:
    return "TK_IDENTIFIER";
  case TK_RETURN:
    return "TK_RETURN";
  case TK_IF:
    return "TK_IF";
  case TK_RESERVED:
    return "TK_RESERVED";
  case TK_EOF:
    return "TK_EOF";
  default:
    return "UNKNOWN_TOKEN";
  }
  return "UNKNOWN_TOKEN";
}
std::string get_ND_type(int _code){
  switch (_code)
  {
  case ND_ADDR:
    return "ND_ADDR";
  case ND_DEREF:
    return "ND_DEREF";
  case ND_ADD:
    return "ND_ADD";
  case ND_SUB:
    return "ND_SUB";
  case ND_MUL:
    return "ND_MUL";
  case ND_DIV:
    return "ND_DIV";
  case ND_EQ:
    return "ND_EQ";
  case ND_NE:
    return "ND_NE";
  case ND_LT:
    return "ND_LT";
  case ND_LEQ:
    return "ND_LEQ";
  case ND_GT:
    return "ND_GT";
  case ND_GEQ:
    return "ND_GEQ";
  case ND_ASSIGN:
    return "ND_ASSIGN";
  case ND_LVAL:
    return "ND_LVAL";
  case ND_RETURN:
    return "ND_RETURN";
  case ND_IF:
    return "ND_IF";
  case ND_WHILE:
    return "ND_WHILE";
  case ND_FOR:
    return "ND_FOR";
  case ND_BLOCK:
    return "ND_BLOCK";
  case ND_FUNC:
    return "ND_FUNC";
  case ND_FUNCCALL:
    return "ND_FUNCCALL";
  case ND_NUMBER:
    return "ND_NUMBER";
  case ND_LVARDEF:
    return "ND_LVARDEF";
  default:
    return "ND_TYPE_ERROR";
  }
}
int print_node(Node *_node, int _id, std::ofstream *_file)
{
  int this_id = _id;
  int tmp_id;
  *_file << this_id << "([" << get_ND_type(_node->type) << "])" << std::endl;
  if(_node->type == NodeType::ND_FUNC){
    LocalVariable *tmp_var = _node->local_vars;
    int vi = 0;
    while(tmp_var){
      *_file << this_id << "l" << vi++ << "(" << tmp_var->offset << ")" << std::endl;
      if(!tmp_var->next)break;
      tmp_var = tmp_var->next;
    }
    for(int i = 0; i< vi;i++){
      *_file << this_id << "-- " << "l_var:" << i << " ---" << this_id << "l" << i << std::endl;
    }
  }
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
    if (_node->next)
  {
    tmp_id = print_node(_node->next, ++_id, _file);
    *_file << this_id << "-- next ---" << tmp_id << std::endl; 
    _id += tmp_id;
  }
  return this_id;
}