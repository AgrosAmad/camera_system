// STL
#include <iostream>

// XE
#include <STLUtils.h>
#include <Material.h>
#include <Camera.h>
#include <Shader.h>
#include <Light.h>

namespace X3
{
    Shader::~Shader()
    {
        Delete();
    }

    Shader::Shader(const std::vector<std::string>& shadersFn) : mShadersFn(shadersFn)
    {

        // Create and build
        mProgramID = glCreateProgram();
        BuildShader();
    }

    bool Shader::Link()
    {
        if (mLinked) {
            return true;
        }

        // Try to link program and get the link status
        glLinkProgram(mProgramID);
        GLint linkStatus;
        glGetProgramiv(mProgramID, GL_LINK_STATUS, &linkStatus);
        mLinked = linkStatus == GL_TRUE;

        if (!mLinked)
        {
            std::cerr << "Error! Shader program wasn't linked!";

            // Get length of the error log first
            GLint logLength;
            glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH, &logLength);

            // If there is some log, then retrieve it and output extra information
            if (logLength > 0)
            {
                GLchar* logMessage = new GLchar[logLength];
                glGetProgramInfoLog(mProgramID, logLength, nullptr, logMessage);
                std::cerr << " The linker returned: " << std::endl << std::endl << logMessage;
                delete[] logMessage;
            }

            std::cerr << std::endl;
            return false;
        }

