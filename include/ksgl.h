/* ksgl.h - kscript bindings for OpenGL
 *
 * 
 * SEE: https://www.opengl.org/
 * 
 * @author: Cade Brown <cade@kscript.org>
 */
#ifndef KSGL_H__
#define KSGL_H__


/* Default config */
#ifndef KSGL_GLFW
#define KSGL_GLFW
#endif


/* Module name */
#define M_NAME "gl"

/* kscript API */
#include <ks/ks.h>




/* OpenGL API (3.3), generated via gl3w */
#include <gl3w.h>




/** Functions **/

/* Checks the last error, and if there has been an error, throws an exception and returns false
 */
bool ksgl_check();


/* Convert arguments to a color (RGBA)
 * 'out' should store '4' values
 */
bool ksgl_getcolor(int nargs, kso* args, ks_cfloat* out);




#ifdef KSGL_GLFW

/** gl.glfw submodule **/

/* GLFW (Window Library) */
#include <GLFW/glfw3.h>



/* gl.glfw.Monitor - GLFW monitor wrapper
 *
 */
typedef struct ksgl_glfw_monitor_s {
    KSO_BASE

    /* Wrapped value */
    GLFWmonitor* val;

}* ksgl_glfw_monitor;


/* gl.glfw.Window - GLFW window wrapper
 *
 */
typedef struct ksgl_glfw_window_s {
    KSO_BASE

    /* Wrapped value */
    GLFWwindow* val;

}* ksgl_glfw_window;





#endif




ks_type
    ksgl_glfwt_monitor,
    ksgl_glfwt_window
;


/** Internals **/

/* gl.glfw module */
ks_module _ksgl_glfw();

void _ksgl_glfw_monitor();
void _ksgl_glfw_window();


#endif /* KSGL_H__ */


