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

    self->val = -1;

    /* Create buffer object */
    GLuint t;
    glGenBuffers(1, &t);
    self->val = t;
    if (!ksgl_check()) {
        return NULL;
    }

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

    if (!ksgl_check()) {
        return NULL;
    }


    return KSO_NONE;
}

static KS_TFUNC(T, integral) {
    ksgl_vbo self;
    KS_ARGS("self:*", &self, ksglt_vbo);

    return (kso)ks_int_new(self->val);
}


static KS_TFUNC(T, bind) {
    ksgl_vbo self;
    KS_ARGS("self:*", &self, ksglt_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, self->val);
    if (!ksgl_check()) {
        return NULL;
    }

    return KSO_NONE;
}

static KS_TFUNC(T, unbind) {
    ksgl_vbo self;
    KS_ARGS("self:*", &self, ksglt_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (!ksgl_check()) {
        return NULL;
    }
    
    return KSO_NONE;
}


static KS_TFUNC(T, write) {
    ksgl_vbo self;
    kso data;
    ks_cint offset = 0;
    KS_ARGS("self:* data ?offset:cint", &self, ksglt_vbo, &data, &offset);

    /* Bind for writing */
    glBindBuffer(GL_ARRAY_BUFFER, self->val);
    if (!ksgl_check()) {
        return NULL;
    }

    /* Convert the data to its bytes equivalent */
    ks_bytes data_bytes = kso_bytes(data);
    if (!data_bytes) {
        return NULL;
    }

    glBufferSubData(GL_ARRAY_BUFFER, offset, data_bytes->len_b, data_bytes->data);
    KS_DECREF(data_bytes);
    if (!ksgl_check()) {
        return NULL;
    }

    return KSO_NONE;
}


static KS_TFUNC(T, read) {
    ksgl_vbo self;
    ks_cint sz = -1;
    ks_cint offset = 0;
    KS_ARGS("self:* sz:cint ?offset:cint", &self, ksglt_vbo, &sz, &offset);

    /* Bind for writing */
    glBindBuffer(GL_ARRAY_BUFFER, self->val);
    if (!ksgl_check()) {
        return NULL;
    }

    if (sz < 0) {
        /* Calculate size*/
        GLint tsz = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &tsz);
        sz = tsz - offset;
        if (sz < 0) sz = 0;
    }

    void* data = ks_malloc(sz);
    if (!data) {
        KS_THROW(kst_Error, "Failed to allocate data");
        return NULL;
    }

    glGetBufferSubData(GL_ARRAY_BUFFER, offset, sz, data);
    if (!ksgl_check()) {
        ks_free(data);
        return NULL;
    }

    return (kso)ks_bytes_newn(sz, data);
}

/* Export */

ks_type ksglt_vbo;

void _ksgl_vbo() {
    ksglt_vbo = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_vbo_s), -1, "OpenGL vertex buffer object (VBO)", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, data='', usage=gl.STATIC_DRAW)", "")},

        {"__integral",             ksf_wrap(T_integral_, T_NAME ".__integral(self)", "Converts to an integer (the OpenGL handle)")},

        {"bind",                   ksf_wrap(T_bind_, T_NAME ".bind(self)", "Bind this vertex buffer object as the current one")},
        {"unbind",                 ksf_wrap(T_unbind_, T_NAME ".unbind(self)", "Unbind this vertex buffer object")},

        {"read",                   ksf_wrap(T_read_, T_NAME ".read(self, sz=-1, offset=0)", "Reads part of the buffer (default: all of the buffer), and returns a bytes object")},
        {"write",                  ksf_wrap(T_write_, T_NAME ".write(self, data, offset=0)", "Writes a bytes-like object to the buffer at the given offset (default: beginning)")},


    ));
}

