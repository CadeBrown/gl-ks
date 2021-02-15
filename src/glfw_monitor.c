/* glfw_monitor.c - gl.glfw.Monitor type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#ifdef KSGL_GLFW

#define T_NAME M_NAME ".glfw.Monitor"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_glfw_monitor self;
    KS_ARGS("self:*", &self, ksgl_glfwt_monitor);


    KSO_DEL(self);
    return KSO_NONE;
}

static KS_TFUNC(T, init) {
    ksgl_glfw_monitor self;
    ks_cint idx = -1;
    KS_ARGS("self:* ?idx:cint", &self, ksgl_glfwt_monitor, &idx);

    if (idx < 0) {
        self->val = glfwGetPrimaryMonitor();
    } else {
        int count;
        GLFWmonitor** all = glfwGetMonitors(&count);
        if (idx < 0 || idx >= count) {
            KS_THROW(kst_IndexError, "Monitor %i does not exist!", (int)idx);
            return NULL;
        }

        self->val = all[idx];
    }

    return KSO_NONE;
}

static KS_TFUNC(T, str) {
    ksgl_glfw_monitor self;
    KS_ARGS("self:*", &self, ksgl_glfwt_monitor);


    return (kso)ks_fmt("<%T val=%p, name='%s'>", self, self->val, glfwGetMonitorName(self->val));
}

static KS_TFUNC(T, getattr) {
    ksgl_glfw_monitor self;
    ks_str attr;
    KS_ARGS("self:* attr:*", &self, ksgl_glfwt_monitor, &attr, kst_str);

    if (ks_str_eq_c(attr, "name", 4)) {
        return (kso)ks_str_new(-1, glfwGetMonitorName(self->val));
    }

    KS_THROW_ATTR(self, attr);
    return NULL;
}


/* Export */

ks_type ksgl_glfwt_monitor;

void _ksgl_glfw_monitor() {
    ksgl_glfwt_monitor = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_glfw_monitor_s), -1, "GLFW monitor", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self, idx=-1)", "")},
        {"__str",                  ksf_wrap(T_str_, T_NAME ".__str(self)", "")},
        {"__repr",                 ksf_wrap(T_str_, T_NAME ".__repr(self)", "")},
        {"__getattr",              ksf_wrap(T_getattr_, T_NAME ".__getattr(self, attr)", "")},
    ));
}


#endif
