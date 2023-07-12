#pragma once

#include "parser.hpp"

class Generator{
  private:
  int global_label_index = 0;
  Function *function_proccessing;
  public:
  Generator();
  void gen_left_value(Node *node);
  void gen_prologue(Node *node);
  void gen_epilogue();
  void start_gen(Function *_function);
  void gen(Node *_node);
};
