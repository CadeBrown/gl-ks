/* vao.c - gl.VAO type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".VAO"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_vao self;
    KS_ARGS("self:*", &self, ksglt_vao);

    if (self->val >= 0) glDeleteVertexArrays(1, (GLuint[]){ self->val });

    KSO_DEL(self);
    return KSO_NONE;
}


static KS_TFUNC(T, init) {
    ksgl_vao self;
    KS_ARGS("self:*", &self, ksglt_vao);

    /* Create buffer object */
    GLuint t;
    glGenVertexArrays(1, &t);
    self->val = t;

    return KSO_NONE;
}

static KS_TFUNC(T, bind) {
    ksgl_vao self;
    KS_ARGS("self:*", &self, ksglt_vao);

    glBindVertexArray(self->val);

    return KSO_NONE;
}

static KS_TFUNC(T, unbind) {
    ksgl_vao self;
    KS_ARGS("self:*", &self, ksglt_vao);

    glBindVertexArray(0);

    return KSO_NONE;
}

static KS_TFUNC(T, attrib) {
    ksgl_vao self;
    ks_cint index, size, type, normalize, stride, offset = 0;
    KS_ARGS("self:* index:cint size:cint type:cint normalize:cint stride:cint ?offset:cint", &self, ksglt_vao, &index, &size, &type, &normalize, &stride, &offset);

    glVertexAttribPointer(index, size, type, normalize, stride, (void*)offset);

    return KSO_NONE;
}

static KS_TFUNC(T, enableAttrib) {
    ksgl_vao self;
    ks_cint index;
    KS_ARGS("self:* index:cint", &self, ksglt_vao, &index);

    glEnableVertexAttribArray(index);

    return KSO_NONE;
}
static KS_TFUNC(T, disableAttrib) {
    ksgl_vao self;
    ks_cint index;
    KS_ARGS("self:* index:cint", &self, ksglt_vao, &index);

    glDisableVertexAttribArray(index);

    return KSO_NONE;
}


/* Export */

ks_type ksglt_vao;

void _ksgl_vao() {
    ksglt_vao = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_vao_s), -1, "OpenGL vertex array object (vao)", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self)", "")},

        {"bind",                   ksf_wrap(T_bind_, T_NAME ".bind(self)", "Bind this vertex array object as the current one")},
        {"unbind",                 ksf_wrap(T_unbind_, T_NAME ".unbind(self)", "Unbind this vertex array")},

        {"attrib",                 ksf_wrap(T_attrib_, T_NAME ".attrib(self, index, size, type, normalize, stride, offset=0)", "Add an attribute pointer to the vao")},
        {"enableAttrib",           ksf_wrap(T_enableAttrib_, T_NAME ".enableAttrib(self, index)", "Enables a vertex attribute")},
        {"disableAttrib",          ksf_wrap(T_disableAttrib_, T_NAME ".disableAttrib(self, index)", "Disables a vertex attribute")},

    ));
}

