/* glfw/window.c - gl.glfw.Window type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#ifdef KSGL_GLFW

#define T_NAME M_NAME ".glfw.Window"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_glfw_window self;
    KS_ARGS("self:*", &self, ksgl_glfwt_window);

    if (self->val) {
        glfwDestroyWindow(self->val);
    }

    KSO_DEL(self);
    return KSO_NONE;
}

static KS_TFUNC(T, init) {
    ksgl_glfw_window self;
    ks_str name;
    ks_tuple size;
    ksgl_glfw_monitor monitor = NULL;
    KS_ARGS("self:* name:* size:* ?monitor", &self, ksgl_glfwt_window, &name, kst_str, &size, kst_tuple, &monitor, ksgl_glfwt_monitor);

    ks_cint w, h;
    if (size->len != 2) {
        KS_THROW(kst_Error, "Expected 'size' to be a tuple of length 2, but was of length '%i'", (int)size->len);
        return NULL;
    }

    if (!kso_get_ci(size->elems[0], &w) || !kso_get_ci(size->elems[1], &h)) {
        return NULL;
    }

    /* Create value */
    self->val = glfwCreateWindow(w, h, name->data, monitor ? monitor->val : NULL, NULL);
    if (!self->val) {
        /* Handle error.... */
    }


    /* Set current OpenGL context */
    glfwMakeContextCurrent(self->val);

    /* 1=vsync, 0=as fast as possible */
    glfwSwapInterval(1);

    return KSO_NONE;
}

static KS_TFUNC(T, bool) {
    ksgl_glfw_window self;
    KS_ARGS("self:*", &self, ksgl_glfwt_window);

    return KSO_BOOL(!glfwWindowShouldClose(self->val));
}

static KS_TFUNC(T, str) {
    ksgl_glfw_window self;
    KS_ARGS("self:*", &self, ksgl_glfwt_window);

    return (kso)ks_fmt("<%T val=%p>", self, self->val);
}

static KS_TFUNC(T, getattr) {
    ksgl_glfw_window self;
    ks_str attr;
    KS_ARGS("self:* attr:*", &self, ksgl_glfwt_window, &attr, kst_str);

    if (ks_str_eq_c(attr, "size", 4)) {
        int w, h;
        glfwGetWindowSize(self->val, &w, &h);
        return (kso)ks_tuple_newn(2, (kso[]) {
            (kso)ks_int_new(w),
            (kso)ks_int_new(h)
        });
    } else if (ks_str_eq_c(attr, "pos", 3)) {
        int x, y;
        glfwGetWindowPos(self->val, &x, &y);
        return (kso)ks_tuple_newn(2, (kso[]) {
            (kso)ks_int_new(x),
            (kso)ks_int_new(y)
        });
    }

    KS_THROW_ATTR(self, attr);
    return NULL;
}

static KS_TFUNC(T, show) {
    ksgl_glfw_window self;
    KS_ARGS("self:*", &self, ksgl_glfwt_window);

    glfwShowWindow(self->val);

    return KSO_NONE;
}

static KS_TFUNC(T, hide) {
    ksgl_glfw_window self;
    KS_ARGS("self:*", &self, ksgl_glfwt_window);

    glfwHideWindow(self->val);

    return KSO_NONE;
}
static KS_TFUNC(T, swap) {
    ksgl_glfw_window self;
    KS_ARGS("self:*", &self, ksgl_glfwt_window);

    glfwSwapBuffers(self->val);

    return KSO_NONE;
}



/* Export */

ks_type ksgl_glfwt_window;

void _ksgl_glfw_window() {
    ksgl_glfwt_window = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_glfw_window_s), -1, "GLFW window", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, name, size, monitor=none)", "")},
        {"__bool",                 ksf_wrap(T_bool_, T_NAME ".__bool(self)", "")},
        {"__str",                  ksf_wrap(T_str_, T_NAME ".__str(self)", "")},
        {"__repr",                 ksf_wrap(T_str_, T_NAME ".__repr(self)", "")},
        {"__getattr",              ksf_wrap(T_getattr_, T_NAME ".__getattr(self, attr)", "")},
    
        {"show",                   ksf_wrap(T_show_, T_NAME ".show(self)", "Shows the window, if it was hidden")},
        {"hide",                   ksf_wrap(T_hide_, T_NAME ".hide(self)", "Hides the window, if it was shown")},
        {"swap",                   ksf_wrap(T_swap_, T_NAME ".swap(self)", "Swaps the window buffers")},
    
    ));
}


#endif
