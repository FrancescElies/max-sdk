#include "commonsyms.h"
#include "ext.h" // standard Max include, always required
#include "ext_common.h"
#include "ext_obex.h" // required for new style Max object
#include "ext_strings.h"
#include "jpatcher_api.h"

////////////////////////// object struct
typedef struct _mytest {
  t_object ob;
  t_atom val;
  t_symbol *name;
  void *out;
} t_mytest;

///////////////////////// function prototypes
//// standard set
void *mytest_new(t_symbol *s, long argc, t_atom *argv);
void mytest_free(t_mytest *x);
void mytest_assist(t_mytest *x, void *b, long m, long a, char *s);

void _int(t_mytest *x, long n);
void _float(t_mytest *x, double f);
void anything(t_mytest *x, t_symbol *s, long ac, t_atom *av);
void bang(t_mytest *x);
void identify(t_mytest *x);
void mytest_dblclick(t_mytest *x);
void mytest_acant(t_mytest *x);
void openmaxwindowsidebar(t_mytest *client, t_symbol *s, short argc,
                          t_atom *argv);

//////////////////////// global class pointer variable
void *mytest_class;

void ext_main(void *r) {
  t_class *c;
  common_symbols_init();

  c = class_new("mytest", (method)mytest_new, (method)mytest_free,
                (long)sizeof(t_mytest), 0L /* leave NULL!! */, A_GIMME, 0);

  class_addmethod(c, (method)bang, "bang", 0);
  class_addmethod(c, (method)_int, "int", A_LONG, 0);
  class_addmethod(c, (method)_float, "float", A_FLOAT, 0);
  class_addmethod(c, (method)anything, "anything", A_GIMME, 0);
  class_addmethod(c, (method)identify, "identify", 0);
  CLASS_METHOD_ATTR_PARSE(c, "identify", "undocumented", gensym("long"), 0,
                          "1");

  // we want to 'reveal' the otherwise hidden 'mymessage' method
  class_addmethod(c, (method)anything, "mymessage", A_GIMME, 0);
  // here's an otherwise undocumented method, which does something that the user
  // can't actually do from the patcher however, we want them to know about it
  // for some weird documentation reason. so let's make it documentable. it
  // won't appear in the quickref, because we can't send it from a message.
  class_addmethod(c, (method)mytest_acant, "blooop", A_CANT, 0);
  CLASS_METHOD_ATTR_PARSE(c, "blooop", "documentable", gensym("long"), 0, "1");

  /* you CAN'T call this from the patcher */
  class_addmethod(c, (method)mytest_assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)mytest_dblclick, "dblclick", A_CANT, 0);

  CLASS_ATTR_SYM(c, "name", 0, t_mytest, name);

  class_register(CLASS_BOX, c);
  mytest_class = c;
}

void openmaxwindowsidebar(t_mytest *x, t_symbol *s, short argc, t_atom *argv) {
  // https://cycling74.com/forums/automatically-open-sidebar-max-console-on-patcher-load
  t_object *patcher = NULL;
  t_object *fv = NULL;
  t_atom rv;

  object_obex_lookup(x, _sym_pound_P, &patcher);
  fv = jpatcher_get_firstview(patcher);
  object_method(fv, _sym_command, gensym("openmaxwindowsidebar"), 0, 0L, &rv);
}

void mytest_acant(t_mytest *x) {
  object_post((t_object *)x, "can't touch this!");
}

void mytest_assist(t_mytest *x, void *b, long m, long a, char *s) {
  if (m == ASSIST_INLET) { // inlet
    sprintf(s, "I am inlet %ld", a);
  } else { // outlet
    sprintf(s, "I am outlet %ld", a);
  }
}

void mytest_free(t_mytest *x) { ; }

void mytest_dblclick(t_mytest *x) {
  object_post((t_object *)x, "I got a double-click");
}

void _int(t_mytest *x, long n) {
  atom_setlong(&x->val, n);
  bang(x);
}

void _float(t_mytest *x, double f) {
  atom_setfloat(&x->val, f);
  bang(x);
}

void openfile(t_atom *av) {
  t_atom rv;
  object_method_typed(_sym_max->s_thing, gensym("openfile"), 2, av, &rv);
}

void closefile(t_atom *av) {
  t_atom rv;
  object_method_typed(_sym_max->s_thing, gensym("closefile"), 1, av, &rv);
}

void anything(t_mytest *x, t_symbol *s, long ac, t_atom *av) {
  if (s == gensym("mymessage")) {
    object_post((t_object *)x, "A hollow voice says 'Plugh'");
  } else if (s == gensym("openmaxwindowsidebar")) {
    defer_low(x, (method)openmaxwindowsidebar, NULL, 0, NULL);
  } else if (s == gensym("openfile")) {
    openfile(av);
  } else if (s == gensym("closefile")) {
    closefile(av);
  } else {
    atom_setsym(&x->val, s);
    bang(x);
  }
}

void bang(t_mytest *x) {
  switch (x->val.a_type) {
  case A_LONG:
    outlet_int(x->out, atom_getlong(&x->val));
    break;
  case A_FLOAT:
    outlet_float(x->out, atom_getfloat(&x->val));
    break;
  case A_SYM:
    outlet_anything(x->out, atom_getsym(&x->val), 0, NULL);
    break;
  default:
    break;
  }
}

void identify(t_mytest *x) {
  object_post((t_object *)x, "my name is %s", x->name->s_name);
}

void *mytest_new(t_symbol *s, long argc, t_atom *argv) {
  t_mytest *x = NULL;

  if ((x = (t_mytest *)object_alloc(mytest_class))) {
    x->name = gensym("");
    if (argc && argv) {
      x->name = atom_getsym(argv);
    }
    if (!x->name || x->name == gensym(""))
      x->name = symbol_unique();

    atom_setlong(&x->val, 0);
    x->out = outlet_new(x, NULL);
  }

  defer_low(x, (method)openmaxwindowsidebar, NULL, 0, NULL);

  return (x);
}
