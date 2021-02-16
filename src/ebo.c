/* ebo.c - gl.EBO type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".EBO"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_ebo self;
    KS_ARGS("self:*", &self, ksglt_ebo);

    if (self->val >= 0) glDeleteBuffers(1, (GLuint[]){ self->val });

    KSO_DEL(self);
    return KSO_NONE;
}



static KS_TFUNC(T, init) {
    ksgl_ebo self;
    kso data = KSO_NONE;
    ks_cint usage = GL_STATIC_DRAW;
    KS_ARGS("self:* ?data ?usage:cint", &self, ksglt_ebo, &data, &usage);

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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->val);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_bytes->len_b, data_bytes->data, usage);

    /* Done with the bytes */
    KS_DECREF(data_bytes);

    return KSO_NONE;
}
static KS_TFUNC(T, bind) {
    ksgl_ebo self;
    KS_ARGS("self:*", &self, ksglt_ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->val);

    return KSO_NONE;
}

static KS_TFUNC(T, unbind) {
    ksgl_ebo self;
    KS_ARGS("self:*", &self, ksglt_ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return KSO_NONE;
}


/* Export */

ks_type ksglt_ebo;

void _ksgl_ebo() {
    ksglt_ebo = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_ebo_s), -1, "OpenGL element buffer object (ebo)", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, data='', usage=gl.STATIC_DRAW)", "")},

        {"bind",                   ksf_wrap(T_bind_, T_NAME ".bind(self)", "Bind this element buffer object as the current one")},
        {"unbind",                 ksf_wrap(T_unbind_, T_NAME ".unbind(self)", "Unbind this element buffer object")},
    ));
}

