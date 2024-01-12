#include "ext.h" // standard Max include, always required
#include "ext_mess.h"
#include "ext_obex.h" // required for new style Max object
#include "ext_post.h"
#include "ext_proto.h"
#include <stdio.h>

typedef struct _myexternaljs {
  t_object ob; // the object itself (must be first)
  double myattr;
  t_object *patcher;
  t_symbol *patcher_name;
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
  x->patcher_name = jpatcher_get_name(x->patcher);
  return (x);
}
char *get_long_string() {
  size_t desired_length_bytes =
      32 * 1024 - 1; // 32KB (-1 for null ternimator)  32767 chars
  //
  // size_t desired_length_bytes = 32 * 1024; // 💥BOOM, max silently truncates

  // Allocate memory for the string
  char *long_string =
      (char *)malloc(desired_length_bytes + 1); // +1 for the null terminator

  if (long_string == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  // Fill the string with a specific character (e.g., 'a')
  for (size_t i = 0; i < desired_length_bytes; i++) {
    long_string[i] = 'a';
  }
  long_string[desired_length_bytes - 1] = 'X';

  // Add the null terminator at the end
  long_string[desired_length_bytes] = '\0';
  printf("%s", long_string);

  return long_string;
}
void print(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av) {
  post("The value of myattr is: %f", x->myattr);
}

#define AC 3

t_max_err doEvilThingsWith(t_myexternaljs *x, t_symbol *s, long ac, t_atom *av,
                           t_atom *rv) {
  t_atom a[AC];
  t_symbol *js_input;

  if (ac == 1) {
    long first_atom = atom_gettype(av);
    if (first_atom == A_SYM) {
      js_input = atom_getsym(av);
      post("js_input %s", js_input->s_name);
    } else {
      error("expected A_OBJ but got %d", first_atom);
    }
  } else {
    error("missing argument for method doEvilThingsWith()");
  }

  // Test 1: return a long string
  char *a_long_string = get_long_string();

  // Test 2: get current patcher object count
  long number_objects_in_patcher = jpatcher_get_count(x->patcher);

  // store the result in the a array.
  atom_setsym(a, gensym(a_long_string));
  atom_setsym(&a[1], x->patcher_name);
  atom_setlong(&a[2], number_objects_in_patcher);

  // return the result to js
  atom_setobj(rv, object_new(gensym("nobox"), gensym("atomarray"), AC, a));

  return MAX_ERR_NONE;
}