        return mLinked;
    }

    void Shader::Use() const
    {
        if (mLinked) glUseProgram(mProgramID);
        else printf("Trying to use a shader that is not linked!\n");
    }

    void Shader::Unuse() const
    {
        if (mLinked) glUseProgram(0);
        else printf("Trying to unuse a shader that is not linked!\n");
    }

    void Shader::Delete()
    {
        if (mProgramID == 0) return;

        glDeleteProgram(mProgramID);
        mUniforms.clear();
        mAttribs.clear();
        mLinked = false;
    }

    void Shader::Render()
    {
        // Activate shader
        this->Use();

        // Render all materials using this shader
        for (int i = 0; i < mMaterials.size(); i++)
        {
            mMaterials[i].lock()->Render(shared_from_this());
        }

        // Desactivate shader
        this->Unuse();
    }

    void Shader::Update()
    {
        utils::EraseExpiredPointers(mMaterials);
    }

    int Shader::GetAttribLocation(std::string name) const
    {
        return mAttribs.at(name);
    }

    bool Shader::HasAttrib(const std::string name) const
    {
        return mAttribs.find(name) != mAttribs.end();
    }

    GLuint Shader::GetID() const
    {
        return mProgramID;
    }

    Uniform& Shader::operator[](const std::string& varName)
    {
        if (mUniforms.count(varName) == 0)
        {
            mUniforms[varName] = Uniform(varName, this);
        }

        return mUniforms[varName];
    }

    Uniform& Shader::Unif(const std::string& varName)
    {
        if (mUniforms.count(varName) == 0)
        {
            mUniforms[varName] = Uniform(varName, this);
        }

        return mUniforms[varName];
    }

    // Model and normal matrix setting is pretty common, that's why this convenience function

    void Shader::SetModelMat(const glm::mat4& modelMatrix)
    {
        //(*this)[ShaderConstants::modelMatrix()] = modelMatrix;
        //(*this)[ShaderConstants::normalMatrix()] = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    }

    GLuint Shader::GetUniformBlockIndex(const std::string& uniformBlockName) const
    {
        if (!mLinked)
        {
            std::cerr << "Cannot get index of uniform block " << uniformBlockName << " when program has not been linked!" << std::endl;
            return GL_INVALID_INDEX;
        }

        const auto result = glGetUniformBlockIndex(mProgramID, uniformBlockName.c_str());
        if (result == GL_INVALID_INDEX) {
            std::cerr << "Could not get index of uniform block " << uniformBlockName << ", check if such uniform block really exists!" << std::endl;
        }

        return result;
    }

    void Shader::BindUniformBlockToBindingPoint(const std::string& uniformBlockName, const GLuint bindingPoint) const
    {
        const auto blockIndex = GetUniformBlockIndex(uniformBlockName);
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(mProgramID, blockIndex, bindingPoint);
        }
    }

    void Shader::SetTransformFeedbackRecordedVariables(const std::vector<std::string>& recordedVariablesNames, const GLenum bufferMode) const
    {
        std::vector<const char*> recordedVariablesNamesPtrs;
        for (const auto& recordedVariableName : recordedVariablesNames) {
            recordedVariablesNamesPtrs.push_back(recordedVariableName.c_str());
        }

        glTransformFeedbackVaryings(mProgramID, static_cast<GLsizei>(recordedVariablesNamesPtrs.size()), recordedVariablesNamesPtrs.data(), bufferMode);
    }

    bool Shader::GetLinesFromFile(const std::string& fileName, std::vector<std::string>& result, std::set<std::string>& filesIncludedAlready, bool isReadingIncludedFile) const
    {
        std::ifstream file(fileName);
        if (!file.good())
        {
            std::cout << "File " << fileName << " not found! (Have you set the working directory of the application to $(SolutionDir)bin/?)" << std::endl;
            return false;
        }

        std::string startDirectory;
        auto slashCharacter = '/';
        auto normFileName = utils::normalizeSlashes(fileName, slashCharacter);

        size_t slashIndex = -1;
        for (auto i = static_cast<int>(fileName.size()) - 1; i >= 0; i--)
        {
            if (fileName[i] == slashCharacter)
            {
                slashIndex = i;
                slashCharacter = fileName[i];
                break;
            }
        }

        startDirectory = fileName.substr(0, slashIndex + 1);

        // Get all lines from a file
        std::string line;
        auto isInsideIncludePart = false;

        while (std::getline(file, line))
        {
            line += "\n"; // getline does not keep newline character
            std::stringstream ss(line);
            std::string firstToken;
            ss >> firstToken;
            if (firstToken == "#include")
            {
                std::string includeFileName;
                ss >> includeFileName;
                if (includeFileName.size() > 0 && includeFileName[0] == '\"' && includeFileName[includeFileName.size() - 1] == '\"')
                {
                    includeFileName = utils::normalizeSlashes(includeFileName.substr(1, includeFileName.size() - 2), slashCharacter);
                    std::string directory = startDirectory;
                    std::vector<std::string> subPaths = utils::split(includeFileName, slashCharacter);
                    std::string sFinalFileName = "";
                    for (const std::string& subPath : subPaths)
                    {
                        if (subPath == "..")
                            directory = utils::upOneDirectory(directory, slashCharacter);
                        else
                        {
                            if (sFinalFileName.size() > 0)
                                sFinalFileName += slashCharacter;
                            sFinalFileName += subPath;
                        }
                    }

                    const auto combinedIncludeFilePath = directory + sFinalFileName;
                    if (filesIncludedAlready.find(combinedIncludeFilePath) == filesIncludedAlready.end())
                    {
                        std::string headOS = "";
                        if (CURRENT_OS == 1) headOS = "/";
                        filesIncludedAlready.insert(combinedIncludeFilePath);
                        GetLinesFromFile(headOS + directory + sFinalFileName, result, filesIncludedAlready, false);
                    }
                }
            }
            else if (firstToken == "#include_part")
                isInsideIncludePart = true;
            else if (firstToken == "#definition_part")
                isInsideIncludePart = false;
            else if (!isReadingIncludedFile || (isReadingIncludedFile && isInsideIncludePart))
                result.push_back(line);
        }

        file.close();
        return true;
    }

    void Shader::BuildShader()
    {
        // Compile shader programs
        for (const auto& fn : mShadersFn) CompileShaderProgram(fn);

        // Attach all programs and links shader
        AttachShaders();

        // Fill vertex attributes
        FindActiveAttribs();

    }

    void Shader::FindActiveAttribs()
    {
        // https://stackoverflow.com/questions/440144/in-opengl-is-there-a-way-to-get-a-list-of-all-uniforms-attribs-used-by-a-shade
        GLint i;
        GLint count;
        GLint size; // size of the variable
        GLenum type; // type of the variable (float, vec3 or mat4, etc)
        const GLsizei bufSize = 32; // maximum name length
        GLchar name[bufSize]; // variable name in GLSL
        GLsizei length; // name length

        glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTES, &count);

        mAttribs.clear();
        for (i = 0; i < count; i++)
        {
            glGetActiveAttrib(mProgramID, (GLuint)i, bufSize, &length, &size, &type, name);
            mAttribs[name] = glGetAttribLocation(mProgramID, name);
        }
    }

    void Shader::AttachShaders()
    {
        for (const auto& shaderObj : mShaderPrograms)
        {
            if (shaderObj.second != 0) glAttachShader(mProgramID, shaderObj.second);
        }

        // After linking, shaders can be detached and erased
        if (Link())
        {
            DetachShaders();
            DeleteShaders();
        }
    }

    void Shader::DetachShaders()
    {
        for (const auto& shaderObj : mShaderPrograms)
        {
            if (shaderObj.second != 0) glDetachShader(mProgramID, shaderObj.second);
        }
    }

    void Shader::DeleteShaders()
    {
        for (const auto& shaderObj : mShaderPrograms)
        {
            glDeleteShader(shaderObj.second);
        }

        mShaderPrograms.clear();
    }

    void Shader::CompileShaderProgram(const std::string& fn)
    {


        // Load file
        std::vector<std::string> fileLines;
        std::set<std::string> filesIncludedAlready;

        if (!GetLinesFromFile(fn, fileLines, filesIncludedAlready)) return;

        std::vector<const char*> programSource;
        for (const auto& line : fileLines) programSource.push_back(line.c_str());

        // Create and compile shader
        GLuint shaderID = 0;
        GLenum shaderType = ShaderTypeFromFn(fn);
        shaderID = glCreateShader(shaderType);
        glShaderSource(shaderID, static_cast<GLsizei>(fileLines.size()), programSource.data(), nullptr);
        glCompileShader(shaderID);

        // Get and check the compilation status
        GLint compilationStatus;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus);
        if (compilationStatus == GL_FALSE)
        {
            std::cerr << "Error! Shader file " << fn << " wasn't compiled!";

            // Get length of the error log first
            GLint logLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

            // If there is some log, then retrieve it and output extra information
            if (logLength > 0)
            {
                GLchar* logMessage = new GLchar[logLength];
                glGetShaderInfoLog(shaderID, logLength, nullptr, logMessage);
                std::cerr << " The compiler returned: " << std::endl << std::endl << logMessage;
                delete[] logMessage;
            }

            std::cerr << std::endl;
        }
        else
        {
            mShaderPrograms.insert(std::pair<GLenum, GLuint>(shaderType, shaderID));
        }
    }

    GLenum Shader::ShaderTypeFromFn(const std::string& fn)
    {
        std::string ext = fn.substr(fn.find_last_of('.') + 1, fn.size());
        if (ext == "vert") return GL_VERTEX_SHADER;
        else if (ext == "frag") return GL_FRAGMENT_SHADER;
        else if (ext == "geom") return GL_GEOMETRY_SHADER;
        else return GL_VERTEX_SHADER;
    }
}