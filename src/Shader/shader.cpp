#include "shader.h"

#include <filesystem>



//alr I don't like copying from a tutorial but genuinely how am I supposed to read the source code to this bruh.
shader::shader(const char* verShadPath, const char* fragShadPath)
{
    //vertex shader source code (graphics.h equivalent)
    std::string vertexSourceCode;
    //fragment shader source code
    std::string fragmentSourceCode;
    //value pointing to the file with the source code of the respective shader
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;

    std::cout << std::filesystem::current_path() << std::endl;




    //I lokey stole this from the tutorial I have no idea why we need it, but it's related to errors
    vertexShaderFile.exceptions( std::ifstream::failbit| std::ifstream::badbit);
    fragmentShaderFile.exceptions( std::ifstream::failbit| std::ifstream::badbit);
    try
    {
        //open the shaders from the path
        vertexShaderFile.open(verShadPath);
        fragmentShaderFile.open(fragShadPath);

        //put the contents of a file in a "string buffer"
        std::stringstream vertexShaderStream, fragmentShaderStream;
        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();

        //turn the string from the file into a std::string
        vertexSourceCode=vertexShaderStream.str();
        fragmentSourceCode=fragmentShaderStream.str();
    }
    catch(...)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }





    //list of chars that openGL prefers over strings
    const char* vShaderCode = vertexSourceCode.c_str();
    const char* fShaderCode = fragmentSourceCode.c_str();


    int shaderSuccess;
    char infoLog[512];


    //create the vertex shader (shader responsible for moving points to the correct location)
    vertex = glCreateShader(GL_VERTEX_SHADER);
    //this shader is responsible for handling color (look at vertex shader for explanation on what each function do)
    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    //sets the code for the shader that we're making to be the one set in graphics.h. the second and fourth parameters are unimportant because we are not using multiple strings
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);

    //compile the shader's code
    glCompileShader(vertex);
    glCompileShader(fragment);



    //check if shader loaded correctly
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &shaderSuccess);
    if(!shaderSuccess)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &shaderSuccess);
    if(!shaderSuccess)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }



    //this is a shader program that brings the other shaders together in a single program that will run every time we render an object
    shaderProgram = glCreateProgram();

    //connect the 2 shaders to the shader program
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    //makes shaderProgram a full shader by A) checking to make sure everything is fit correctly and B) bringing it together
    glLinkProgram(shaderProgram);

    //check if final shader loaded correctly
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderSuccess);
    if(!shaderSuccess)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //delete shaders from local memory?
    glDeleteShader(vertex);
    glDeleteShader(fragment);


}
void shader::use() const
{
    //the thing that tells openGL to use the shader whenever rendering something
    glUseProgram(shaderProgram);
}
shader::~shader()
{
    glDeleteProgram(shaderProgram);
}
