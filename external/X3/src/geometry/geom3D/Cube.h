#pragma once

// GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// X3
#include <CameraFPS.h>
#include <Light.h>
#include <Mesh.h>

namespace X3
{
    namespace geom
    {
        class Cube: public Mesh
        {
        public:
            Cube(const std::string& name, GLenum primitive = GL_TRIANGLES);
        private:
        };
    } // namespace geometry
} // namespace X3