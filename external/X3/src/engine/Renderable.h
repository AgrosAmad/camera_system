#pragma once

// STL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

// GL
#include <glad/glad.h>
#include <imgui.h>

// XE
#include <Texture2D.h>
#include <Material.h>
#include <Shaders.h>
#include <Vao.h>
#include <Vbo.h>

// By default up vector is in 'y' axis and forward on '-z'
#define VEC_UP 0,1,0
#define VEC_FWD 0,0,-1

// Math
#include <XMath.cuh>

namespace X3
{

    // Common objects
    enum class Objects { Cube, Mesh, Light, Quad, Camera };

    class Renderable : public std::enable_shared_from_this<Renderable>
    {
    public:

        // Constructor
        Renderable();
        virtual ~Renderable() = default;

        // Common methods
        virtual void Select(glm::ivec4 idx) {};
        virtual void Deselect() {};

        virtual void Update() {};
        virtual void ReDraw() {};

        virtual void RenderUi() {};
        virtual void Render() {};

        virtual GLuint GetVbo() { return 0; }

        // Handle materials
        void SetPass(const RenderPass& pass, const std::string& matName);
        std::string MatPass(const RenderPass& pass);
        bool HasPass(const RenderPass& pass);

        // Tree structure
        std::vector<std::weak_ptr<Renderable>>& Children() { return mChildren; }
        std::weak_ptr<Renderable>& Parent() { return mParent; }
        void Parent(const std::shared_ptr<Renderable>& parent) { mParent = parent; }
        void Children(const std::shared_ptr<Renderable>& child) { mChildren.push_back(child); }

        // Misc
        void Name(const std::string& name) { mName = name; }
        std::string Name() { return mName; }

        void BoundingBox(const XM::AABB& box) { mBoundingBox = box; }
        XM::AABB& BoundingBox() { return mBoundingBox; }

        void Visible(const bool& v) { mVisible = v; }
        bool Visible() { return mVisible; }

        // Geometry Transformations

        ///////////////////////////
        // Model matrix
        virtual glm::mat4 MatM();

        // Position
        virtual void Translate(const glm::vec3& t) { mPosition += t; }
        virtual void Position(const glm::vec3& p) { mPosition = p; };
        virtual glm::vec3 Position() { return mPosition; }

        // Scaling
        virtual void Scale(const float& s) { mScale = glm::vec3(s); }
        virtual void Scale(const glm::vec3& s) { mScale = s; }
        virtual glm::vec3 Scale() { return mScale; }

        // Rotations
        virtual void Rotate(const glm::vec3& pyr) { mPYR += pyr; }
        virtual void PYR(const glm::vec3& pyr) { mPYR = pyr; }
        virtual void Pitch(const float& p) { mPYR.x = p; }
        virtual void Roll(const float& r) { mPYR.z = r; }
        virtual void Yaw(const float& y) { mPYR.y = y; }
        virtual glm::vec3 PYR() { return mPYR; }

        // Orientation
        virtual void Orientation(const glm::quat& q) { mPYR = glm::vec3(glm::pitch(q), glm::yaw(q), glm::roll(q)); }
        virtual void ViewDir(const glm::vec3& dir) { mPYR.x = asin(dir.y); mPYR.y = atan2(-dir.x, -dir.z); }
        virtual glm::quat Orientation() { return glm::quat(glm::vec3(mPYR.x, mPYR.y, mPYR.z)); }
        virtual glm::vec3 ViewVector() { return Orientation() * glm::vec3(VEC_FWD); }
        virtual glm::vec3 UpVector() { return Orientation() * glm::vec3(VEC_UP); }
        ///////////////////////////


    protected:

        // Misc data
        std::map<RenderPass, std::string> mPasses; // name of material associated to pass
        GLenum mPrimitive; // rendering primitive mode
        XM::AABB mBoundingBox; // Axis-aligned bounding box
        std::string mName; // renderable name
        bool mVisible = true; // visibility flag
        int mDataSize; // total size of data arrays (vbos)

        // Tree structure
        std::vector<std::weak_ptr<Renderable>> mChildren;
        std::weak_ptr<Renderable> mParent;

        // Position and orientation
        glm::vec3 mPosition;
        glm::vec3 mCenter;
        glm::vec3 mScale;
        glm::vec3 mPYR;

        // Helpers
        virtual void BuildBoundingBox() {};
    };
}