#include <cstdio>

#include "parser.hpp"
#include "generator.hpp"

Generator::Generator()
{
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
}

void Generator::gen_left_value(Node *node)
{
  if (node->type != NodeType::ND_LVAL)
  {
    fprintf(stderr, "left hand side of assignment statement is not local variable");
    exit(1);
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void Generator::gen_prologue()
{
  // prologue
  // 変数26個分の領域を確保する(26 * 8 = 208)
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");
}

void Generator::gen_epilogue()
{
  // epilogue
  // 最後の式の結果がraxに残っているはずなのでそれが返り値になる
  printf("  pop rax\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
void Generator::gen(Node *_node)
{
  switch (_node->type)
  {
  case NodeType::ND_NUMBER:
    printf("  push %d\n", _node->value);
    return;
  case NodeType::ND_LVAL:
    gen_left_value(_node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case NodeType::ND_ASSIGN:
    gen_left_value(_node->leftHandSideNode);
    gen(_node->rightHandSideNode);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case NodeType::ND_RETURN:
    gen(_node->leftHandSideNode);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case NodeType::ND_IF:
    gen(_node->condNode);
    // IF文の結果が真ならスタックトップに1が、偽なら０が入っている
    printf("  pop rax\n");
    // スタックトップが０（IFが偽）なら次の処理を飛ばす
    printf("  cmp rax, 0\n");
    printf("  je .Lend_mcc%d\n", global_label_index);
    if (_node->thenNode)
    {
      gen(_node->thenNode);
    }
    printf(".Lend_mcc%d:\n", global_label_index++);
    if (_node->elseNode)
    {
      gen(_node->elseNode);
    }
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