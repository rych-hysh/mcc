#include <cstdio>
#include <list>

#include "parser.hpp"
#include "generator.hpp"

Generator::Generator()
{
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
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

void Generator::gen_prologue(Node *_node)
{
  // prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
    std::list<char*> regs {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  while(function_proccessing->local_var){
    //現時点ではローカル変数のサイズは８固定なので各変数で８ずつ領域を確保する
    printf("  sub rsp, 8\n");
    printf("  mov rax, rsp\n");
    printf("  mov [rax], %s\n", regs.front());
    regs.pop_front();
    function_proccessing->local_var = function_proccessing->local_var->next;
  }
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

void Generator::start_gen(Function *_function)
{
  function_proccessing = _function;
  gen(function_proccessing->Func_top_node);
}
void Generator::gen(Node *_node)
{

  LocalVariable *lo_var = function_proccessing->local_var;
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
  case NodeType::ND_WHILE:
    printf(".Lbegin_mcc%d:\n", global_label_index);
    gen(_node->condNode);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend_mcc%d\n", global_label_index);
    gen(_node->thenNode);
    printf("  jmp .Lbegin_mcc%d\n", global_label_index);
    printf(".Lend_mcc%d:\n", global_label_index);
    global_label_index++;
    return;
  case NodeType::ND_FOR:
    if(_node->initNode){
      gen(_node->initNode);
    }
    printf(".Lbegin_mcc%d:\n", global_label_index);
    if(_node->condNode){
      gen(_node->condNode);
    }else{
      printf("  push 1");
    }
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend_mcc%d\n", global_label_index);
    if(_node->thenNode){
      gen(_node->thenNode);
    }
    if(_node->loopNode){
      gen(_node->loopNode);
    }
    printf("  jmp .Lbegin_mcc%d\n", global_label_index);
    printf(".Lend_mcc%d:\n", global_label_index);
    global_label_index++;
    return;
  case NodeType::ND_BLOCK:
    while(_node->thenNode){
      gen(_node->thenNode);
      // TODO:
      // ↓rui ueyamaさんの教科書によると必要らしい。
      // 今のところなくても動くけど必要になったらコメントを外す
      //printf("  pop rax\n");
      _node = _node->thenNode;
    }
    return;
  case NodeType::ND_FUNC:
    printf("%s:\n", _node->identifier);
    gen_prologue(_node);
    while(_node->next){
      gen(_node->next);
      _node = _node->next;
    }
    gen_epilogue();
    return;
  case NodeType::ND_FUNCCALL:
    std::list<char*> regs {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    while(_node && _node->arg && _node->arg->node){
      gen(_node->arg->node);
      printf("  pop rax\n");
      printf("  mov %s, rax\n", regs.front());
      _node->arg = _node->arg->next;
      regs.pop_front();
    }
    printf("  call %s\n", _node->identifier);
    printf("  push rax\n");
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