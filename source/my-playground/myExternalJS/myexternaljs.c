#include "ext.h" // standard Max include, always required
#include "ext_mess.h"
#include "ext_obex.h" // required for new style Max object

typedef struct _myexternaljs {
  t_object ob; // the object itself (must be first)
  double myattr;
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
  return (x);
}

void print(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av) {
  post("The value of myattr is: %f", x->myattr);
}

t_max_err doEvilThingsWith(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av,
                           t_atom *rv) {
  t_atom a[1];
  void *patcher;

  if (ac == 1) {
    post("Argument Count (ac): %d", ac);
  } else if (ac == 2) {
    post("Argument Count (ac): %d", ac);
    long atom_type_0 = atom_gettype(av);
    long atom_type_1 = atom_gettype(&av[1]);
    post("Argument Type (1) %d (2) %d", atom_type_0, atom_type_1);
    if (atom_type_1 == A_OBJ)
      patcher = atom_getobj(&av[1]);
    else
      error("expected A_OBJ but got %d", atom_type_1);
  } else
    error("missing argument for method doEvilThingsWith()");

  // 💥BOOM💥 this crashes!!
  // jpatcher_get_count(patcher);

  // store the result in the a array.
  atom_setlong(a, 1);

  // return the result to js
  atom_setobj(rv, object_new(gensym("nobox"), gensym("atomarray"), 1, a));

  return MAX_ERR_NONE;
}
