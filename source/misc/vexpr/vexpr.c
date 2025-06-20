#include "ext.h"
#include "ext_common.h"
#include "ext_expr.h"
#include "ext_obex.h" //for atom routines

void* vexpr_class;

#define VEXPR_MINSIZE 1
#define VEXPR_MAXSIZE 32767
#define VEXPR_DEFAULTSIZE 4096
#define VEXPR_ARGVCOUNT 10
typedef struct vexpr
{
    t_object v_ob;
    t_expr* v_expr;
    void** v_proxy;
    void* v_outlet;
    t_atom* v_argv[VEXPR_ARGVCOUNT];
    short v_argc[VEXPR_ARGVCOUNT];
    short v_numargs;
    char v_scalarmode;
    long v_maxsize;
} t_vexpr;

C74_EXPORT void ext_main(void* r);
void vexpr_bang(t_vexpr* x);
void vexpr_scalarbang(t_vexpr* x);
void vexpr_vectorbang(t_vexpr* x);
void vexpr_int(t_vexpr* x, long n);
void vexpr_float(t_vexpr* x, double f);
void vexpr_list(t_vexpr* x, t_symbol* s, short ac, t_atom* av);
void vexpr_scalarmode(t_vexpr* x, long n);
void vexpr_assist(t_vexpr* x, void* b, long m, long a, char* s);
void vexpr_inletinfo(t_vexpr* x, void* b, long a, char* t);
void vexpr_free(t_vexpr* x);
void* vexpr_new(t_symbol* s, short ac, t_atom* av);
t_max_err vexpr_attr_maxsize_set(t_vexpr* x, void* attr, long ac, t_atom* av);

C74_EXPORT void ext_main(void* r)
{
    t_class* c;

    c = class_new("vexpr", (method)vexpr_new, (method)vexpr_free, (long)sizeof(t_vexpr), 0L, A_GIMME, 0);
    class_addmethod(c, (method)vexpr_bang, "bang", 0);
    class_addmethod(c, (method)vexpr_int, "int", A_LONG, 0);
    class_addmethod(c, (method)vexpr_float, "float", A_FLOAT, 0);
    class_addmethod(c, (method)vexpr_list, "list", A_GIMME, 0);
    class_addmethod(c, (method)vexpr_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)vexpr_inletinfo, "inletinfo", A_CANT, 0);

    CLASS_ATTR_CHAR(c, "scalarmode", 0, t_vexpr, v_scalarmode);
    CLASS_ATTR_STYLE_LABEL(c, "scalarmode", 0, "onoff", "Scalar Mode");
    CLASS_ATTR_CATEGORY(c, "scalarmode", 0, "Behavior");
    CLASS_ATTR_BASIC(c, "scalarmode", 0);

    CLASS_ATTR_LONG(c, "maxsize", 0, t_vexpr, v_maxsize);
    CLASS_ATTR_LABEL(c, "maxsize", 0, "Maximum List Length");
    CLASS_ATTR_ACCESSORS(c, "maxsize", (method)NULL, (method)vexpr_attr_maxsize_set)
    CLASS_ATTR_CATEGORY(c, "maxsize", 0, "Behavior");
    CLASS_ATTR_BASIC(c, "maxsize", 0);

    class_register(CLASS_BOX, c);
    vexpr_class = c;

    return;
}

t_max_err vexpr_attr_maxsize_set(t_vexpr* x, void* attr, long ac, t_atom* av)
{
    if (ac && av) {
        long i;
        t_atom_long max = atom_getlong(av);
        CLIP_ASSIGN(max, VEXPR_MINSIZE, VEXPR_MAXSIZE);
        x->v_maxsize = max;

        for (i = 0; i < VEXPR_ARGVCOUNT; i++) {
            x->v_argv[i] = (t_atom*)sysmem_resizeptr(x->v_argv[i], sizeof(t_atom) * x->v_maxsize);
            x->v_argc[i] = x->v_maxsize;
        }
    }
    return MAX_ERR_NONE;
}

void vexpr_bang(t_vexpr* x)
{
    if (x->v_scalarmode) {
        vexpr_scalarbang(x);
    }
    else {
        vexpr_vectorbang(x);
    }
}

void vexpr_scalarbang(t_vexpr* x)
{
    long i, j, count, index;
    t_atom *result, input[VEXPR_ARGVCOUNT];
    short scalar[VEXPR_ARGVCOUNT];
    t_bool hadscalar = false;
    long maxlist = x->v_maxsize + 1;

    result = (t_atom*)sysmem_newptr(sizeof(t_atom) * x->v_maxsize);
    count = maxlist; // count should be higher than the limit
    for (i = 0; i <= x->v_numargs; i++) {
        scalar[i] = x->v_argc[i] == 1;
        if (scalar[i]) {
            hadscalar = true;
        }
        else {
            count = MIN(x->v_argc[i], count);
        }
    }
    if (count == maxlist) {
        if (hadscalar) {
            count = 1;
        }
        else {
            sysmem_freeptr(result);
            return;
        }
    }
    if (count > x->v_maxsize) {
        count = x->v_maxsize;
    }

    for (i = 0; i < count; i++) {
        for (j = 0; j <= x->v_numargs; j++) {
            index = scalar[j] ? 0 : i;
            input[j] = *(x->v_argv[j] + index);
        }
        expr_eval(x->v_expr, x->v_numargs + 1, input, result + i);
    }
    if (count > 1) {
        outlet_list(x->v_outlet, 0L, count, result);
    }
    else {
        if (result[0].a_type == A_LONG) {
            outlet_int(x->v_outlet, result[0].a_w.w_long);
        }
        else if (result[0].a_type == A_FLOAT) {
            outlet_float(x->v_outlet, result[0].a_w.w_float);
        }
    }
    sysmem_freeptr(result);
}

