#pragma once

#include "parser.hpp"

class Generator{
  private:
  bool debug_mode = false;
  int global_label_index = 0;
  Function *function_proccessing;
  public:
  Generator();
  Generator(bool);
  void gen_left_value(Node *node);
  void gen_prologue(Node *node);
  void gen_epilogue();
  void start_gen(Function* funcs);
  void gen(Node *_node);
};
