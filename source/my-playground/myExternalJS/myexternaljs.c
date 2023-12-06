#include "ext.h"      // standard Max include, always required
#include "ext_obex.h" // required for new style Max object

typedef struct _myexternaljs {
  t_object ob; // the object itself (must be first)
  double myattr;
} t_myexternaljs;

void *myexternaljs_new(t_symbol *s, long argc, t_atom *argv);
void myexternaljs_free(t_myexternaljs *x);
void myexternaljs_print(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av);
t_max_err myexternaljs_doabs(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av, t_atom *rv);

static t_class *myexternaljs_class;

void ext_main(void *r) {
  t_class *c;

  c = class_new("myexternaljs", (method)myexternaljs_new, (method)myexternaljs_free,
                (long)sizeof(t_myexternaljs), 0L /* leave NULL!! */, A_GIMME, 0);

  class_addmethod(c, (method)myexternaljs_print, "print", 0);
  class_addmethod(c, (method)myexternaljs_doabs, "doAbs", A_GIMMEBACK, 0);

  CLASS_ATTR_DOUBLE(c, "myattr", 0, t_myexternaljs, myattr);

  c->c_flags = CLASS_FLAG_POLYGLOT;
  class_register(CLASS_NOBOX, c);
  myexternaljs_class = c;
}

void myexternaljs_free(t_myexternaljs *x) { ; }

void *myexternaljs_new(t_symbol *s, long argc, t_atom *argv) {
  t_myexternaljs *x = NULL;

  if ((x = (t_myexternaljs *)object_alloc(myexternaljs_class))) {
    x->myattr = 74.;
  }
  return (x);
}

void myexternaljs_print(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av) {
  post("The value of myattr is: %f", x->myattr);
}

t_max_err myexternaljs_doabs(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av, t_atom *rv) {
  t_atom a[1];
  double f = 0;

  if (ac) {
    if (atom_gettype(av) == A_LONG)
      f = (double)abs(atom_getlong(av));
    else if (atom_gettype(av) == A_FLOAT)
      f = fabs(atom_getfloat(av));
  } else
    error("missing argument for method doAbs()");

  // store the result in the a array.
  atom_setfloat(a, f);

  // return the result to js
  atom_setobj(rv, object_new(gensym("nobox"), gensym("atomarray"), 1, a));

  return MAX_ERR_NONE;
}
