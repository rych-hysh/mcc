#include <list>
#include <iostream>
#include <string>

#include "parser.hpp"
#include "generator.hpp"

#include "util.hpp"

using std::cout, std::endl, std::string, std::cerr;

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
    cerr << "left hand side of assignment statement is not local variable" << endl;
    exit(1);
  }
  cout << "\tmov rax, rbp" << endl;
  cout << "\tsub rax, " << node->offset << endl;
  cout << "\tpush rax" << endl;
}

void Generator::gen_prologue(Node *_node)
{
  // prologue
  cout << "\tpush rbp" << endl;
  cout << "\tmov rbp, rsp" << endl;
  std::list<string> regs {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  while(function_proccessing->local_var){
    //現時点ではローカル変数のサイズは８固定なので各変数で８ずつ領域を確保する
    cout << "\tsub rsp, 8" << endl;
    cout << "\tmov rax, rsp" << endl;
    cout << "\tmov [rax], " << regs.front() << endl;
    regs.pop_front();
    function_proccessing->local_var = function_proccessing->local_var->next;
  }
}

void Generator::gen_epilogue()
{
  // epilogue
  // 最後の式の結果がraxに残っているはずなのでそれが返り値になる
  cout << "\tpop rax" << endl;
  cout << "\tmov rsp, rbp" << endl;
  cout << "\tpop rbp" << endl;
  cout << "\tret" << endl;
}

void Generator::start_gen(Function *_function)
{
  function_proccessing = _function;
  gen(function_proccessing->Func_top_node);
}
void Generator::gen(Node *_node)
{
  if(debug_mode){
    std::cout << "#" <<  get_ND_type(_node->type) << std::endl;
  }
  switch (_node->type)
  {
  case NodeType::ND_NUMBER:
    cout << "\tpush " << _node->value << endl;
    return;
  case NodeType::ND_LVAL:
    gen_left_value(_node);
    cout << "\tpop rax" << endl;
    cout << "\tmov rax, [rax]" << endl;
    cout << "\tpush rax" << endl;
    return;
  case NodeType::ND_ASSIGN:
    gen_left_value(_node->leftHandSideNode);
    gen(_node->rightHandSideNode);
    cout << "\tpop rdi" << endl;
    cout << "\tpop rax" << endl;
    cout << "\tmov [rax], rdi" << endl;
    cout << "\tpush rdi" << endl;
    return;
  case NodeType::ND_RETURN:
    gen(_node->leftHandSideNode);
    gen_epilogue();
    return;
  case NodeType::ND_IF:
    gen(_node->condNode);
    // IF文の結果が真ならスタックトップに1が、偽なら０が入っている
    cout << "\tpop rax" << endl;
    // スタックトップが０（IFが偽）なら次の処理を飛ばす
    cout << "\tcmp rax, 0" << endl;
    cout << "\tje .Lend_mcc" <<global_label_index << endl;
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
    cout << "\tpop rax" << endl;
    cout << "\tcmp rax, 0" << endl;
    cout << "\tje .Lend_mcc" << global_label_index << endl;
    gen(_node->thenNode);
    cout << "\tjmp .Lbegin_mcc" << global_label_index << endl;
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
      cout << "\tpush 1" << endl;
    }
    cout << "\tpop rax" << endl;
    cout << "\tcmp rax, 0" << endl;
    cout << "\tje .Lend_mcc" << global_label_index << endl;
    if(_node->thenNode){
      gen(_node->thenNode);
    }
    if(_node->loopNode){
      gen(_node->loopNode);
    }
    cout << "\tjmp .Lbegin_mcc" << global_label_index << endl;
    cout << ".Lend_mcc" << global_label_index << ":" << endl;
    global_label_index++;
    return;
  case NodeType::ND_BLOCK:
    while(_node->thenNode){
      gen(_node->thenNode);
      // TODO:
      // ↓rui ueyamaさんの教科書によると必要らしい。
      // 今のところなくても動くけど必要になったらコメントを外す
      //printf("\tpop rax\n");
      _node = _node->thenNode;
    }
    return;
  // TODO: トップレベルノードにしか来ないならgen_startで処理？
  // クラス内で関数定義する場合もあるか？
  // Cに厳密に従うならクラスのブロック内では関数の定義をしないか？
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
        cout << "\tpop rax" << endl;
        cout << "\tmov " << regs.front() << ", rax" << endl;
        _node->arg = _node->arg->next;
        regs.pop_front();
      }
      cout << "\tcall " << _node->identifier << endl;
      cout << "\tpush rax" << endl;
      return;
    }
  case NodeType::ND_ADDR:
    gen_left_value(_node->leftHandSideNode);
    return;
  case NodeType::ND_DEREF:
    gen(_node->leftHandSideNode);
    cout << "\tpop rax" << endl;
    cout << "\tmov rax, [rax]" << endl;
    cout << "\tpush rax" << endl;
    return;
  default:
    break;
  }

  gen(_node->leftHandSideNode);
  gen(_node->rightHandSideNode);

  cout << "\tpop rdi" << endl;
  cout << "\tpop rax" << endl;

  switch (_node->type)
  {
  case NodeType::ND_EQ:
    cout << "\tcmp rax, rdi" << endl;
    cout << "\tsete al" << endl;
    cout << "\tmovzb rax, al" << endl;
    break;
  case NodeType::ND_NE:
    cout << "\tcmp rax, rdi" << endl;
    cout << "\tsetne al" << endl;
    cout << "\tmovzb rax, al" << endl;
    break;
  case NodeType::ND_LT:
    cout << "\tcmp rax, rdi" << endl;
    cout << "\tsetl al" << endl;
    cout << "\tmovzb rax, al" << endl;
    break;
  case NodeType::ND_LEQ:
    cout << "\tcmp rax, rdi" << endl;
    cout << "\tsetle al" << endl;
    cout << "\tmovzb rax, al" << endl;
    break;
  case NodeType::ND_GT:
    cout << "\tcmp rax, rdi" << endl;
    cout << "\tsetg al" << endl;
    cout << "\tmovzb rax, al" << endl;
    break;
  case NodeType::ND_GEQ:
    cout << "\tcmp rax, rdi" << endl;
    cout << "\tsetge al" << endl;
    cout << "\tmovzb rax, al" << endl;
    break;
  case NodeType::ND_ADD:
    cout << "\tadd rax, rdi" << endl;
    break;
  case NodeType::ND_SUB:
    cout << "\tsub rax, rdi" << endl;
    break;
  case NodeType::ND_MUL:
    cout << "\timul rax, rdi" << endl;
    break;
  case NodeType::ND_DIV:
    cout << "\tcqo" << endl;
    cout << "\tidiv rdi" << endl;
    break;
  default:
    cerr << "Invalide NodeType" << endl;
    break;
  }
  cout << "\tpush rax" << endl;
}