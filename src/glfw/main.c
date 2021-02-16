/* glfw/main.c - GLFW submodule
 *
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#ifdef KSGL_GLFW

/* Module Functions */

static KS_TFUNC(M, poll) {
    KS_ARGS("");

    glfwPollEvents();

    return KSO_NONE;
}


/* Export */

ks_module _ksgl_glfw() {

    /* Initialize GLFW */
    if (!glfwInit()) {
        KS_THROW(kst_Error, "Failed to initialize GLFW");
        return NULL;
    }

    //glfwSetErrorCallback(glfw_errcb);

    /* Request OpenGL 3.3 core */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   
    /* Reset time */
    glfwSetTime(0.0);


    _ksgl_glfw_monitor();
    _ksgl_glfw_window();

    ks_module res = ks_module_new("gl.glfw", "", "GLFW bindings", KS_IKV(
        /* Types */
        {"Monitor", (kso)ksgl_glfwt_monitor},
        {"Window", (kso)ksgl_glfwt_window},


        /* Functions */
        {"poll",                   ksf_wrap(M_poll_, M_NAME ".poll()", "Polls all GLFW events")},


    ));
}

#endif
