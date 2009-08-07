#ifndef _STMT_H_
#define _STMT_H_

#include <cstdio>
#include "alist.h"
#include "baseAST.h"
#include "symbol.h"
#include "expr.h"


enum BlockTag {
  BLOCK_NORMAL,
  BLOCK_SCOPELESS, // does not introduce a new scope
  BLOCK_TYPE       // deleted after type resolution
};


class BlockStmt : public Expr {
 public:
  BlockTag blockTag;
  AList body;
  CallExpr* blockInfo;
  CallExpr* modUses;  // module uses via PRIM_USE
  LabelSymbol* breakLabel;
  LabelSymbol* continueLabel;
  const char* userLabel;

  BlockStmt(Expr* init_body = NULL, BlockTag init_blockTag = BLOCK_NORMAL);
  ~BlockStmt();
  void verify();
  DECLARE_COPY(BlockStmt);
  void replaceChild(Expr* old_ast, Expr* new_ast);
  void codegen(FILE* outfile);

  void insertAtHead(Expr* ast);
  void insertAtTail(Expr* ast);
  void insertAtTailBeforeGoto(Expr* ast);

  bool isLoop(void);
  int length(void);

  void addUse(ModuleSymbol* mod);
};


class CondStmt : public Expr {
 public:
  Expr* condExpr;
  BlockStmt* thenStmt;
  BlockStmt* elseStmt;

  CondStmt(Expr* iCondExpr, BaseAST* iThenStmt, BaseAST* iElseStmt = NULL);
  Expr* fold_cond_stmt();
  void verify();
  DECLARE_COPY(CondStmt);
  void replaceChild(Expr* old_ast, Expr* new_ast);

  void codegen(FILE* outfile);
};


enum GotoTag {
  GOTO_NORMAL,
  GOTO_BREAK,
  GOTO_CONTINUE
};


class GotoStmt : public Expr {
 public:
  GotoTag gotoTag;
  Expr* label;

  GotoStmt(GotoTag init_gotoTag, const char* init_label);
  GotoStmt(GotoTag init_gotoTag, Symbol* init_label);
  GotoStmt(GotoTag init_gotoTag, Expr* init_label);
  void verify();
  DECLARE_COPY(GotoStmt);
  void replaceChild(Expr* old_ast, Expr* new_ast);
  void codegen(FILE* outfile);
  const char* getName();
};


void codegenStmt(FILE* outfile, Expr* stmt);


#endif
