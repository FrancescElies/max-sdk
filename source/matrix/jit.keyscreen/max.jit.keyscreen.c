/*
        Copyright 2001-2005 - Cycling '74
        R. Luke Dubois luke@cycling74.com
*/

// updated for new architecture by rld, 2/23/02

#include "jit.common.h"
#include "max.jit.mop.h"

typedef struct _max_jit_keyscreen
{
    t_object ob;
    void* obex;
} t_max_jit_keyscreen;

t_jit_err jit_keyscreen_init(void);

void* max_jit_keyscreen_new(t_symbol* s, long argc, t_atom* argv);
void max_jit_keyscreen_free(t_max_jit_keyscreen* x);
t_messlist* max_jit_keyscreen_class;

C74_EXPORT void ext_main(void* r)
{
    void *p, *q;

    jit_keyscreen_init();
    setup(&max_jit_keyscreen_class, (method)max_jit_keyscreen_new, (method)max_jit_keyscreen_free, (short)sizeof(t_max_jit_keyscreen),
          0L, A_GIMME, 0);

    p = max_jit_classex_setup(calcoffset(t_max_jit_keyscreen, obex));
    q = jit_class_findbyname(gensym("jit_keyscreen"));
    max_jit_classex_mop_wrap(p, q, 0);
    max_jit_classex_standard_wrap(p, q, 0);
    addmess((method)max_jit_mop_assist, "assist", A_CANT, 0);
}

void max_jit_keyscreen_free(t_max_jit_keyscreen* x)
{
    max_jit_mop_free(x);
    jit_object_free(max_jit_obex_jitob_get(x));
    max_jit_obex_free(x);
}

void* max_jit_keyscreen_new(t_symbol* s, long argc, t_atom* argv)
{
    t_max_jit_keyscreen *x, *o;

    if (x = (t_max_jit_keyscreen*)max_jit_obex_new(max_jit_keyscreen_class, gensym("jit_keyscreen"))) {
        if (o = jit_object_new(gensym("jit_keyscreen"))) {
            max_jit_mop_setup_simple(x, o, argc, argv);
            max_jit_attr_args(x, argc, argv);
        }
        else {
            jit_object_error((t_object*)x, "jit.keyscreen: could not allocate object");
            freeobject((t_object*)x);
            x = NULL;
        }
    }
    return (x);
}