void vexpr_vectorbang(t_vexpr* x)
{
    long i, j, count;
    t_atom *result, input[VEXPR_ARGVCOUNT];
    long maxlist = x->v_maxsize + 1;

    result = (t_atom*)sysmem_newptr(sizeof(t_atom) * x->v_maxsize);

    count = maxlist; // count should be higher than the limit
    for (i = 0; i <= x->v_numargs; i++) {
        count = MIN(x->v_argc[i], count);
    }
    if (count == maxlist) {
        sysmem_freeptr(result);
        return;
    }
    if (count > x->v_maxsize) {
        count = x->v_maxsize;
    }

    for (i = 0; i < count; i++) {
        for (j = 0; j <= x->v_numargs; j++) {
            input[j] = *(x->v_argv[j] + i);
        }
        expr_eval(x->v_expr, x->v_numargs + 1, input, result + i);
    }
    if (count > 1) {
        outlet_list(x->v_outlet, 0L, count, result);
    }
    else if (count == 1)
    {
        if (result[0].a_type == A_LONG) {
            outlet_int(x->v_outlet, result[0].a_w.w_long);
        }
        else if (result[0].a_type == A_FLOAT) {
            outlet_float(x->v_outlet, result[0].a_w.w_float);
        }
    }
    sysmem_freeptr(result);
}

void vexpr_int(t_vexpr* x, long number)
{
    long i;

    i = proxy_getinlet((t_object*)x);
    x->v_argc[i] = 1;
    if (x->v_expr->exp_var[i].ex_type == ET_FI) {
        A_SETFLOAT(x->v_argv[i], (float)number);
    }
    else {
        A_SETLONG(x->v_argv[i], number);
    }
    if (!i) {
        vexpr_bang(x);
    }
}

void vexpr_float(t_vexpr* x, double number)
{
    long i;

    i = proxy_getinlet((t_object*)x);
    x->v_argc[i] = 1;
    if (x->v_expr->exp_var[i].ex_type == ET_II) {
        A_SETLONG(x->v_argv[i], (long)number);
    }
    else {
        A_SETFLOAT(x->v_argv[i], number);
    }
    if (!i) {
        vexpr_bang(x);
    }
}

void vexpr_list(t_vexpr* x, t_symbol* s, short argc, t_atom* argv)
{
    long i, j;

    i = proxy_getinlet((t_object*)x);
    if (argc > x->v_maxsize) {
        argc = x->v_maxsize;
    }
    x->v_argc[i] = argc;

    switch (x->v_expr->exp_var[i].ex_type) {
        case ET_II:
            for (j = 0; j < argc; j++) {
                A_SETLONG((x->v_argv[i]) + j, atom_getlong(argv + j));
            }
            break;
        case ET_FI:
            for (j = 0; j < argc; j++) {
                A_SETFLOAT((x->v_argv[i]) + j, atom_getfloat(argv + j));
            }
            break;
        default:
            sysmem_copyptr(argv, x->v_argv[i], argc * sizeof(t_atom));
    }
    if (!i) {
        vexpr_bang(x);
    }
}

void vexpr_inletinfo(t_vexpr* x, void* b, long a, char* t)
{
    if (a) {
        *t = 1;
    }
}

void vexpr_assist(t_vexpr* x, void* b, long m, long a, char* s)
{
    if (m == ASSIST_INLET) {
        if (a) {
            sprintf(s, "int $i%ld, float $f%ld, table $s%ld", a + 1, a + 1, a + 1);
        }
        else {
            sprintf(s, "Evaluate Expression, int $i1, float $f1, table $s1"); // leftmost inlet
        }
    }
    else {
        sprintf(s, "Expression Result");
    }
}

void vexpr_free(t_vexpr* x)
{
    long i;

    if (x->v_proxy) {
        for (i = 0; i < x->v_numargs; i++) {
            if (x->v_proxy[i]) {
                freeobject(x->v_proxy[i]);
            }
        }
        freebytes(x->v_proxy, x->v_numargs * sizeof(void*));
    }
    if (x->v_expr) {
        freeobject((t_object*)x->v_expr);
    }
    for (i = 0; i < VEXPR_ARGVCOUNT; i++) {
        sysmem_freeptr(x->v_argv[i]);
    }
}

void* vexpr_new(t_symbol* s, short argc, t_atom* argv)
{
    t_vexpr* x;
    t_atom result[VEXPR_ARGVCOUNT];
    long i;
    long attroffset = attr_args_offset(argc, argv);

    x = (t_vexpr*)object_alloc(vexpr_class);
    x->v_outlet = outlet_new(x, 0L);
    x->v_proxy = 0;
    x->v_numargs = 0;
    x->v_scalarmode = 0;
    x->v_maxsize = VEXPR_DEFAULTSIZE;

    for (i = 0; i < VEXPR_ARGVCOUNT; i++) {
        x->v_argv[i] = (t_atom*)sysmem_newptr(x->v_maxsize * sizeof(t_atom));
        x->v_argc[i] = 0;
    }
    x->v_expr = expr_new(attroffset, argv, result);
    if (x->v_expr) { /* create an orphan object */
        for (i = 8; i >= 1; i--) {
            if (result[i].a_type) {
                if (!x->v_proxy) {
                    x->v_proxy = (void**)getbytes(i * sizeof(void*));
                    x->v_numargs = i;
                }
                x->v_proxy[i - 1] = proxy_new(x, (long)i, NULL);
            }
            else if (i < x->v_numargs) {
                x->v_proxy[i - 1] = NULL;
            }
        }
        attr_args_process(x, argc, argv);
    }
    else {
        // rbs -- failed to create expr, we should be bogus.
        freeobject((t_object*)x);
        x = NULL;
    }
    return (x);
}
