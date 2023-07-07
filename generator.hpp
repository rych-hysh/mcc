#pragma once

#include "parser.hpp"

class Generator{
  private:
  int global_label_index = 0;
  public:
  Generator();
  void gen_left_value(Node *node);
  void gen_prologue();
  void gen_epilogue();
  void gen(Node *_node);
};
