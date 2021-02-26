/* util/main.c - util submodule
 *
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

/** Internal utilities type **/



/* Export */

ks_module _ksgl_util() {

    ks_module res = ks_module_new("gl.util", "", "Utilities", KS_IKV(
        /* Types */

        /* Functions */


    ));
}
