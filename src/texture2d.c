/* texture2d.c - gl.Texture2D type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".Texture2D"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_texture2d self;
    KS_ARGS("self:*", &self, ksglt_texture2d);

    if (self->val >= 0) glDeleteTextures(1, (GLuint[]){ self->val });

    KSO_DEL(self);
    return KSO_NONE;
}



static KS_TFUNC(T, init) {
    ksgl_texture2d self;
    kso data = KSO_NONE;
    ks_cint width = -1, height = -1;
    ks_cint format = GL_RGBA;
    ks_cint type = GL_UNSIGNED_BYTE;
    ks_cint internalformat = -1;
    KS_ARGS("self:* ?data ?width:cint ?height:cint ?format:cint ?type:cint ?internalformat:cint", &self, ksglt_texture2d, &data, &width, &height, &format, &type, &internalformat);

    if (internalformat < 0) internalformat = format;


    /* Create buffer object */
    GLuint t;
    glGenTextures(1, &t);
    self->val = t;

    /* Convert the data to its bytes equivalent */
    ks_bytes data_bytes = kso_bytes(data);
    if (!data_bytes) {
        return NULL;
    }

    /* Bind as the currently used texture */
    glBindTexture(GL_TEXTURE_2D, self->val);
    if (!ksgl_check()) {
        return NULL;
    }

    /* Set default parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Now, figure out image conversion */
    if (data != KSO_NONE) {
        if (width < 0 || height < 0) {
            KS_THROW(kst_Error, "'width' and 'height' must be given if 'data' is given");
            return NULL;
        }
        /* Upload image data */
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data_bytes->data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    /* Done with the bytes */
    KS_DECREF(data_bytes);

    return KSO_NONE;
}

static KS_TFUNC(T, integral) {
    ksgl_texture2d self;
    KS_ARGS("self:*", &self, ksglt_texture2d);

    return (kso)ks_int_new(self->val);
}

static KS_TFUNC(T, write) {
    ksgl_texture2d self;
    kso data;
    ks_cint width, height;
    ks_cint format = GL_RGBA;
    ks_cint type = GL_UNSIGNED_BYTE;
    ks_cint internalformat = -1;
    KS_ARGS("self:* data width:cint height:cint ?format:cint ?type:cint ?internalformat:cint", &self, ksglt_texture2d, &data, &width, &height, &format, &type, &internalformat);

    if (internalformat < 0) internalformat = format;

    /* Convert the data to its bytes equivalent */
    ks_bytes data_bytes = kso_bytes(data);
    if (!data_bytes) {
        return NULL;
    }

    if (width < 0 || height < 0) {
        KS_THROW(kst_Error, "'width' and 'height' must be given if 'data' is given");
        return NULL;
    }

    /* Bind as the currently used texture */
    glBindTexture(GL_TEXTURE_2D, self->val);
    if (!ksgl_check()) {
        return NULL;
    }

    /* Upload image data */
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data_bytes->data);
    if (!ksgl_check()) {
        return NULL;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    if (!ksgl_check()) {
        return NULL;
    }

    KS_DECREF(data_bytes);

    return KSO_NONE;
}


static KS_TFUNC(T, bind) {
    ksgl_texture2d self;
    ks_cint idx;
    KS_ARGS("self:* idx:cint", &self, ksglt_texture2d, &idx);

    if (idx < 0 || idx >= 16) {
        KS_THROW(kst_Error, "Bad texture unit: %i. Only 0 through 15 supported", (int)idx);
        return NULL;
    }

    /* Bind to that texture */
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, self->val);

    return KSO_NONE;
}

static KS_TFUNC(T, unbind) {
    ksgl_texture2d self;
    KS_ARGS("self:*", &self, ksglt_texture2d);

    glBindTexture(GL_TEXTURE_2D, 0);

    return KSO_NONE;
}



/* Export */

ks_type ksglt_texture2d;

void _ksgl_texture2d() {
    ksglt_texture2d = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_texture2d_s), -1, "OpenGL 2D texture", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, data='', width=none, height=none, format=gl.RGBA, type=gl.UNSIGNED_BYTE, internalformat=-1)", "If 'internalformat < 0', then it is set equal to 'format'")},

        {"__integral",             ksf_wrap(T_integral_, T_NAME ".__integral(self)", "Converts to an integer (the OpenGL handle)")},

        {"bind",                   ksf_wrap(T_bind_, T_NAME ".bind(self)", "Bind this vertex buffer object as the current one")},
        {"unbind",                 ksf_wrap(T_unbind_, T_NAME ".unbind(self)", "Unbind this vertex buffer object")},
    
        {"write",                  ksf_wrap(T_write_, T_NAME ".write(self, data, width, height, format=gl.RGBA, type=gl.UNSIGNED_BYTE, internalformat=-1)", "Write to the image. If 'internalformat < 0', then it is set equal to 'format'")},
    
    
    ));
}

