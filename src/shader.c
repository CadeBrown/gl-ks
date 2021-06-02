/* shader.c - gl.shader type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".Shader"


/* Internals */

/* Maximum size of information log */
#define KSGL_INFOLOG_MAX 1024

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_shader self;
    KS_ARGS("self:*", &self, ksglt_shader);

    if (self->val >= 0) glDeleteProgram(self->val);

    KSO_DEL(self);
    return KSO_NONE;
}



/* Internal compilation, which takes a kind (GL_VERTEX_SHADER, etc),
 *   source
 */
static int compile_shader(int kind, ks_str src) {
    /* Create shader */
    int sh = glCreateShader(kind);
    glShaderSource(sh, 1, (const char*[]){ 
        (const char*)src->data 
    }, NULL);
    glCompileShader(sh);
    if (!ksgl_check()) {
        glDeleteShader(sh);
        return -1;
    }

    /* Check success status */
    int success;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &success);
    if (!success) {
        /* Query message */
        char infolog[KSGL_INFOLOG_MAX];
        glGetShaderInfoLog(sh, KSGL_INFOLOG_MAX, NULL, infolog);

        /* Delete the shader */
        glDeleteShader(sh);

        /* Throw */
        KS_THROW(kst_Error, "Compiling '%s' shader failed: %s", kind == GL_VERTEX_SHADER ? "vertex" : (kind == GL_FRAGMENT_SHADER ? "fragment" : "unknown"), infolog);
        return -1;
    }

    return sh;
}


/* Internal program maker, from a list of shaders
 */
static int make_program(int nshaders, int* shaders) {
    /* Create complete shader program */
    int prog = glCreateProgram();
    if (!ksgl_check()) {
        glDeleteProgram(prog);
        return -1;
    }

    /* Attach all prts */
    int i;
    for (i = 0; i < nshaders; ++i) {
        glAttachShader(prog, shaders[i]);
        if (!ksgl_check()) {
            glDeleteProgram(prog);
            return -1;
        }
    }

    glLinkProgram(prog);
    if (!ksgl_check()) {
        glDeleteProgram(prog);
        return -1;
    }

    /* Check status */
    int success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        /* Query message */
        char infolog[KSGL_INFOLOG_MAX];
        glGetProgramInfoLog(prog, KSGL_INFOLOG_MAX, NULL, infolog);

        /* Delete program */
        glDeleteProgram(prog);

        /* Throw */
        KS_THROW(kst_Error, "Linking shader failed: %s", infolog);
        return -1;
    }

    return prog;
}

static KS_TFUNC(T, init) {
    ksgl_shader self;
    ks_str src_vert, src_frag;
    KS_ARGS("self:* src_vert:* src_frag:*", &self, ksglt_shader, &src_vert, kst_str, &src_frag, kst_str);

    self->val = -1;
    
    /* Compile vertex shader */
    int sh_vert = compile_shader(GL_VERTEX_SHADER, src_vert);
    if (sh_vert < 0) {
        return NULL;
    }

    /* Compile fragment shader */
    int sh_frag = compile_shader(GL_FRAGMENT_SHADER, src_frag);
    if (sh_frag < 0) {
        glDeleteShader(sh_vert);
        return NULL;
    }

    self->val = make_program(2, (int[]) { sh_vert, sh_frag });
    glDeleteShader(sh_vert);
    glDeleteShader(sh_frag);
    if (self->val < 0) {
        return NULL;
    }

    return KSO_NONE;
}

static KS_TFUNC(T, integral) {
    ksgl_shader self;
    KS_ARGS("self:*", &self, ksglt_shader);

    return (kso)ks_int_new(self->val);
}


static KS_TFUNC(T, use) {
    ksgl_shader self;
    KS_ARGS("self:*", &self, ksglt_shader);

    glUseProgram(self->val);
    if (!ksgl_check()) {
        return NULL;
    }
    
    return KSO_NONE;
}
static KS_TFUNC(T, uniformloc) {
    ksgl_shader self;
    ks_str name;
    KS_ARGS("self:* name:*", &self, ksglt_shader, &name, kst_str);

    int pos = glGetUniformLocation(self->val, name->data);
    if (pos < 0) {
        KS_THROW(kst_Error, "Unknown uniform %R", name);
        return NULL;
    }
    if (!ksgl_check()) {
        return NULL;
    }

    return (kso)ks_int_new(pos);
}

