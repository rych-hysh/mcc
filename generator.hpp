#pragma once

#include "parser.hpp"

class Generator{
  public:
  void gen_left_value(Node *node);
  void gen(Node *_node);
};
