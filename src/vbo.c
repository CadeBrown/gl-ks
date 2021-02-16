/* vbo.c - gl.VBO type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".VBO"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_vbo self;
    KS_ARGS("self:*", &self, ksglt_vbo);

    if (self->val >= 0) glDeleteBuffers(1, (GLuint[]){ self->val });

    KSO_DEL(self);
    return KSO_NONE;
}



static KS_TFUNC(T, init) {
    ksgl_vbo self;
    kso data = KSO_NONE;
    ks_cint usage = GL_STATIC_DRAW;
    KS_ARGS("self:* ?data ?usage:cint", &self, ksglt_vbo, &data, &usage);

    /* Create buffer object */
    GLuint t;
    glGenBuffers(1, &t);
    self->val = t;

    /* Convert the data to its bytes equivalent */
    ks_bytes data_bytes = kso_bytes(data);
    if (!data_bytes) {
        return NULL;
    }

    /* Bind as the currently used buffer */
    glBindBuffer(GL_ARRAY_BUFFER, self->val);
    glBufferData(GL_ARRAY_BUFFER, data_bytes->len_b, data_bytes->data, usage);

    /* Done with the bytes */
    KS_DECREF(data_bytes);

    return KSO_NONE;
}
static KS_TFUNC(T, bind) {
    ksgl_vbo self;
    KS_ARGS("self:*", &self, ksglt_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, self->val);

    return KSO_NONE;
}

static KS_TFUNC(T, unbind) {
    ksgl_vbo self;
    KS_ARGS("self:*", &self, ksglt_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return KSO_NONE;
}


/* Export */

ks_type ksglt_vbo;

void _ksgl_vbo() {
    ksglt_vbo = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_vbo_s), -1, "OpenGL vertex buffer object (VBO)", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, data='', usage=gl.STATIC_DRAW)", "")},

        {"bind",                   ksf_wrap(T_bind_, T_NAME ".bind(self)", "Bind this vertex buffer object as the current one")},
        {"unbind",                 ksf_wrap(T_unbind_, T_NAME ".unbind(self)", "Unbind this vertex buffer object")},
    ));
}

