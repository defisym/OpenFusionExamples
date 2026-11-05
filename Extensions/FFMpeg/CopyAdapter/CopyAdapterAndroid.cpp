#include "CopyAdapterAndroid.h"
#ifndef _WIN32
#include <cstring>
namespace {
bool CopyPixels(const AndroidCopyTarget& target, const unsigned char* data) {
    if (!data || !target.pixels || target.width <= 0 || target.height <= 0
        || target.stride < target.width * 4
        || target.capacity / 4 / target.width < static_cast<size_t>(target.height)) return false;
    for (int row = 0; row < target.height; ++row) {
        auto dst = target.pixels + static_cast<size_t>(row) * target.width * 4;
        std::memcpy(dst, data + static_cast<ptrdiff_t>(row) * target.stride, target.width * 4);
        if (target.premultiply)
            for (int x = 0; x < target.width; ++x)
                for (int channel = 0; channel < 3; ++channel)
                    dst[x*4+channel] = (dst[x*4+channel] * dst[x*4+3] + 127) / 255;
    }
    return true;
}
}
// Android equivalent of CopyAdapterD3D11: external decoder image -> Fusion 2D texture.
#ifdef __ANDROID__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
namespace {
struct Attribute {
    GLint enabled, size, stride, type, normalized, buffer;
    void* pointer;
    Attribute() {
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
        glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
        glGetVertexAttribPointerv(0, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
    }
    void restore() const {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glVertexAttribPointer(0, size, type, normalized, stride, pointer);
        if (enabled) glEnableVertexAttribArray(0); else glDisableVertexAttribArray(0);
    }
};
struct State {
    GLint program, framebuffer, arrayBuffer, active, texture, external, viewport[4];
    GLboolean blend, depth, scissor, cull, stencil, color[4];
    Attribute attribute;
    State() {
        glGetIntegerv(GL_CURRENT_PROGRAM, &program); glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBuffer); glGetIntegerv(GL_ACTIVE_TEXTURE, &active);
        glActiveTexture(GL_TEXTURE0);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture); glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &external);
        glGetIntegerv(GL_VIEWPORT, viewport); glGetBooleanv(GL_COLOR_WRITEMASK, color);
        blend = glIsEnabled(GL_BLEND); depth = glIsEnabled(GL_DEPTH_TEST); scissor = glIsEnabled(GL_SCISSOR_TEST);
        cull = glIsEnabled(GL_CULL_FACE); stencil = glIsEnabled(GL_STENCIL_TEST);
    }
    static void enable(GLenum cap, GLboolean value) { if (value) glEnable(cap); else glDisable(cap); }
    ~State() {
        attribute.restore(); glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glUseProgram(program); glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        glColorMask(color[0],color[1],color[2],color[3]);
        enable(GL_BLEND, blend); enable(GL_DEPTH_TEST, depth); enable(GL_SCISSOR_TEST, scissor);
        enable(GL_CULL_FACE, cull); enable(GL_STENCIL_TEST, stencil);
        glBindTexture(GL_TEXTURE_2D, texture); glBindTexture(GL_TEXTURE_EXTERNAL_OES, external); glActiveTexture(active);
    }
};
}
bool CopyAdapterAndroid::CopyTexture(const AndroidCopyTarget& target, const unsigned char* data) {
    if (type == AV_HWDEVICE_TYPE_NONE) return CopyPixels(target, data);
    const auto& transform = target.transform;
    const auto program = target.program, external = target.external;
    const auto framebuffer = target.framebuffer, vbo = target.vbo;
    const auto width = target.width, height = target.height;
    State state;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); glViewport(0, 0, width, height);
    glDisable(GL_BLEND); glDisable(GL_DEPTH_TEST); glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE); glDisable(GL_STENCIL_TEST); glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glUseProgram(program); glBindTexture(GL_TEXTURE_EXTERNAL_OES, external);
    glUniform1i(glGetUniformLocation(program, "image"), 0);
    glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, transform);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    return true;
}
#endif

#ifndef __ANDROID__
bool CopyAdapterAndroid::CopyTexture(const AndroidCopyTarget& target, const unsigned char* data) {
    return type == AV_HWDEVICE_TYPE_NONE && CopyPixels(target, data);
}
#endif
#endif
