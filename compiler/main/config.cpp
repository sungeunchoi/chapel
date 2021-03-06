#include "config.h"

#include "chpl.h"
#include "expr.h"
#include "stmt.h"

#include "../parser/lexyacc.h"

static Map<const char*, Expr*> configMap;
static Vec<const char*>        usedConfigParams;

bool                           mainHasArgs;

void checkConfigs() {
  //
  // Minimal module compilation doesn't support the standard CHPL_*
  // variables, and therefore shouldn't check for them.
  //
  if (fMinimalModules) {
    return;
  }

  bool             anyBadConfigParams = false;
  Vec<const char*> configParamSetNames;

  // configParamMap.get_keys(configParamSetNames);
  configMap.get_keys(configParamSetNames);

  forv_Vec(const char, name, configParamSetNames) {
    if (!usedConfigParams.in(name)) {
      USR_FATAL_CONT("Trying to set unrecognized config param '%s' via -s flag", name);
      anyBadConfigParams = true;
    }
  }

  if (anyBadConfigParams) {
    USR_STOP();
  }
}

//
// This function is designed to convert a name/value pair of C strings in to
// corresponding AST that is then inserted into the "configMap".
//
// This is currently acheived by synthesizing a chapel statement as a string,
// invoking the parser, and then extracting the desired portion of the result.
// This means that this function has some knowledge of the structure of the
// generated AST
//
// Historically the generated AST consisted of a BlockStmt which then contained
// a scope-less BlockStmt.  Recent changes to the parser are working to remove
// the internal scope-less BlockStmt.

// 2014/07/01 It is expected that it should be possible to remove the support
// for nested BlockStmt in the near future.

void parseCmdLineConfig(const char* name, const char* value) {
  // Generate a C-string for a nominal Chapel assignment statement
  const char* stmtText = (value[0] != '\0') ? astr("dummyConfig=", value, ";") : astr("dummyConfig=true;");
  const char* parseFn  = astr("Command-line arg (", name, ")");
  const char* parseMsg = astr("parsing '", value, "'");

  // Invoke the parser to generate AST
  BlockStmt*  stmt     = parseString(stmtText, parseFn, parseMsg);

  // Determine if the body is also a BlockStmt
  BlockStmt*  b        = toBlockStmt(stmt->body.head);
  Expr*       newExpr  = NULL;

  // If NO then extract the RHS from the stmt
  if (b == 0) {
    if (CallExpr* c = toCallExpr(stmt->body.head)) {
      newExpr = c->get(2)->copy();

    } else {
      INT_ASSERT(false);
    }

  } else {
    if (CallExpr* c = toCallExpr(b->body.head)) {
      newExpr = c->get(2)->copy();

    } else {
      INT_ASSERT(false);
    }

  }

  configMap.put(astr(name), newExpr);

  INT_ASSERT(newExpr == configMap.get(astr(name)));
}

Expr* getCmdLineConfig(const char* name) {
  return configMap.get(astr(name));
}

void useCmdLineConfig(const char* name) {
  usedConfigParams.add(name);
}

bool isUsedCmdLineConfig(const char* name) {
  return usedConfigParams.in(name);
}


