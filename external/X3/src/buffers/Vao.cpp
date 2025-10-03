#include <Vao.h>

namespace X3
{
    Vao::Vao()
    {
        Generate();
    }

    Vao::~Vao()
    {
        if (mID != 0) glDeleteVertexArrays(1, &mID);
    }

    Vao::Vao(const std::vector<VertexAttrib>& attributes) : Vao()
    {
        //  Add the attributes and update pointers
        AddAttribs(attributes);
        UpdateVertAttribPointers();
    }

    void Vao::SetVertexAttribPointer(const GLuint& index, const VertexAttrib& attrib)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, attrib.mSize, attrib.Type, attrib.mNormalized, attrib.mStride, attrib.mPointer);
    }

    void Vao::Generate()
    {
        glGenVertexArrays(1, &mID);
    }

    void Vao::Bind()
    {
        glBindVertexArray(mID);
    }

    void Vao::Unbind()
    {
        glBindVertexArray(0);
    }

    void Vao::AddAttribs(const std::vector<VertexAttrib>& attributes, bool deleteOld)
    {
        // Delete old attributes
        if (deleteOld) DeleteAttribs();

        // Insert new ones
        mAttributes.insert(mAttributes.begin(), attributes.begin(), attributes.end());
    }

    void Vao::UpdateVertAttribPointers(const std::shared_ptr<Shader>& shader)
    {
        // No attributes, do nothing
        if (mAttributes.size() == 0) return;

        Bind();

        for (const auto& attrib : mAttributes)
        {
            // Already deleted Vbo
            if (attrib.mVbo == nullptr) continue;

            attrib.mVbo->Bind();

            if (attrib.mName.size() > 0)
            {
                // named attrib
                if (shader && shader->HasAttrib(attrib.mName))
                {
                    SetVertexAttribPointer(shader->GetAttribLocation(attrib.mName), attrib);
                }
            }
            else
            {
                // index given
                SetVertexAttribPointer(attrib.mID, attrib);
            }

            attrib.mVbo->Unbind();
        }

        Unbind();
    }

    void Vao::BindEbo(const std::shared_ptr<GLBufferBase>& ebo)
    {

        // Binding Ebo within Vao context to associate 
        Bind();
        ebo->Bind();
        Unbind();
        ebo->Unbind();
    }

    void Vao::DeleteAttribs()
    {
        mAttributes.clear();
    }
}