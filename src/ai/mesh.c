/* ai/mesh.c - gl.ai.Mesh type
 *
 * @author: Cade Brown <cade@kscript.org>
 */
#include <ksgl.h>

#define T_NAME M_NAME ".ai.Mesh"


/* Internals */

/* C-API */

/* Type Functions */

static KS_TFUNC(T, free) {
    ksgl_ai_mesh self;
    KS_ARGS("self:*", &self, ksgl_ait_mesh);

    KSO_DEL(self);
    return KSO_NONE;
}

static KS_TFUNC(T, init) {
    ksgl_ai_mesh self;
    KS_ARGS("self:*", &self, ksgl_ait_mesh);
    
    KS_THROW(kst_TypeError, "'%T' cannot be created directly", self);
    return NULL;
}

static KS_TFUNC(T, str) {
    ksgl_ai_mesh self;
    KS_ARGS("self:*", &self, ksgl_ait_mesh);

    return (kso)ks_fmt("<%T val=%p, name='%.*s'>", self, self->val, (int)self->val->mName.length, self->val->mName.data);
}

static KS_TFUNC(T, getattr) {
    ksgl_ai_mesh self;
    ks_str attr;
    KS_ARGS("self:* attr:*", &self, ksgl_ait_mesh, &attr, kst_str);

    if (ks_str_eq_c(attr, "name", 4)) {
        return (kso)ks_str_new(self->val->mName.length, self->val->mName.data);
    } else if (ks_str_eq_c(attr, "nvert", 5)) {
        return (kso)ks_int_new(self->val->mNumVertices);
    } else if (ks_str_eq_c(attr, "ntri", 4)) {
        return (kso)ks_int_new(self->val->mNumFaces);

    } else if (ks_str_eq_c(attr, "pos", 3)) {
        return (kso)nx_array_newc(nxt_array, self->val->mVertices, nxd_F, 2, (ks_size_t[]){ self->val->mNumVertices, 3 }, NULL);
    } else if (ks_str_eq_c(attr, "normals", 7)) {
        return (kso)nx_array_newc(nxt_array, self->val->mNormals, nxd_F, 2, (ks_size_t[]){ self->val->mNumVertices, 3 }, NULL);
    } else if (ks_str_eq_c(attr, "uv", 2)) {
        return (kso)nx_array_newc(nxt_array, self->val->mTextureCoords, nxd_F, 2, (ks_size_t[]){ self->val->mNumVertices, 2 }, NULL);

    } else if (ks_str_eq_c(attr, "idx", 3)) {
        nx_u32* rdata = ks_smalloc(sizeof(*rdata) * 3 * self->val->mNumFaces);
        int i;
        for (i = 0; i < self->val->mNumFaces; ++i) {
            if (self->val->mFaces[i].mNumIndices != 3) {
                KS_THROW(kst_SizeError, "Faces were not triangulated! (This is an internal error)");
                ks_free(rdata);
                return NULL;
            }
            rdata[3 * i + 0] = self->val->mFaces[i].mIndices[0];
            rdata[3 * i + 1] = self->val->mFaces[i].mIndices[1];
            rdata[3 * i + 2] = self->val->mFaces[i].mIndices[2];
        }

        nx_array res = nx_array_newc(nxt_array, rdata, nxd_u32, 2, (ks_size_t[]){ self->val->mNumFaces, 3 }, NULL);
        ks_free(rdata);
        return (kso)res;

    }


    KS_THROW_ATTR(self, attr);
    return NULL;
}


/* Export */

ks_type ksgl_ait_mesh;

void _ksgl_ai_mesh() {
    ksgl_ait_mesh = ks_type_new(T_NAME, kst_object, sizeof(struct ksgl_ai_mesh_s), -1, "Assimp mesh", KS_IKV(
        {"__free",                 ksf_wrap(T_free_, T_NAME ".__free(self)", "")},
        {"__init",                 ksf_wrap(T_init_, T_NAME ".__init(self)", "")},
        {"__str",                  ksf_wrap(T_str_, T_NAME ".__str(self)", "")},
        {"__repr",                 ksf_wrap(T_str_, T_NAME ".__repr(self)", "")},
        {"__getattr",              ksf_wrap(T_getattr_, T_NAME ".__getattr(self, attr)", "")},
    ));
}
