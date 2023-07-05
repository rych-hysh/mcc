#include <cstdio>

#include "parser.hpp"
#include "generator.hpp"

void Generator::gen(Node *_node)
{
  if (_node->type == NodeType::ND_NUMBER)
  {
    printf("  push %d\n", _node->value);
    return;
  }

  gen(_node->leftHandSideNode);
  gen(_node->rightHandSideNode);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (_node->type)
  {
    case NodeType::ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case NodeType::ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case NodeType::ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case NodeType::ND_LEQ:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    case NodeType::ND_GT:
      printf("  cmp rax, rdi\n");
      printf("  setg al\n");
      printf("  movzb rax, al\n");
      break;
    case NodeType::ND_GEQ:
      printf("  cmp rax, rdi\n");
      printf("  setge al\n");
      printf("  movzb rax, al\n");
      break;
    case NodeType::ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case NodeType::ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case NodeType::ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case NodeType::ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    default:
      fprintf(stderr, "Invalide NodeType");
      break;
  }
  printf("  push rax\n");
}