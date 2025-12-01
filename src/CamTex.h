#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

// X3
#include <X3.h>

// for PhxGrabber::Frame
#include "PhxGrabber.h"

class CamTex {
public:
    GLuint id = 0;
    int w = 0, h = 0;

    void init(int width, int height) {
        w = width; h = height;
        if (!id) glGenTextures(1, &id);

        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Swizzle RED→RGB so single-channel reads as grayscale in RGBA shaders/ImGui
        GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    }

    // autoRelease=true: call f.release() after upload so the SDK can reuse the buffer.
    bool update(const PhxGrabber::Frame& f, bool autoRelease = true) {
        if (!f.data || f.width == 0 || f.height == 0) return false;

        const int width  = static_cast<int>(f.width);
        const int height = static_cast<int>(f.height);

        if (width != w || height != h) {
            init(width, height);
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // In our PhxGrabber wrapper, buffers are Mono8 and tightly packed: bytes == w*h.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        GL_RED, GL_UNSIGNED_BYTE, f.data);

        if (autoRelease && f.release) f.release();
        return true;
    }

    // Raw upload (no PhxGrabber::Frame)
    void updateRaw(const uint8_t* src, int width, int height, int srcStrideBytes) {
        if (width != w || height != h) {
            init(width, height);
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        const int bytesPerPixel = 1;                 // Bayer8 / Mono8
        const int rowBytes      = width * bytesPerPixel;

        if (srcStrideBytes == rowBytes) {
            // Tightly packed → direct upload
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            width, height,
                            GL_RED, GL_UNSIGNED_BYTE,
                            src);
        } else {
            // Repack rows: copy only used pixels from each stride
            std::vector<uint8_t> tight(size_t(rowBytes) * size_t(height));
            for (int y = 0; y < height; ++y) {
                std::memcpy(&tight[size_t(y) * size_t(rowBytes)],
                            src + size_t(y) * size_t(srcStrideBytes),
                            size_t(rowBytes));
            }

            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            width, height,
                            GL_RED, GL_UNSIGNED_BYTE,
                            tight.data());
        }
    }


    void destroy() {
        if (id) { glDeleteTextures(1, &id); id = 0; }
        w = h = 0;
    }
};
