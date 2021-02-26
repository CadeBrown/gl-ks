/* ai/main.c - ai submodule
 *
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

/** Internal utilities type **/

/* Module Functions */


/* Expands 'node' into an object */
static ksgl_ai_node my_expand(C_STRUCT aiNode* node) {
    ksgl_ai_node res = KSO_NEW(ksgl_ai_node, ksgl_ait_node);
    res->val = node;
    res->sub = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < res->val->mNumChildren; ++i) {
        ksgl_ai_node s = my_expand(res->val->mChildren[i]);
        if (!s) {
            KS_DECREF(res);
        }
        ks_list_pushu(res->sub, (kso)s);
    }

    return res;
}

static KS_TFUNC(M, load) {
    ks_str src;
    KS_ARGS("src:*", &src, kst_str);

    C_STRUCT aiScene* res = aiImportFileEx(src->data, aiProcess_Triangulate | aiProcess_CalcTangentSpace, NULL);
    if (!res) {
        KS_THROW(kst_Error, "Failed to import %R: %s", src, aiGetErrorString());
        return NULL;
    }

    ksgl_ai_scene rr = KSO_NEW(ksgl_ai_scene, ksgl_ait_scene);
    KS_INCREF(src);
    rr->src = src;
    rr->val = res;

    /* Set the root node */
    rr->root = my_expand(rr->val->mRootNode);
    if (!rr->root) {
        KS_DECREF(rr);
        return NULL;
    }

    /* Create meshes */
    rr->meshes = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < rr->val->mNumMeshes; ++i) {
        ksgl_ai_mesh s = KSO_NEW(ksgl_ai_mesh, ksgl_ait_mesh);
        s->val = res->mMeshes[i];
        ks_list_pushu(rr->meshes, (kso)s);
    }

    return (kso)rr;
}


/* Export */

ks_module _ksgl_ai() {
    _ksgl_ai_scene();
    _ksgl_ai_node();
    _ksgl_ai_mesh();

    ks_module res = ks_module_new("gl.ai", "", "Assimp bindings", KS_IKV(
        /* Types */
        {"Scene", (kso)ksgl_ait_scene},
        {"Node", (kso)ksgl_ait_node},
        {"Mesh", (kso)ksgl_ait_mesh},

        /* Functions */
        {"load",                   ksf_wrap(M_load_, M_NAME ".load(src)", "Loads a model, and returns an gl.ai.Scene object")},

    ));
}
