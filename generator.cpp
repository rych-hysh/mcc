#include <list>
#include <iostream>
#include <string>

#include "parser.hpp"
#include "generator.hpp"

#include "util.hpp"

using std::cout, std::endl, std::string;

Generator::Generator()
{
  cout << ".intel_syntax noprefix" << endl;
  cout << ".globl main" << endl;
}

Generator::Generator(bool is_debug)
{
  this->debug_mode = is_debug;
  cout << ".intel_syntax noprefix" << endl;
  cout << ".globl main" << endl;
}

void Generator::gen_left_value(Node *node)
{
  if (node->type != NodeType::ND_LVAL)
  {
    fprintf(stderr, "left hand side of assignment statement is not local variable");
    exit(1);
  }
  cout << "  mov rax, rbp" << endl;
  cout << "  sub rax, " << node->offset << endl;
  cout << "  push rax" << endl;
}

void Generator::gen_prologue(Node *_node)
{
  // prologue
  cout << "  push rbp" << endl;
  cout << "  mov rbp, rsp" << endl;
  std::list<string> regs {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  while(function_proccessing->local_var){
    //現時点ではローカル変数のサイズは８固定なので各変数で８ずつ領域を確保する
    cout << "  sub rsp, 8" << endl;
    cout << "  mov rax, rsp" << endl;
    cout << "  mov [rax], " << regs.front() << endl;
    regs.pop_front();
    function_proccessing->local_var = function_proccessing->local_var->next;
  }
}

void Generator::gen_epilogue()
{
  // epilogue
  // 最後の式の結果がraxに残っているはずなのでそれが返り値になる
  cout << "  pop rax" << endl;
  cout << "  mov rsp, rbp" << endl;
  cout << "  pop rbp" << endl;
  cout << "  ret" << endl;
}

void Generator::start_gen(Function *_function)
{
  function_proccessing = _function;
  gen(function_proccessing->Func_top_node);
}
void Generator::gen(Node *_node)
{

  LocalVariable *lo_var = function_proccessing->local_var;  
  if(debug_mode){
    std::cout << "#" <<  get_ND_type(_node->type) << std::endl;
  }
  switch (_node->type)
  {
  case NodeType::ND_NUMBER:
    cout << "  push " << _node->value << endl;
    return;
  case NodeType::ND_LVAL:
    gen_left_value(_node);
    cout << "  pop rax" << endl;
    cout << "  mov rax, [rax]" << endl;
    cout << "  push rax" << endl;
    return;
  case NodeType::ND_ASSIGN:
    gen_left_value(_node->leftHandSideNode);
    gen(_node->rightHandSideNode);
    cout << "  pop rdi" << endl;
    cout << "  pop rax" << endl;
    cout << "  mov [rax], rdi" << endl;
    cout << "  push rdi" << endl;
    return;
  case NodeType::ND_RETURN:
    gen(_node->leftHandSideNode);
    gen_epilogue();
    return;
  case NodeType::ND_IF:
    gen(_node->condNode);
    // IF文の結果が真ならスタックトップに1が、偽なら０が入っている
    cout << "  pop rax" << endl;
    // スタックトップが０（IFが偽）なら次の処理を飛ばす
    cout << "  cmp rax, 0" << endl;
    cout << "  je .Lend_mcc" <<global_label_index << endl;
    if (_node->thenNode)
    {
      gen(_node->thenNode);
    }
    cout << ".Lend_mcc" << global_label_index++ << ":" <<endl;
    if (_node->elseNode)
    {
      gen(_node->elseNode);
    }
    return;
  case NodeType::ND_WHILE:
    cout << ".Lbegin_mcc" << global_label_index << ":" << endl;
    gen(_node->condNode);
    cout << "  pop rax" << endl;
    cout << "  cmp rax, 0" << endl;
    cout << "  je .Lend_mcc" << global_label_index << endl;
    gen(_node->thenNode);
    cout << "  jmp .Lbegin_mcc" << global_label_index << endl;
    cout << ".Lend_mcc" << global_label_index << ":" << endl;
    global_label_index++;
    return;
  case NodeType::ND_FOR:
    if(_node->initNode){
      gen(_node->initNode);
    }
    cout << ".Lbegin_mcc" << global_label_index << ":" << endl;
    if(_node->condNode){
      gen(_node->condNode);
    }else{
      cout << "  push 1" << endl;
    }
    cout << "  pop rax" << endl;
    cout << "  cmp rax, 0" << endl;
    cout << "  je .Lend_mcc" << global_label_index << endl;
    if(_node->thenNode){
      gen(_node->thenNode);
    }
    if(_node->loopNode){
      gen(_node->loopNode);
    }
    cout << "  jmp .Lbegin_mcc" << global_label_index << endl;
    cout << ".Lend_mcc" << global_label_index << ":" << endl;
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
    cout << _node->identifier << ":" << endl;
    gen_prologue(_node);
    while(_node->next){
      gen(_node->next);
      _node = _node->next;
    }
    gen_epilogue();
    return;
  case NodeType::ND_FUNCCALL:
    {
      std::list<string> regs{"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
      while(_node && _node->arg && _node->arg->node){
        gen(_node->arg->node);
        cout << "  pop rax" << endl;
        cout << "\tmov " << regs.front() << ", rax" << endl;
        _node->arg = _node->arg->next;
        regs.pop_front();
      }
      cout << "  call " << _node->identifier << endl;
      cout << "  push rax" << endl;
      return;
    }
  case NodeType::ND_ADDR:
    gen_left_value(_node->leftHandSideNode);
    return;
  case NodeType::ND_DEREF:
    gen(_node->leftHandSideNode);
    cout << "  pop rax" << endl;
    cout << "  mov rax, [rax]" << endl;
    cout << "  push rax" << endl;
    return;
  }

  gen(_node->leftHandSideNode);
  gen(_node->rightHandSideNode);

  cout << "  pop rdi" << endl;
  cout << "  pop rax" << endl;

  switch (_node->type)
  {
  case NodeType::ND_EQ:
    cout << "  cmp rax, rdi" << endl;
    cout << "  sete al" << endl;
    cout << "  movzb rax, al" << endl;
    break;
  case NodeType::ND_NE:
    cout << "  cmp rax, rdi" << endl;
    cout << "  setne al" << endl;
    cout << "  movzb rax, al" << endl;
    break;
  case NodeType::ND_LT:
    cout << "  cmp rax, rdi" << endl;
    cout << "  setl al" << endl;
    cout << "  movzb rax, al" << endl;
    break;
  case NodeType::ND_LEQ:
    cout << "  cmp rax, rdi" << endl;
    cout << "  setle al" << endl;
    cout << "  movzb rax, al" << endl;
    break;
  case NodeType::ND_GT:
    cout << "  cmp rax, rdi" << endl;
    cout << "  setg al" << endl;
    cout << "  movzb rax, al" << endl;
    break;
  case NodeType::ND_GEQ:
    cout << "  cmp rax, rdi" << endl;
    cout << "  setge al" << endl;
    cout << "  movzb rax, al" << endl;
    break;
  case NodeType::ND_ADD:
    cout << "  add rax, rdi" << endl;
    break;
  case NodeType::ND_SUB:
    cout << "  sub rax, rdi" << endl;
    break;
  case NodeType::ND_MUL:
    cout << "  imul rax, rdi" << endl;
    break;
  case NodeType::ND_DIV:
    cout << "  cqo" << endl;
    cout << "  idiv rdi" << endl;
    break;
  default:
    fprintf(stderr, "Invalide NodeType");
    break;
  }
  cout << "  push rax" << endl;
}