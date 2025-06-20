/**
 @file
 thresh - forming lists

 updated 3/22/09 ajm: new API

 @ingroup	examples
 */

#include "ext.h"
#include "ext_obex.h"
#include "ext_common.h"

void* thresh_class;

#define MAXSIZE 32767

typedef struct thresh
{
    t_object t_ob;
    t_atom* t_av;
    long t_ac;
    void* t_clock;
    double t_interval;
    double t_time;
    void* t_proxy;
    void* t_out;
} t_thresh;

t_symbol* ps_list;

void thresh_int(t_thresh* x, t_atom_long n);
void thresh_float(t_thresh* x, double f);
void thresh_bang(t_thresh* x);
void thresh_list(t_thresh* x, t_symbol* s, long ac, t_atom* av);
void thresh_tick(t_thresh* x);
void thresh_free(t_thresh* x);
void thresh_assist(t_thresh* x, void* b, long m, long a, char* s);
void thresh_inletinfo(t_thresh* x, void* b, long a, char* t);
void* thresh_new(double interval);

C74_EXPORT void ext_main(void* r)
{
    t_class* c;

    c = class_new("thresh", (method)thresh_new, (method)thresh_free, (short)sizeof(t_thresh), 0L, A_DEFFLOAT, 0);
    class_addmethod(c, (method)thresh_int, "int", A_LONG, 0);
    class_addmethod(c, (method)thresh_list, "list", A_GIMME, 0);
    class_addmethod(c, (method)thresh_float, "float", A_FLOAT, 0);
    class_addmethod(c, (method)thresh_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)thresh_inletinfo, "inletinfo", A_CANT, 0);
    class_register(CLASS_BOX, c);
    thresh_class = c;

    ps_list = gensym("list");

    return;
}

void thresh_int(t_thresh* x, t_atom_long n)
{
    if (proxy_getinlet((t_object*)x)) {
        x->t_interval = n < 5 ? 5 : n;
    }
    else {
        if (x->t_ac < MAXSIZE) {
            x->t_time = gettime_forobject((t_object*)x);
            atom_setlong(x->t_av + x->t_ac, n);
            x->t_ac++;
            clock_fdelay(x->t_clock, x->t_interval);
        }
    }
}

void thresh_float(t_thresh* x, double f)
{
    if (proxy_getinlet((t_object*)x)) {
        x->t_interval = f < 5 ? 5 : f;
    }
    else {
        if (x->t_ac < MAXSIZE) {
            x->t_time = gettime_forobject((t_object*)x);
            atom_setfloat(x->t_av + x->t_ac, f);
            x->t_ac++;
            clock_fdelay(x->t_clock, x->t_interval);
        }
    }
}

void thresh_bang(t_thresh* x)
{
    if (x->t_ac) {
        outlet_list(x->t_out, ps_list, x->t_ac, x->t_av);
    }
    x->t_ac = 0;
}

void thresh_list(t_thresh* x, t_symbol* s, long ac, t_atom* av)
{
    long i, upto;

    x->t_time = gettime_forobject((t_object*)x);
    upto = MIN(ac + x->t_ac, MAXSIZE);
    for (i = x->t_ac; i < upto; i++, av++) {
        x->t_av[i] = *av;
    }
    x->t_ac = upto;
    clock_fdelay(x->t_clock, x->t_interval);
}

void thresh_tick(t_thresh* x)
{
    double tt;

    tt = gettime_forobject((t_object*)x);
    if (tt - x->t_time >= x->t_interval) {
        thresh_bang(x);
    }
    else {
        clock_fdelay(x->t_clock, x->t_interval);
    }
}

void thresh_free(t_thresh* x)
{
    object_free(x->t_clock);
    object_free(x->t_proxy);
    if (x->t_av) {
        sysmem_freeptr(x->t_av);
    }
}

void thresh_inletinfo(t_thresh* x, void* b, long a, char* t)
{
    if (a) {
        *t = 1;
    }
}

void thresh_assist(t_thresh* x, void* b, long m, long a, char* s)
{
    if (m == ASSIST_OUTLET) {
        sprintf(s, "Gathered list");
    }
    else {
        switch (a) {
            case 0:
                sprintf(s, "Numbers to be Gathered Into list");
                break;
            case 1:
                sprintf(s, "Arrival Threshold");
                break;
        }
    }
}

void* thresh_new(double interval)
{
    t_thresh* x;
    long i;

    x = object_alloc(thresh_class);
    x->t_proxy = proxy_new(x, 1, NULL);
    x->t_out = outlet_new((t_object*)x, 0); // cause it sends out lists and ints
    x->t_clock = clock_new(x, (method)thresh_tick);
    x->t_time = gettime_forobject((t_object*)x);
    x->t_interval = interval < 5 ? 5 : interval;
    x->t_av = (t_atom*)sysmem_newptr(MAXSIZE * sizeof(t_atom));
    x->t_ac = 0;
    for (i = 0; i < MAXSIZE; i++) {
        atom_setlong(x->t_av + i, 0);
    }

    return x;
}
