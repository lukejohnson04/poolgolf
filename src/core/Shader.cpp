
#ifdef RELEASE
#define SHADER_PATH "shaders/"
#else
#define SHADER_PATH "../src/shaders/"
#endif


std::string readShaderFile(const std::string &shaderPath) {
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    // Open file
    shaderFile.open(shaderPath);
    if (!shaderFile.is_open()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: Cannot open file " << shaderPath << std::endl;
        return "";
    }

    // Read file's buffer contents into streams
    shaderStream << shaderFile.rdbuf();
    shaderFile.close(); // Close file handlers

    // Convert stream into string
    return shaderStream.str();
}

GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = readShaderFile(vertexPath);
    std::string fragmentCode = readShaderFile(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    GLuint vertexShader = compileShader(vShaderCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


struct Shader {
    GLuint id;
    
    inline GLuint Uniform(const GLchar *name) {
        return glGetUniformLocation(id, name);
    }

    void Uniform1i(const GLchar *name, GLuint tex) {
        glBindTexture(GL_TEXTURE_2D,tex);
        glUniform1i(Uniform(name),0);
    }

    void UniformM4fv(const GLchar *name, glm::mat4 value) {
        glUniformMatrix4fv(Uniform(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void UniformM4fv(const GLchar *name, mat4 value) {
        glUniformMatrix4fv(Uniform(name), 1, GL_FALSE, (GLfloat*)value.vals);
    }

    void UniformColor(const GLchar *name, Color color) {
        glUniform4f(Uniform(name),((float)color.r/255.f),((float)color.g/255.f),((float)color.b/255.f),((float)color.a/255.f));
    }

    void Uniform4f(const GLchar *name, float a, float b, float c, float d) {
        glUniform4f(Uniform(name),a,b,c,d);
    }

    void Uniform2f(const GLchar *name, float a, float b) {
        glUniform2f(Uniform(name),a,b);
    }

    void Uniform2f(const GLchar *name, v2 a) {
        glUniform2f(Uniform(name),a.x,a.y);
    }

    void Uniform3f(const GLchar *name, v3 a) {
        glUniform3f(Uniform(name),a.x,a.y,a.z);
    }

    void Uniform3f(const GLchar *name, float a, float b, float c) {
        glUniform3f(Uniform(name),a, b, c);
    }

    void Uniform3f(const GLchar *name, glm::vec3 a) {
        glUniform3f(Uniform(name), a.x, a.y, a.z);
    }

};

void UseShader(Shader *shader) {
    glUseProgram(shader->id);
}

Shader CreateShader(const std::string &vertexPath, const std::string &fragmentPath) {
    Shader shader;
    shader.id = createShaderProgram(SHADER_PATH + vertexPath,SHADER_PATH + fragmentPath);
    return shader;
}
