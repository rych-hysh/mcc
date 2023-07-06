#pragma once

#include "parser.hpp"

class Generator{
  public:
  Generator();
  void gen_left_value(Node *node);
  void gen_prologue();
  void gen_epilogue();
  void gen(Node *_node);
};
