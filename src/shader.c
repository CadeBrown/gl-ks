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

    /* Attach all prts */
    int i;
    for (i = 0; i < nshaders; ++i) {
        glAttachShader(prog, shaders[i]);
    }

    glLinkProgram(prog);

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

static KS_TFUNC(T, use) {
    ksgl_shader self;
    KS_ARGS("self:*", &self, ksglt_shader);

    glUseProgram(self->val);

    return KSO_NONE;
}

/* Export */

ks_type ksglt_shader;

void _ksgl_shader() {
    ksglt_shader = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_shader_s), -1, "OpenGL shader", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, src_vert, src_frag)", "")},

        {"use",                    ksf_wrap(T_use_, T_NAME ".use(self)", "Set this shader to the current OpenGL shader")},

    ));
}

