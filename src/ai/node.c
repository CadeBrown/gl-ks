/* ai/node.c - gl.ai.Node type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".ai.Node"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_ai_node self;
    KS_ARGS("self:*", &self, ksgl_ait_node);

    KSO_DEL(self);
    return KSO_NONE;
}

static KS_TFUNC(T, init) {
    ksgl_ai_node self;
    KS_ARGS("self:*", &self, ksgl_ait_node);
    
    KS_THROW(kst_TypeError, "'%T' cannot be created directly", self);
    return NULL;
}

static KS_TFUNC(T, str) {
    ksgl_ai_node self;
    KS_ARGS("self:*", &self, ksgl_ait_node);

    return (kso)ks_fmt("<%T val=%p, name='%.*s'>", self, self->val, (int)self->val->mName.length, self->val->mName.data);
}
static KS_TFUNC(T, getattr) {
    ksgl_ai_node self;
    ks_str attr;
    KS_ARGS("self:* attr:*", &self, ksgl_ait_node, &attr, kst_str);

    if (ks_str_eq_c(attr, "name", 4)) {
        return (kso)ks_str_new(self->val->mName.length, self->val->mName.data);
    } else if (ks_str_eq_c(attr, "sub", 3)) {
        return KS_NEWREF(self->sub);
    } else if (ks_str_eq_c(attr, "meshes", 6)) {
        ks_list res = ks_list_new(0, NULL);
        int i;
        for (i = 0; i < self->val->mNumMeshes; ++i) {
            ks_list_pushu(res, (kso)ks_int_new(self->val->mMeshes[i]));
        }

        return (kso)res;

    } else if (ks_str_eq_c(attr, "transform", 9)) {
        nx_F v[4][4];

        #define _V(i, j, n) v[i][j] = self->val->mTransformation.n
        _V(0, 0, a1);
        _V(0, 1, a2);
        _V(0, 2, a3);
        _V(0, 3, a4);
        _V(1, 0, b1);
        _V(1, 1, b2);
        _V(1, 2, b3);
        _V(1, 3, b4);
        _V(2, 0, c1);
        _V(2, 1, c2);
        _V(2, 2, c3);
        _V(2, 3, c4);
        _V(3, 0, d1);
        _V(3, 1, d2);
        _V(3, 2, d3);
        _V(3, 3, d4);

        return (kso)nx_array_newc(nxt_array, (void*)&v[0][0], nxd_F, 2, (ks_size_t[]){ 4, 4 }, NULL);
    }

    KS_THROW_ATTR(self, attr);
    return NULL;
}



/* Export */

ks_type ksgl_ait_node;

void _ksgl_ai_node() {
    ksgl_ait_node = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_ai_node_s), -1, "Assimp scene", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self)", "")},
        {"__str",                  ksf_wrap(T_str_, T_NAME ".__str(self)", "")},
        {"__repr",                 ksf_wrap(T_str_, T_NAME ".__repr(self)", "")},
        {"__getattr",              ksf_wrap(T_getattr_, T_NAME ".__getattr(self, attr)", "")},
    ));
}
