/* ai/scene.c - gl.ai.Scene type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".ai.Scene"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_ai_scene self;
    KS_ARGS("self:*", &self, ksgl_ait_scene);

    KS_NDECREF(self->src);
    KS_NDECREF(self->root);

    aiReleaseImport(self->val);


    KSO_DEL(self);
    return KSO_NONE;
}

static KS_TFUNC(T, init) {
    ksgl_ai_scene self;
    KS_ARGS("self:*", &self, ksgl_ait_scene);
    
    KS_THROW(kst_TypeError, "'%T' cannot be created directly", self);
    return NULL;
}

static KS_TFUNC(T, str) {
    ksgl_ai_scene self;
    KS_ARGS("self:*", &self, ksgl_ait_scene);

    return (kso)ks_fmt("<%T val=%p, src=%R>", self, self->val, self->src);
}
static KS_TFUNC(T, getattr) {
    ksgl_ai_scene self;
    ks_str attr;
    KS_ARGS("self:* attr:*", &self, ksgl_ait_scene, &attr, kst_str);


    if (ks_str_eq_c(attr, "src", 3)) {
        return KS_NEWREF(self->src);
    } else if (ks_str_eq_c(attr, "root", 4)) {
        return KS_NEWREF(self->root);
    } else if (ks_str_eq_c(attr, "meshes", 6)) {
        return KS_NEWREF(self->meshes);
    }

    KS_THROW_ATTR(self, attr);
    return NULL;
}

/* Export */

ks_type ksgl_ait_scene;

void _ksgl_ai_scene() {
    ksgl_ait_scene = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_ai_scene_s), -1, "Assimp scene", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self)", "")},
        {"__str",                  ksf_wrap(T_str_, T_NAME ".__str(self)", "")},
        {"__repr",                 ksf_wrap(T_str_, T_NAME ".__repr(self)", "")},
        {"__getattr",              ksf_wrap(T_getattr_, T_NAME ".__getattr(self, attr)", "")},
    ));
}
