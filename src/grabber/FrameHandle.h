#pragma once
#include <iostream>
#include <string>
#include "phx_core.h"

namespace PhxWrap 
{
    enum class PixelFormat 
    {
        Mono8, Mono16,
        Bayer8, Bayer10p, Bayer12p, Bayer16,
        RGB8, RGBA8, YUV422Packed,
        Unknown
    };

    struct FrameView 
    {
        const uint8_t* data = nullptr;  // driver-owned; valid until release
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t strideBytes = 0;       // bytes per line
        PixelFormat fmt = PixelFormat::Unknown;
        uint32_t srcDepthBits = 0;      // 8/10/12/16
        etParamValue srcCol = static_cast<etParamValue>(0); // PHX_SRC_COL_*
    };

    // RAII handle:
    class FrameHandle 
    {
    public:
        FrameHandle() = default;
        FrameHandle(CPhxCore* core, uint8_t* p, const FrameView& v) : core_(core), ptr_(p), view_(v) {}
        FrameHandle(FrameHandle&& o) noexcept { *this = std::move(o); }
        FrameHandle& operator=(FrameHandle&& o) noexcept {
            if (this != &o) { release(); core_ = o.core_; ptr_ = o.ptr_; view_ = o.view_; o.core_ = nullptr; o.ptr_ = nullptr; }
            return *this;
        }
        ~FrameHandle() { release(); }

        bool valid() const { return core_ && ptr_; }
        const FrameView& view() const { return view_; }
        void release() {
            if (core_ && ptr_) { core_->BufferRelease(); ptr_ = nullptr; }
        }

    private:
        CPhxCore* core_ = nullptr;
        uint8_t*  ptr_  = nullptr;
        FrameView view_{};
    };
} // namespace phxwrap