#ifndef _pattern_H_
#define _pattern_H_

#include <sys/types.h>
#include "code.h"

class Sym;
class FA;
class Fun;
class AType;
class AVar;

#define MAX_ARGS 1000

#define int2Position(_i) ((void*)((intptr_t)-(_i)))
#define Position2int(_p) (-((intptr_t)_p))
#define is_intPosition(_p) (((uint32)Position2int(_p)) < MAX_ARGS)

class MPosition : public gc {
 public:
  Vec<void*> pos;
  MPosition *parent;
  void copy(MPosition &p);
  void set_top(void *p) { pos.v[pos.n-1] = p; }
  void push(int i) { pos.add(int2Position(i)); }
  void push(void *p) { pos.add(p); }
  void pop() { pos.pop(); }
  void inc() { pos.v[pos.n-1] = int2Position(Position2int(pos.v[pos.n-1]) + 1); }
  void dec() { pos.v[pos.n-1] = int2Position(Position2int(pos.v[pos.n-1]) - 1); }
  void *last() { return pos.v[pos.n -1]; }
  int is_numeric() { for (int i = 0; i < pos.n; i++) if (!is_intPosition(pos.v[i])) return 0; return 1; }
  int last_is_numeric() { return is_intPosition(last()); }
  int prefix_to_last(MPosition &p);
  MPosition() : parent(0) {}
  MPosition(MPosition &p);
};
#define forv_MPosition(_p, _v) forv_Vec(MPosition, _p, _v)

inline int 
MPosition::prefix_to_last(MPosition &p) { 
  if (pos.n != p.pos.n + 1) return 0; 
  if (memcmp(pos.v, p.pos.v, p.pos.n * sizeof(void*)))
    return 0;
  return 1;
}

class MPositionHashFuns : public gc {
 public:
  static uint hash(MPosition *x) {
    uint h = 1;
    for (int i = 0; i < x->pos.n; i++)
      h += ((uintptr_t)x->pos.v[i]) * open_hash_multipliers[i];
    if (!h)
      h = 1;
    return h;
  }
  static int equal(MPosition *x, MPosition *y) {
    if (x->pos.n != y->pos.n)
      return 0;
    for (int i = 0; i < x->pos.n; i++)
      if (x->pos.v[i] != y->pos.v[i])
	return 0;
    return 1;
  }
};

class MType : public gc {
 public:
  Map<MPosition *, Vec<Fun *> *> funs;
};

class Patterns : public gc {
 public:
  Vec<Sym *>		types;
  Vec<Sym *>		types_set;
  Vec<MType *>		mtypes; 
};

class Match : public gc {
 public:
  Fun *fun;
  Map<MPosition *, AType *> all_filters;
  Map<MPosition *, AType *> filters;
  Map<MPosition *, MPosition *> named_to_positional;
  Map<MPosition *, MPosition *> actual_to_formal_position;
  Vec<MPosition *> default_args;
  Vec<MPosition *> generic_args;
  Vec<MPosition *> pointwise_args;
  int partial;

  Match(Fun *afun) : fun(afun), partial(0) {assert(afun);}
};
#define forv_Match(_p, _v) forv_Vec(Match, _p, _v)


void build_patterns(FA *fa);
void build_arg_positions(FA *fa);
int pattern_match(Vec<AVar *> &args, AVar *send, Partial_kind partial_ok, Vec<Match *> *matches);
MPosition *cannonicalize_mposition(MPosition &p);

#endif
