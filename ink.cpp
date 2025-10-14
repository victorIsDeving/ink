#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "stb_image/stb_image.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <filesystem>

double lastTime,lt,fps,iniTime, totalTime;
int nbFrames = 0;
char title_string[50];
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window,double *mouse, bool * shouldDraw, bool * press);
void DisplayFramebufferTexture(unsigned int textureID, Shader *program,unsigned int VAO,glm::vec2 R);
unsigned int loadTexture(const char *path);
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
const char *imagePath;

float vertices[] = {
    1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
};

float TVvertices[] = {
    1.0f,  1.0f,   1.0f, 1.0f, // top right
    1.0f, -1.0f,   1.0f, 0.0f, // bottom right
    -1.0f, -1.0f,   0.0f, 0.0f, // bottom left
    -1.0f,  1.0f,   0.0f, 1.0f  // top left
};

unsigned int indices[] = {
    0,1,3,
    1,2,3
};

int main()
{
    if (__cplusplus == 201703L)
        std::cout << "C++17" << std::endl;
    else if (__cplusplus == 201402L)
        std::cout << "C++14" << std::endl;
    else if (__cplusplus == 201103L)
        std::cout << "C++11" << std::endl;
    else if (__cplusplus == 199711L)
        std::cout << "C++98" << std::endl;
    else
        std::cout << "pre-standard C++" << std::endl;


    std::filesystem::path path = std::filesystem::current_path();
    const std::string pathString = path.string();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    int nc;
    const std::string imageP = "../../../../resources/textures/ShaderToyTextures/enterprise.png";
    imagePath = imageP.c_str();
//    unsigned char *data = stbi_load(imagePath, &SCR_WIDTH, &SCR_HEIGHT, &nc, 0);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Flythrough", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Fazer a imagem não dividir em 4 na janela do OpenGL
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    // Update your screen dimensions to match framebuffer
    SCR_WIDTH = framebufferWidth;
    SCR_HEIGHT = framebufferHeight;
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::string parentPath = (path.parent_path().parent_path()).string();

    const std::string vertexShader = "/Shaders/VertexShader.vs";
    const std::string vertexPath = parentPath + vertexShader;
    const char * vertexShaderPath = vertexPath.c_str();

    const std::string TVvertexShader = "/Shaders/TextureViewer.vs.vert";
    const std::string TVvertexPath = parentPath + TVvertexShader;
    const char * TVvertexShaderPath = TVvertexPath.c_str();


    const std::string fragmentShader = "/Shaders/Image.fs";
    const std::string fragmentPath = parentPath + fragmentShader;
    const char * fragmentShaderPath = fragmentPath.c_str();
   


    const std::string fragmentShaderA = "/Shaders/BufferA.fs";
    const std::string fragmentPathA = parentPath + fragmentShaderA;
    const char * fragmentShaderAPath = fragmentPathA.c_str();


    const std::string fragmentShaderB = "/Shaders/BufferB.fs";
    const std::string fragmentPathB = parentPath + fragmentShaderB;
    const char * fragmentShaderBPath = fragmentPathB.c_str();

    const std::string fragmentShaderC = "/Shaders/BufferC.fs";
    const std::string fragmentPathC = parentPath + fragmentShaderC;
    const char * fragmentShaderCPath = fragmentPathC.c_str();

    const std::string fragmentShaderD = "/Shaders/BufferD.fs";
    const std::string fragmentPathD = parentPath + fragmentShaderD;
    const char * fragmentShaderDPath = fragmentPathD.c_str();

    const std::string TVShaderString = "/Shaders/TextureViewer.fs.frag";
    const std::string TVShaderPathST = parentPath + TVShaderString;
    const char * TVShaderPath = TVShaderPathST.c_str();



    const std::string cString = parentPath+ "/Shaders/common.incl";
    const char * commonPath = cString.c_str();
    Shader Imageprogram(commonPath,vertexShaderPath,fragmentShaderPath);
    Shader BufferAprogram(commonPath,vertexShaderPath,fragmentShaderAPath);
    Shader BufferBprogram(commonPath,vertexShaderPath,fragmentShaderBPath);
//    Shader BufferCprogram(commonPath,vertexShaderPath,fragmentShaderCPath);
//    Shader BufferDprogram(commonPath,vertexShaderPath,fragmentShaderDPath);
    const char * commonPath1 = cString.c_str();
    Shader TVShaderprogram(commonPath1,TVvertexShaderPath,TVShaderPath);

    ////////////////////
    // OPENGL BÁSICOS //
    ////////////////////
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),indices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),(void *)0 );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),(void *) (3*sizeof(float)) );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float),(void *) (6*sizeof(float)) );
    glEnableVertexAttribArray(2);



    ////////////////////
    // OPENGL BÁSICOS //
    ////////////////////
    unsigned int TV_VBO, TV_VAO, TV_EBO;
    glGenVertexArrays(1, &TV_VAO);
    glGenBuffers(1, &TV_VBO);
    glGenBuffers(1, &TV_EBO);
    glBindVertexArray(TV_VAO);

    glBindBuffer(GL_ARRAY_BUFFER,TV_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVvertices), TVvertices, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TV_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),indices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),(void *)0 );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),(void *) (2*sizeof(float)) );
    glEnableVertexAttribArray(1);


    //unsigned int imageTexture0 = loadTexture(imagePath);
    ////////////////////
    //    TEXTURES    //
    ////////////////////

    //Abrindo textura presente nas pastas do projeto]
    //const std::string container = "/../black.jpg";
    //const std::string texturePath = pathString + container;

    //std::cout << texturePath << std::endl;
    //const char * finalPath = texturePath.c_str();
    //std::cout << "finalPath:" << finalPath << std::endl;
    //int width, height, nrChannels;
    //unsigned char *data = stbi_load(finalPath, &width, &height, &nrChannels, 0);



    //printf("hello");

    ////////////////////
    //  FRAMEBUFFER   //
    ////////////////////

     

    //Criando o framebuffer
    unsigned int FBO_0;
    glGenFramebuffers(1, &FBO_0);

    //Ligando o framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,FBO_0);

    //Para utilizar o framebuffer, é necessário ligar a ele uma textura
    unsigned int iChannel_0;

    glGenTextures(1, &iChannel_0);
    glBindTexture(GL_TEXTURE_2D, iChannel_0);
     
    //Gerando uma textura vazia para ligar ao framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    //LIGANDO A TEXTURA AO FRAMBUFFER

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iChannel_0, 0 );

    //create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo_0;
    glGenRenderbuffers(1, &rbo_0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_0);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_0); // now actually attach it
    //now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //Criando o framebuffer
    unsigned int FBO_1;
    glGenFramebuffers(1, &FBO_1);
    //Ligando o framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,FBO_1);


    unsigned int iChannel_1;

    glGenTextures(1, &iChannel_1);
    glBindTexture(GL_TEXTURE_2D, iChannel_1);

    //Gerando uma textura vazia para ligar ao framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    //LIGANDO A TEXTURA AO FRAMBUFFER

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iChannel_1, 0);

    //create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo_1;
    glGenRenderbuffers(1, &rbo_1);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_1);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_1); // now actually attach it
    //now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
         std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //Criando o framebuffer
    unsigned int FBO_2;
    glGenFramebuffers(1, &FBO_2);
    //Ligando o framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,FBO_2);

    unsigned int iChannel_2;

    glGenTextures(1, &iChannel_2);
    glBindTexture(GL_TEXTURE_2D, iChannel_2);

    //Gerando uma textura vazia para ligar ao framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    //LIGANDO A TEXTURA AO FRAMBUFFER

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iChannel_2, 0 );

    //create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo_2;
    glGenRenderbuffers(1, &rbo_2);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_2); // now actually attach it
    //now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
         std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //Criando o framebuffer
    unsigned int FBO_3;
    glGenFramebuffers(1, &FBO_3);
    //Ligando o framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,FBO_3);

    unsigned int iChannel_3;

    glGenTextures(1, &iChannel_3);
    glBindTexture(GL_TEXTURE_2D, iChannel_3);

    //Gerando uma textura vazia para ligar ao framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    //LIGANDO A TEXTURA AO FRAMBUFFER

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iChannel_3, 0 );

    //create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo_3;
    glGenRenderbuffers(1, &rbo_3);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_3);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_3); // now actually attach it
    //now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
         std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //int MaxTextureImageUnits;
    //glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    double * mouse = (double *) malloc(sizeof(double)*4);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textura);
    //Shaderprogram2.use();
    //Shaderprogram2.setSampler("Texture");
    glm::vec2 resolution(SCR_WIDTH,SCR_HEIGHT);
    //In your render loop, temporarily make canvas fullscreen:
    glm::vec2 canvasSize = resolution;
    glm::vec2 canvasOffset = glm::vec2(0.0f);
    //Shaderprogram2.setVec2("iResolution",resolution);
    Imageprogram.use();
    Imageprogram.setVec2("iResolution",resolution);

    bool shouldDraw = false;
    bool press = false;
    float currentTime;
    int frame=0;
    ////////////////////
    //  RENDER LOOP   //
    ////////////////////
    while(!glfwWindowShouldClose(window)){
        currentTime = static_cast<float>(glfwGetTime());
        //Renderizando para o framebuffer criado
        
        processInput(window,mouse, &shouldDraw, & press);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        //glEnable(GL_DEPTH_TEST);
        
//        int framebufferWidth, framebufferHeight;
//        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
//        glViewport(0, 0, framebufferWidth, framebufferHeight);
//
//        // Update resolution to match actual framebuffer
//        glm::vec2 resolution(framebufferWidth, framebufferHeight);
        
        // --- Ping-pong state ---
        static bool pingA = false;
        static bool pingB = false;

        if (shouldDraw)
        {
            // Buffer A
            unsigned int readTexA = pingB ? iChannel_3 : iChannel_2;
            unsigned int writeFBO_A = pingA ? FBO_0 : FBO_1;
            glBindFramebuffer(GL_FRAMEBUFFER, writeFBO_A);
            glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            BufferAprogram.use();
            BufferAprogram.setSampler("iChannel0", 0);
            BufferAprogram.setVec2("iResolution", resolution);
            BufferAprogram.setVec4("iMouse", mouse);
            BufferAprogram.setFloat("iTime", totalTime);
            BufferAprogram.setInt("iFrame", frame);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, readTexA);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            unsigned int justWrittenAtex = (writeFBO_A == FBO_0) ? iChannel_0 : iChannel_1;
            pingA = !pingA; // swap A

            // Buffer B
            unsigned int writeFBO_B = pingB ? FBO_2 : FBO_3;
            glBindFramebuffer(GL_FRAMEBUFFER, writeFBO_B);
            glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT);

            BufferBprogram.use();
            BufferBprogram.setSampler("iChannel0", 0);
            BufferBprogram.setVec2("iResolution", resolution);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, justWrittenAtex);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            unsigned int justWrittenBtex = (writeFBO_B == FBO_2) ? iChannel_2 : iChannel_3;
            pingB = !pingB; // swap B

            // Final render
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            Imageprogram.use();
            Imageprogram.setVec2("iResolution", resolution);
            Imageprogram.setSampler("iChannel0", 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, justWrittenBtex);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            double CurrentTime = glfwGetTime();
            double past = CurrentTime - lt;
            totalTime = CurrentTime-iniTime;
            if(past>0.1) {
                fps = (float)nbFrames/past;
                sprintf(title_string, "Inking - FPS = %.i ", (int)fps);
                glfwSetWindowTitle(window, title_string);
                lt+=past;
                nbFrames = 0;
            }
            nbFrames++;
            frame++;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteFramebuffers(1, &FBO_0);
    glDeleteFramebuffers(1, &FBO_1);
    glDeleteFramebuffers(1, &FBO_2);
    glDeleteFramebuffers(1, &FBO_3);
    glDeleteRenderbuffers(1, &rbo_0);
    glDeleteRenderbuffers(1, &rbo_1);
    glDeleteRenderbuffers(1, &rbo_2);
    glDeleteRenderbuffers(1, &rbo_3);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window,double *mouse, bool * shouldDraw, bool * press)
{
    //Caso aperte 'esc', indica que a janela deve ser fechada
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    //Alocando o vetor que contém o mouse
    //double * mouse = (double *) malloc(sizeof(double)*4);

    //Encontrando a posição do mouse
    glfwGetCursorPos(window,&mouse[0], &mouse[1]);
    mouse[2] = 0.0;
    mouse[3] = 0.0;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) mouse[2] = 1.0;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) mouse[3] = 1.0;

    // Get both window size and framebuffer size
    int windowWidth, windowHeight;
    int framebufferWidth, framebufferHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    
    // Calculate the scale factor
    float scaleX = (float)framebufferWidth / (float)windowWidth;
    float scaleY = (float)framebufferHeight / (float)windowHeight;
    
    // Convert mouse coordinates to framebuffer space
    mouse[0] = mouse[0] * scaleX;
    mouse[1] = (windowHeight - mouse[1]) * scaleY;  // Flip Y and scale
    
    // Debug output (uncomment to see the coordinates)
//    static int counter = 0;
//    if (counter++ % 60 == 0) {  // Print every 60 frames
//        std::cout << "Window: " << windowWidth << "x" << windowHeight
//                  << " Framebuffer: " << framebufferWidth << "x" << framebufferHeight
//                  << " Scale: " << scaleX << "x" << scaleY
//                  << " Mouse: (" << mouse[0] << ", " << mouse[1] << ")" << std::endl;
//    }
    
    if(glfwGetKey(window, 32) == GLFW_PRESS && !*press) {
        *shouldDraw = !*shouldDraw;
        lt = glfwGetTime();
        iniTime=lt;
        * press = true;
    }
    if(glfwGetKey(window, 32) == GLFW_RELEASE) * press = false;
}

void DisplayFramebufferTexture(unsigned int textureID,Shader *program, unsigned int VAO,glm::vec2 R)
{

   program->use();


    program->setSampler("fboAttachment",0);
    program->setVec2("iResolution",R);
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
    glUseProgram(0);
}

//call: chamada quando um determinado evento acontece
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    printf("%i:%i\n", width,height);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
         format = GL_RED;
        else if (nrComponents == 3)
         format = GL_RGB;
        else if (nrComponents == 4)
         format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
