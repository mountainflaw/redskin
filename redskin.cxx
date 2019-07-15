/*
 *   Copyright (c) 2019, Red                                                             *
 *   All rights reserved.                                                                *
 *                                                                                       *
 *   Redistribution and use in source and binary forms, with or without                  *
 *   modification, are permitted provided that the following conditions are met:         *
 *                                                                                       *
 *       * Redistributions of source code must retain the above copyright                *
 *         notice, this list of conditions and the following disclaimer.                 *
 *       * Redistributions in binary form must reproduce the above copyright             *
 *         notice, this list of conditions and the following disclaimer in the           *
 *         documentation and/or other materials provided with the distribution.          *
 *       * Neither the name of the modconv 2 developers nor the                          *
 *         names of its contributors may be used to endorse or promote products          *
 *         derived from this software without specific prior written permission.         *
 *                                                                                       *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND     *
 *   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED       *
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE              *
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY                *
 *   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES          *
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;        *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND         *
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS       *
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        *
 */

/*
 * Redskin
 * Redskin animation system animation converter
 * Author: Red
 */

#include "../src/modconv.hxx"

#include "../src/buffer.hxx"
#include "../src/f3d.hxx"

#include "redskin.hxx"

typedef struct {
    u16 vtx[REDSKIN_BONE_MAX_VTX][30];
    u16 mtx[4][4];
} RedskinBone;

static void inspect_vtx(aiNode* node, const aiScene* scene) {
    for (u16 i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        vert += mesh->mNumFaces * 3;
    }

    for (u16 i = 0; i < node->mNumChildren; i++) {
        inspect_vtx(node->mChildren[i], scene);
    }
}

void redskin_main(const std::string &file, const std::string &fileOut, const s16 scale, const u8 microcode) {
    Assimp::Importer importer;

    /* We don't use ASSIMP's built in tristripping because of the vertex buffer. */
    const aiScene* scene = importer.ReadFile(file, aiProcess_ValidateDataStructure | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_GenUVCoords);

    reset_file(fileOut + "/model.s");
    inspect_vtx(scene->mRootNode, scene);

    vBuffers = vert / microcode;

    if (vert % microcode > 0) { /* is there a trailing vbuffer? */
        vBuffers++;
    }

    info_message("Vertices " + std::to_string(vert));
    info_message("Triangles " + std::to_string(vert / 3));

    VertexBuffer vBuf[vBuffers];
    cycle_vbuffers(vBuf, BUFFER, microcode);
}
