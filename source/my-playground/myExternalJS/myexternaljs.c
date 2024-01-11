#include "ext.h" // standard Max include, always required
#include "ext_mess.h"
#include "ext_obex.h" // required for new style Max object
#include "ext_post.h"

typedef struct _myexternaljs {
  t_object ob; // the object itself (must be first)
  double myattr;
  t_object *patcher;
} t_myexternaljs;

void *new_routine(t_symbol *s, long argc, t_atom *argv);
void free_routine(t_myexternaljs *x);
void print(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av);
t_max_err doEvilThingsWith(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av,
                           t_atom *rv);

static t_class *myexternaljs_class;

void ext_main(void *r) {
  t_class *c;

  c = class_new("myexternaljs", (method)new_routine, (method)free_routine,
                (long)sizeof(t_myexternaljs), 0L /* leave NULL!! */, A_GIMME,
                0);

  class_addmethod(c, (method)print, "print", 0);
  class_addmethod(c, (method)doEvilThingsWith, "doEvilThingsWith", A_GIMMEBACK,
                  0);

  CLASS_ATTR_DOUBLE(c, "myattr", 0, t_myexternaljs, myattr);

  c->c_flags = CLASS_FLAG_POLYGLOT;
  class_register(CLASS_NOBOX, c);
  myexternaljs_class = c;
}

void free_routine(t_myexternaljs *x) { ; }

void *new_routine(t_symbol *s, long argc, t_atom *argv) {
  t_myexternaljs *x = NULL;

  if ((x = (t_myexternaljs *)object_alloc(myexternaljs_class))) {
    x->myattr = 74.;
  }
  // NOTE: Needs Max 8.6 at least
  x->patcher = gensym("#P")->s_thing;
  return (x);
}

void print(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av) {
  post("The value of myattr is: %f", x->myattr);
}

t_max_err doEvilThingsWith(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av,
                           t_atom *rv) {
  t_atom a[1];
  t_symbol *js_input;

  if (ac == 1) {
    long atom_type_1 = atom_gettype(&av[1]);
    if (atom_type_1 == A_SYM) {
      js_input = atom_getsym(&av[1]);
      post("js_input %s", js_input->s_name);
    } else {
      error("expected A_OBJ but got %d", atom_type_1);
    }
  } else {
    error("missing argument for method doEvilThingsWith()");
  }

  //
  // object_obex_lookup(x, gensym("#P"), &patcher);

  post("patcher address, using p = gensym(\"#P\")->s_thing in constructor: %lx",
       x->patcher);

  long result = jpatcher_get_count(x->patcher);

  // store the result in the a array.
  atom_setlong(a, result);

  // return the result to js
  atom_setobj(rv, object_new(gensym("nobox"), gensym("atomarray"), 1, a));

  return MAX_ERR_NONE;
}