static KS_TFUNC(T, uniform) {
    ksgl_shader self;
    ks_str name;
    kso val;
    KS_ARGS("self:* name:* val", &self, ksglt_shader, &name, kst_str, &val);

    int pos = glGetUniformLocation(self->val, name->data);
    if (pos < 0) {
        KS_THROW(kst_Error, "Unknown uniform %R", name);
        return NULL;
    }

    if (kso_is_int(val)) {
        /* Set as integer */
        ks_cint v;
        if (!kso_get_ci(val, &v)) {
            return NULL;
        }

        glUniform1i(pos, v);

    } else {
        /* Some sort of matrix/vector */

        nx_t vn;
        kso ref = NULL;
        if (!nx_get(val, nxd_F, &vn, &ref)) {
            return NULL;
        }

        if (vn.rank == 0) {
            /* Scalar */
            glUniform1f(pos, *(nx_F*)vn.data);
        } else if (vn.rank == 1) {
            /* Vector */
            int n = vn.shape[0];
            int sn = vn.strides[0];
            #define GET(_i) (*(nx_F*)((ks_uint)vn.data + sn * (_i)))

            if (n == 1) {
                glUniform1f(pos, GET(0));
            } else if (n == 2) {
                glUniform2f(pos, GET(0), GET(1));
            } else if (n == 3) {
                glUniform3f(pos, GET(0), GET(1), GET(2));
            } else if (n == 4) {
                glUniform4f(pos, GET(0), GET(1), GET(2), GET(3));
            } else {
                KS_THROW(kst_SizeError, "Expected rank-1 array to have length 1, 2, 3, or 4 for shader uniform");
                KS_NDECREF(ref);
                return NULL;
            }
            #undef GET
        } else if (vn.rank == 2) {
            /* Vector */
            int m = vn.shape[0], n = vn.shape[1];
            int sm = vn.strides[0], sn = vn.strides[1];
            GLfloat v[16];

            #define GET(_i, _j) (*(nx_F*)((ks_uint)vn.data + sm * (_i) + sn * (_j)))

            /* Copy into 'v', as dense array */
            #define COPY() do { \
                int i, j; \
                for (i = 0; i < m; ++i) { \
                    for (j = 0; j < n; ++j) { \
                        v[i * n + j] = GET(i, j); \
                    } \
                } \
            } while (0)

            if (m == 1 && n == 1) {
                glUniform1f(pos, GET(0, 0));
            } else if (m == 1 && n == 2) {
                glUniform2f(pos, GET(0, 0), GET(0, 1));
            } else if (m == 1 && n == 3) {
                glUniform3f(pos, GET(0, 0), GET(0, 1), GET(0, 2));
            } else if (m == 1 && n == 4) {
                glUniform4f(pos, GET(0, 0), GET(0, 1), GET(0, 2), GET(0, 3));
            } else if (m == 1 && n == 1) {
                glUniform1f(pos, GET(0, 0));
            } else if (m == 2 && n == 1) {
                glUniform2f(pos, GET(0, 0), GET(1, 0));
            } else if (m == 3 && n == 1) {
                glUniform3f(pos, GET(0, 0), GET(1, 0), GET(2, 0));
            } else if (m == 4 && n == 1) {
                glUniform4f(pos, GET(0, 0), GET(1, 0), GET(2, 0), GET(3, 0));

            } else if (m == 2 && n == 2) {
                COPY();
                glUniformMatrix2fv(pos, 1, GL_TRUE, v);
            } else if (m == 2 && n == 3) {
                COPY();
                glUniformMatrix2x3fv(pos, 1, GL_TRUE, v);
            } else if (m == 2 && n == 4) {
                COPY();
                glUniformMatrix2x4fv(pos, 1, GL_TRUE, v);
            
            } else if (m == 3 && n == 2) {
                COPY();
                glUniformMatrix3x2fv(pos, 1, GL_TRUE, v);
            } else if (m == 3 && n == 3) {
                COPY();
                glUniformMatrix3fv(pos, 1, GL_TRUE, v);
            } else if (m == 3 && n == 4) {
                COPY();
                glUniformMatrix3x4fv(pos, 1, GL_TRUE, v);

            } else if (m == 4 && n == 2) {
                COPY();
                glUniformMatrix4x2fv(pos, 1, GL_TRUE, v);
            } else if (m == 4 && n == 3) {
                COPY();
                glUniformMatrix4x3fv(pos, 1, GL_TRUE, v);
            } else if (m == 4 && n == 4) {
                COPY();
                glUniformMatrix4fv(pos, 1, GL_TRUE, v);

            } else {
                KS_THROW(kst_SizeError, "Expected rank-2 array to have length 1, 2, 3, or 4 for both shapes");
                KS_NDECREF(ref);
                return NULL;
            }

            #undef GET
        } else {
            KS_THROW(kst_SizeError, "Only expected rank-0, rank-1, or rank-2 arrays for shader uniform");
            KS_NDECREF(ref);
            return NULL;
        }

        KS_NDECREF(ref);
    }


    return KSO_NONE;
}



/* Export */

ks_type ksglt_shader;

void _ksgl_shader() {
    ksglt_shader = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_shader_s), -1, "OpenGL shader", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, src_vert, src_frag)", "")},

        {"__integral",             ksf_wrap(T_integral_, T_NAME ".__integral(self)", "Converts to an integer (the OpenGL handle)")},

        {"use",                    ksf_wrap(T_use_, T_NAME ".use(self)", "Set this shader to the current OpenGL shader")},
        {"uniform",                ksf_wrap(T_uniform_, T_NAME ".uniform(self, name, val)", "Set the uniform 'name' to a given value")},
        {"uniformloc",             ksf_wrap(T_uniformloc_, T_NAME ".uniformloc(self, name)", "Return the uniform location")},

    ));
}

