/* util.c - various utilities
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>


bool ksgl_check() {
    int rc = glGetError();
    if (!rc) {
        return true;
    }

    KS_THROW(kst_Error, "OpenGL error %i", (int)rc);

    return false;
}


bool ksgl_getcolor(int nargs, kso* args, ks_cfloat* out) {
    /* Default alpha to 1.0 */
    out[3] = 1.0;

    out[0] = out[1] = out[2] = 0.0;

    if (nargs > 4) {
        KS_THROW(kst_Error, "Expected at most 4 arguments to create color from");
        return false;
    }

    int i;
    for (i = 0; i < nargs; ++i) {
        if (!kso_get_cf(args[i], &out[i])) {
            return false;
        }
    }

    return true;
}



