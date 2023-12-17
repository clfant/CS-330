#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h> // GLFW library
#include <C:\Users\Chris\OneDrive\Documents\SNHU\CS_330\OpenGL\OpenGL\SOIL2\SOIL2.h>


// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <C:\Users\Chris\OneDrive\Documents\SNHU\CS_330\OpenGL\OpenGL\camera.h> // Camera class

using namespace std; // Standard namespace

// Location for Objects
float boxLocX = 4.0f, boxLocY = -0.5f, boxLocZ = -3.0f;
float planeLocX = 0.0f, planeLocY = -0.5f, planeLocZ = 0.0f; 
float backLocX = 0.0f, backLocY = 0.0f, backLocZ = -7.0f;
float triLocX = -2.5f, triLocY = -0.5f, triLocZ = 7.0f;
float holderLocX = 1.0f, holderLocY = -0.25f, holderLocZ = 10.0f;
float spoonLocX = 1.0f, spoonLocY = 0.0f, spoonLocZ = 10.0f;
float snowLocX = 0.0f, snowLocY = -0.5f, snowLocZ = 4.0f;
float camX = 0.0f, camY = 0.0f, camZ = 6.0f;
glm::mat4 pMat, vMat, mMat, mvMat; // mvp matrix variables

// Array for rectangle rotations
glm::float32 recRotations[] = { 0.0f, 90.0f, 180.0f, 270.0f };


/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Goldfish: Flavor Blasted"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        //
        GLuint boxVAO, boxVBO, boxEBO;      // Handle for the vertex array object
        GLuint planeVAO, planeVBO, planeEBO;// Handles for the vertex buffer objects
        GLuint triVAO, triVBO, triEBO;
        GLuint backPlaneVAO, backPlaneVBO;
        GLuint nBoxIndices, nPlaneIndices, nTriIndices, nRecIndices, nJarIndices, nHolderIndices, nHolderBottomIndices, nSpoonHandleIndices, nSnowIndices;    // Number of indices of the mesh
        GLuint recVAO, recVBO, recEBO;
        GLuint holderVAO, holderVBO, holderEBO;
        GLuint holderBVAO, holderBVBO, holderBEBO;
        GLuint spoonVAO, spoonVBO, spoonEBO;
        GLuint snowVAO, snowVBO, snowEBO;
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Texture id
    GLuint gTextureIdGoldFish;
    GLuint gTextureIdPlane;
    GLuint gTextureIdPlane2;
    GLuint gTextureIdHolder;
    GLuint gTextureIdJar;
    GLuint gTextureIdWood;
    GLuint gTextureIdGlass;
    GLuint gTextureIdRed;
    glm::vec2 gUVScale(5.0f, 5.0f);
    GLint gTexWrapMode = GL_REPEAT;
    // Shader program
    GLuint gProgramId;

    // camera
    Camera gCamera(glm::vec3(0.0f, 4.0f, 15.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    GLfloat fov = 45.0f;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);



/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
    layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1
    layout(location = 2) in vec2 texCoord;

    out vec4 vertexColor; // variable to transfer color data to the fragment shader
    out vec2 oTexCoord;

    //Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates .x, position.y, position.z  
    vertexColor = color;   // references incoming color data
    oTexCoord = texCoord;
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader
    in vec2 oTexCoord;

    out vec4 fragmentColor;
    uniform sampler2D goldFishTexture;
    uniform sampler2D planeTexture;
    uniform bool multipleTextures;
    uniform sampler2D planeTexture2;
    uniform sampler2D holderTexture;
    uniform sampler2D jarTexture;
    uniform sampler2D woodTexture;
    uniform sampler2D glassTexture;
    uniform sampler2D redTexture;

void main()
{
    fragmentColor = texture(goldFishTexture, oTexCoord);
    if (multipleTextures)
    {
        vec4 secondaryTexture = texture(planeTexture, oTexCoord);
        vec4 thirdTexture = texture(planeTexture2, oTexCoord);
        vec4 fourthTexture = texture(holderTexture, oTexCoord);
        vec4 fifthTexture = texture(jarTexture, oTexCoord);
        vec4 sixthTexture = texture(woodTexture, oTexCoord);
        vec4 seventhTexture = texture(glassTexture, oTexCoord);
        vec4 eigthTexture = texture(redTexture, oTexCoord);

        if (secondaryTexture.a != 0.0)
            fragmentColor = secondaryTexture;
        if (thirdTexture.a != 0.0)
            fragmentColor = thirdTexture;
        if (fourthTexture.a != 0.0)
            fragmentColor = fourthTexture;
        if (fifthTexture.a != 0.0)
            fragmentColor = fifthTexture;
        if (sixthTexture.a != 0.0)
            fragmentColor = sixthTexture;
        if (seventhTexture.a != 0.0)
            fragmentColor = seventhTexture;
        if (eigthTexture.a != 0.0)
            fragmentColor = eigthTexture;
    }

}
);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object



    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Load Textures
    const char* file = "goldfish.jpg";
    if (!UCreateTexture(file, gTextureIdGoldFish))
    {
        cout << "Failed to load texture " << file << endl;
        return EXIT_FAILURE;
    }
    const char* file2 = "Granite_Counter_Top.jpg";
    if (!UCreateTexture(file2, gTextureIdPlane))
    {
        cout << "Failed to load texture " << file2 << endl;
        return EXIT_FAILURE;
    }
    const char* file3 = "Tile.jpg";
    if (!UCreateTexture(file3, gTextureIdPlane2))
    {
        cout << "Failed to load texture " << file3 << endl;
        return EXIT_FAILURE;
    }
    const char* file4 = "Spoon_Holder.jpg";
    if (!UCreateTexture(file4, gTextureIdHolder))
    {
        cout << "Failed to load texture " << file4 << endl;
        return EXIT_FAILURE;
    }
    const char* file5 = "jar1.jpg";
    if (!UCreateTexture(file5, gTextureIdJar))
    {
        cout << "Failed to load texture " << file5 << endl;
        return EXIT_FAILURE;
    }
    const char* file6 = "wood.jpg";
    if (!UCreateTexture(file6, gTextureIdWood))
    {
        cout << "Failed to load texture " << file6 << endl;
        return EXIT_FAILURE;
    }
    const char* file7 = "glass.jpg";
    if (!UCreateTexture(file7, gTextureIdGlass))
    {
        cout << "Failed to load texture " << file7 << endl;
        return EXIT_FAILURE;
    }
    const char* file8 = "red.jpg";
    if (!UCreateTexture(file8, gTextureIdRed))
    {
        cout << "Failed to load texture " << file8 << endl;
        return EXIT_FAILURE;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // set the texture as texture unit
    glUniform1i(glGetUniformLocation(gProgramId, "goldFishTexture"), 0);

    glUniform1i(glGetUniformLocation(gProgramId, "planeTexture"), 1);

    glUniform1i(glGetUniformLocation(gProgramId, "planeTexture2"), 2);

    glUniform1i(glGetUniformLocation(gProgramId, "holderTexture"), 3);

    glUniform1i(glGetUniformLocation(gProgramId, "jarTexture"), 4);

    glUniform1i(glGetUniformLocation(gProgramId, "woodTexture"), 5);

    glUniform1i(glGetUniformLocation(gProgramId, "glassTexture"), 6);

    glUniform1i(glGetUniformLocation(gProgramId, "redTexture"), 7);



    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release shader program
    UDestroyShaderProgram(gProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        gCamera.ProcessKeyboard(VIEW, gDeltaTime);
    
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Functioned called to render a frame
void URender()
{

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the shader to be used
    glUseProgram(gProgramId);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    // 2. Rotates shape by 15 degrees in the x axis
    glm::mat4 rotation = glm::rotate(0.01f, glm::vec3(0.01f, 0.01f, 0.01f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Transforms the camera: move the camera back (z axis)
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);


    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-camX, -camY, -camZ));


    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Build the plane for counter
    glBindTexture(GL_TEXTURE_2D, gTextureIdPlane);
    glBindVertexArray(gMesh.planeVAO);
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(planeLocX, planeLocY, planeLocZ)); // translate plane (note initial glm::mat4(1.0f))	 
    mMat = glm::rotate(mMat, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f)); // concatenate rotatation (note mMat)	
    mMat = glm::scale(mMat, glm::vec3(3.f, 3.f, 3.f)); // Scale plane
    //Copy perspective and MV matrices to uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glEnable(GL_DEPTH_TEST); // Z-buffer operation
    glDepthFunc(GL_LEQUAL); // Used with Depth test
    glDrawArrays(GL_TRIANGLES, 0, 6); // draws plane
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Build the plane for the back wall
    glBindTexture(GL_TEXTURE_2D, gTextureIdPlane2);
    glBindVertexArray(gMesh.backPlaneVAO);
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(backLocX, backLocY, backLocZ));	 
    mMat = glm::rotate(mMat, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f)); // concatenate rotatation (note mMat)	
    mMat = glm::scale(mMat, glm::vec3(3.f, 3.f, 3.f));
    //Copy perspective and MV matrices to uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glEnable(GL_DEPTH_TEST); // Z-buffer operation
    glDepthFunc(GL_LEQUAL); // Used with Depth test
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    // Activate the VBOs contained within the mesh's VAO
    glBindTexture(GL_TEXTURE_2D, gTextureIdGoldFish);
    glBindVertexArray(gMesh.recVAO);
    // build base for goldfish box
    for (int i = 0; i < 4; ++i)
    {
        
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(boxLocX, boxLocY, boxLocZ)); 	 
        mMat = glm::rotate(mMat, glm::radians(recRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f)); // concatenate rotatation (note mMat)	
        mMat = glm::scale(mMat, glm::vec3(3.f, 3.f, 3.f));
        //Copy perspective and MV matrices to uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glEnable(GL_DEPTH_TEST); // Z-buffer operation
        glDepthFunc(GL_LEQUAL); // Used with Depth test
        glDrawElements(GL_TRIANGLES, gMesh.nRecIndices, GL_UNSIGNED_SHORT, NULL);
    }
    glBindVertexArray(0);
     
    // build top of goldfish box
    glBindTexture(GL_TEXTURE_2D, gTextureIdGoldFish);
    glBindVertexArray(gMesh.boxVAO);
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(boxLocX, boxLocY, boxLocZ)); 	 
    mMat = glm::rotate(mMat, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f)); // concatenate rotatation (note mMat)	
    mMat = glm::scale(mMat, glm::vec3(3.f, 3.f, 3.f));
    //Copy perspective and MV matrices to uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glEnable(GL_DEPTH_TEST); // Z-buffer operation
    glDepthFunc(GL_LEQUAL); // Used with Depth test
    glDrawElements(GL_TRIANGLES, gMesh.nBoxIndices, GL_UNSIGNED_SHORT, NULL);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    // build the jar
    glBindTexture(GL_TEXTURE_2D, gTextureIdJar);
    glBindVertexArray(gMesh.triVAO); // Activate VAO (now references VBO and VA association) Can be placed anywhere before glDrawArrays()
    // Use loop to build Model matrix for triangle
    for (float i = 0; i < 30; i++) {
        // Apply Transform to model // Build model matrix for tri
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(triLocX, triLocY, triLocZ));
        mMat = glm::rotate(mMat, i * (360 / 30), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip i times the angle
        mMat = glm::scale(mMat, glm::vec3(1.5f, 1.5f, 1.5f)); // Scale triangle		
        mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
        //Copy perspective and MV matrices to uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glEnable(GL_DEPTH_TEST); // Z-buffer operation
        glDepthFunc(GL_LEQUAL); // Used with Depth test
        glDrawElements(GL_TRIANGLES, gMesh.nTriIndices, GL_UNSIGNED_SHORT, NULL);
        

    }

    // build the base for the holder
    glBindVertexArray(gMesh.holderVAO); // Activate VAO (now references VBO and VA association) Can be placed anywhere before glDrawArrays()
    // Use loop to build Model matrix for triangle
    for (float i = 0; i < 30; i++) {
        // Apply Transform to model // Build model matrix for tri
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(holderLocX, holderLocY, holderLocZ)); 
        mMat = glm::rotate(mMat, i * (360 / 30), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip i times the angle
        mMat = glm::scale(mMat, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale triangl		
        mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
        //Copy perspective and MV matrices to uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glEnable(GL_DEPTH_TEST); // Z-buffer operation
        glDepthFunc(GL_LEQUAL); // Used with Depth test
        glDrawElements(GL_TRIANGLES, gMesh.nHolderIndices, GL_UNSIGNED_SHORT, NULL);


    }
    glBindVertexArray(0);// Optional unbinding but recommended

    // build bottom of holder
    glBindVertexArray(gMesh.holderBVAO);
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(holderLocX, holderLocY, holderLocZ));
    mMat = glm::rotate(mMat, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f)); // concatenate rotatation (note mMat)
    mMat = glm::scale(mMat, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale		
    mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
    //Copy perspective and MV matrices to uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glEnable(GL_DEPTH_TEST); // Z-buffer operation
    glDepthFunc(GL_LEQUAL); // Used with Depth test
    glDrawElements(GL_TRIANGLES, gMesh.nHolderBottomIndices, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);


    // build spoon head
    glBindTexture(GL_TEXTURE_2D, gTextureIdWood);
    glBindVertexArray(gMesh.holderVAO); // Activate VAO (now references VBO and VA association) Can be placed anywhere before glDrawArrays()
    // Use loop to build Model matrix for triangle
    for (float i = 0; i < 30; i++) {
        // Apply Transform to model // Build model matrix for tri
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(spoonLocX, spoonLocY, spoonLocZ)); 
        mMat = glm::rotate(mMat, glm::radians(9.0f), glm::vec3(1.0f, 0.0f, 1.5f)); // Rotate to match the spoon laying on holder
        mMat = glm::rotate(mMat, i * (360 / 30), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip i times the angle
        mMat = glm::scale(mMat, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale triangl		
        mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
        //Copy perspective and MV matrices to uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glEnable(GL_DEPTH_TEST); // Z-buffer operation
        glDepthFunc(GL_LEQUAL); // Used with Depth test
        glDrawElements(GL_TRIANGLES, gMesh.nHolderIndices, GL_UNSIGNED_SHORT, NULL);


    }
    glBindVertexArray(0);

    // build spoon handle
    glBindVertexArray(gMesh.spoonVAO);
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(spoonLocX, spoonLocY, spoonLocZ));
    mMat = glm::rotate(mMat, glm::radians(9.0f), glm::vec3(1.0f, 0.0f, 1.5f)); // rotate to match head
    mMat = glm::scale(mMat, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale triangl		
    mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
    //Copy perspective and MV matrices to uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glEnable(GL_DEPTH_TEST); // Z-buffer operation
    glDepthFunc(GL_LEQUAL); // Used with Depth test
    glDrawElements(GL_TRIANGLES, gMesh.nHolderBottomIndices, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);

    // build snowman
    glBindTexture(GL_TEXTURE_2D, gTextureIdGlass);
    glBindVertexArray(gMesh.snowVAO); // Activate VAO (now references VBO and VA association) Can be placed anywhere before glDrawArrays()
    // Use loop to build Model matrix for triangle
    for (float i = 0; i < 30; i++) {
        // Apply Transform to model // Build model matrix for tri
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(snowLocX, snowLocY, snowLocZ));
        mMat = glm::rotate(mMat, i * (360 / 30), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip i times the angle
        mMat = glm::scale(mMat, glm::vec3(1.5f, 1.0f, 1.5f)); // Scale triangl		
        mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
        //Copy perspective and MV matrices to uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glEnable(GL_DEPTH_TEST); // Z-buffer operation
        glDepthFunc(GL_LEQUAL); // Used with Depth test
        glDrawElements(GL_TRIANGLES, gMesh.nTriIndices, GL_UNSIGNED_SHORT, NULL);


    }
    glBindVertexArray(0);
    // Use loop to build Model matrix for triangle
    glBindTexture(GL_TEXTURE_2D, gTextureIdGlass);
    glBindVertexArray(gMesh.snowVAO);
    for (float i = 0; i < 30; i++) {
        // Apply Transform to model // Build model matrix for tri
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(snowLocX, snowLocY+2, snowLocZ));
        mMat = glm::rotate(mMat, i * (360 / 30), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip i times the angle
        mMat = glm::scale(mMat, glm::vec3(1.0f, 0.5f, 1.0f)); // Scale triangl		
        mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
        //Copy perspective and MV matrices to uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glEnable(GL_DEPTH_TEST); // Z-buffer operation
        glDepthFunc(GL_LEQUAL); // Used with Depth test
        glDrawElements(GL_TRIANGLES, gMesh.nTriIndices, GL_UNSIGNED_SHORT, NULL);
    }
    glBindVertexArray(0);

    // build lid for snowman
    glBindTexture(GL_TEXTURE_2D, gTextureIdRed);
    glBindVertexArray(gMesh.snowVAO);
    for (float i = 0; i < 30; i++) {
        // Apply Transform to model // Build model matrix for tri
        mMat = glm::translate(glm::mat4(1.0f), glm::vec3(snowLocX, snowLocY + 3, snowLocZ));
        mMat = glm::rotate(mMat, i * (360 / 30), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip i times the angle
        mMat = glm::scale(mMat, glm::vec3(1.0f, 0.125f, 1.0f)); // Scale triangl		
        mvMat = vMat * mMat; // view and model matrix multiplied here (instead of in shader for better performance)
        //Copy perspective and MV matrices to uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glEnable(GL_DEPTH_TEST); // Z-buffer operation
        glDepthFunc(GL_LEQUAL); // Used with Depth test
        glDrawElements(GL_TRIANGLES, gMesh.nTriIndices, GL_UNSIGNED_SHORT, NULL);
    }
    glBindVertexArray(0);
    
    


    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat boxVerts[] = {
        // Vertex Positions    // Colors (r,g,b,a)       

         0.5f, 1.5f, 0.5f, 
         -2.0f,  0.0f, 0.0f, 1.0f,  
         0.0f, 0.0f, // 0

        0.5f, 1.5f, -0.5f, -2.0f,  
        0.0f, 0.0f,1.0f,  
        1.0f, 0.0f, // 1

        -0.5f, 1.5f, -0.5f, 
        -2.0f, 0.0f, 0.0f,1.0f,  
        0.0f, 0.0f, // 2

        -0.5f, 1.5f, 0.5f, 
        -2.0f,  0.0f, 0.0f,1.0f,  
        1.0f, 0.0f,  // 3
        
        0.1f,  1.7f,   0.0f,  
        0.0f, 0.0f, 0.0f, 1.0f,  
        1.0f, 0.0f,   // Lid Mid Right Vertex 4 

        -0.1f,  1.7f,   0.0f,  
        0.0f, 0.0f, 0.0f, 1.0f,  
        0.0f, 0.0f,   // Lid Mid Left Vertex 5          

         0.5f,  1.75f,  0.0f,  
         1.0f, 1.0f, 0.5f, 1.0f,  
         1.0f, 1.0f,    // Lid Outside Right Vertex 6   

         -0.5f,  1.75f,  0.0f,  
         1.0f, 1.0f, 0.5f, 1.0f,  
         0.0f, 1.0f,   // Lid Outside Left Vertex 7  

        
       

         
    };

    // Index data to share position data
    GLushort boxIndices[] = {
       0, 1, 3,  // Triangle 1 Base
        1, 2, 3,  // Triangle 2 Base
       
        0, 1, 4,  // Triangle 11 Inside Lid Right
        1, 4, 6, // Triangle 13 in lid front right
        4, 0, 6,

        1, 7, 6, // Triangle 15 out lid front 1
        1, 2, 7,  // Triangle 16 out lid front 2

        2, 5, 3,  // Triangle 12 Inside Lid Left
        3, 5, 7, // Triangle 17 in lid front left
        2, 5, 7, // Trinagle 18 in lid back left

        3, 7, 6, // Triangle 19 out lid back 1
        6, 0, 3,  // Triangle 20 out lid back 2

    };

    // Position and Color data
    GLfloat recVerts[] = {
          

         0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,   // Top Back Right Vertex 4       
         0.5f,  0.0f,  -0.5f,  1.0f, 1.0f, 0.5f, 1.0f,  1.0f, 0.0f,   // Top Front Right Vertex 5      
         0.5f,  1.5f,  -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,   // Top Front Left Vertex 6        
         0.5f,  1.5f,  0.5f,  1.0f, 1.0f, 0.5f, 1.0f,  0.0f, 1.0f,   // Top Back Left Vertex 7        


    };

    // Index data to share position data
    GLushort recIndices[] = {
        0, 1, 3,  // Triangle 1 Base
        1, 2, 3,  // Triangle 2 Base
        
    };


    GLfloat planeVerts[] = {
        -5.0f, 0.0f,  -3.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,    //vertex 12 Plane    
         5.0f, 0.0f,  -3.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,    //vertex 13 Plane    
         5.0f, 0.0f,   3.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,    //vertex 14 Plane
         5.0f, 0.0f,   3.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,    //vertex 14 Plane
        -5.0f, 0.0f,   3.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,    //vertex 15 Plane
        -5.0f, 0.0f,  -3.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,    //vertex 12 Plane
    };

    GLfloat backPlaneVerts[] = {
        -5.0f, -0.5f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,    //vertex 12 Plane    
         5.0f, -0.5f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,    //vertex 13 Plane    
         5.0f,  2.5f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,    //vertex 14 Plane
         5.0f,  2.5f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,    //vertex 14 Plane
        -5.0f,  2.5f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,    //vertex 15 Plane
        -5.0f, -0.5f,  0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,    //vertex 12 Plane
    };

    // Define vertex data for Triangle (GLfloat array)
    GLfloat jarBaseVertices[] = {
        // positon attributes (x,y,z)
        

        0.0f, 0.0f, 0.0f,  // vert 0  
        1.0f, 0.0f, 0.0f, 1.0, // red
        1.0f, 0.0f,

        1.0f, 0.0f, 0.0f, // vert 1
        0.0f, 0.0f, 1.0f, 1.0, // blue
        1.0f, 0.0f,

        0.8439f, 0.0f, -0.5366f, // vert 2
        0.0f, 1.0f, 0.0f, 1.0,// green
        1.0f, 0.0f,

        1.0f, 2.0f, 0.0f, // vert 3
        0.0f, 0.0f, 1.0f, 1.0, // blue
        0.0f, 1.0f,

        0.8439f, 2.0f, -0.5366f, // vert 4
        0.0f, 1.0f, 0.0f, 1.0,// green
        1.0f, 1.0f,

       
    };

    GLushort jarBaseIndices[] = {
     
        0, 1, 2,
        1, 2, 3,
        3, 4, 2,
        
    };

    GLfloat holderVertices[] = {
        0.0f, -0.25f, 0.0f,  // vert 0  
        1.0f, 0.0f, 0.0f, 1.0, // red
        1.0f, 1.0f,

        1.0f, 0.0f, 0.0f, // vert 1
        0.0f, 0.0f, 1.0f, 1.0, // blue
        1.0f, 0.0f,

        0.8439f, 0.0f, -0.5366f, // vert 2
        0.0f, 1.0f, 0.0f, 1.0,// green
        0.0f, 1.0f,
    };

    GLushort holderIndices[] = {
        0, 1, 2,
    };

    GLfloat holderBottomVertices[] = {
        -0.866f, 0.0f, 0.5,
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f,

        0.866f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f,

        -0.55f, 0.0f, 1.4f,
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f,

        0.55, 0.0f, 1.4f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f,

        0.0f, -.15f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f,
    };

    GLushort holderBottomIndices[] = {
        0, 2, 4,
        3, 2, 4,
        1, 3, 4
    };

    GLfloat spoonHandleVertices[] = {
        -0.6f, 0.0f, 0.8f, //  0
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f,

        -.866f, 0.0f, -0.5f, // 1
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f,

        -5.0f, 0.0f, 0.05f,  // 4
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f,

        -4.85f, 0.0f, 1.0f,  // 5
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f,




    };

    GLushort spoonHandleIndices[] = {
        0, 2, 1,
        2, 3, 0,
        
    };

    GLfloat snowVertices[] = {
        0.0f, 0.0f, 0.0f,  // vert 0  
        1.0f, 0.0f, 0.0f, 1.0, 
        1.0f, 0.0f,

        1.0f, 0.0f, 0.0f, // vert 1
        0.0f, 0.0f, 1.0f, 1.0, 
        0.0f, 0.0f,

        0.8439f, 0.0f, -0.5366f, // vert 2
        0.0f, 1.0f, 0.0f, 1.0,
        1.0f, 0.0f,

        1.0f, 2.0f, 0.0f, // vert 3
        0.0f, 0.0f, 1.0f, 1.0, 
        0.0f, 1.0f,

        0.8439f, 2.0f, -0.5366f, // vert 4
        0.0f, 1.0f, 0.0f, 1.0,
        1.0f, 1.0f,

        0.0f, 2.0f, 0.0f,  // vert 5  
        1.0f, 0.0f, 0.0f, 1.0, 
        1.0f, 0.0f,
    };

    GLushort snowIndices[] = {
        //0, 1, 2,
        1, 2, 3,
        5, 3, 4,
        3, 4, 2,
        
        
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerUV = 2;

    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV);



    // plane buffers
    glGenVertexArrays(1, &mesh.planeVAO);
    glGenBuffers(1, &mesh.planeVBO);
    glBindVertexArray(mesh.planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW); // Load vertex attributes
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    
    // goldfish buffers
    glGenBuffers(1, &mesh.boxVBO);
    glGenBuffers(1, &mesh.boxEBO);
    glGenVertexArrays(1, &mesh.boxVAO);
    glBindVertexArray(mesh.boxVAO);
    mesh.nBoxIndices = sizeof(boxIndices) / sizeof(boxIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.boxVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.boxEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVerts), boxVerts, GL_STATIC_DRAW); // Load vertex attributes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndices), boxIndices, GL_STATIC_DRAW); // Load indices
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // goldfish buffers part 2
    glGenBuffers(1, &mesh.recVBO);
    glGenBuffers(1, &mesh.recEBO);
    glGenVertexArrays(1, &mesh.recVAO);
    glBindVertexArray(mesh.recVAO);
    mesh.nRecIndices = sizeof(recIndices) / sizeof(recIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.recVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.recEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(recVerts), recVerts, GL_STATIC_DRAW); // Load vertex attributes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(recIndices), recIndices, GL_STATIC_DRAW); // Load indices
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // jar/triangle buffers
    glGenVertexArrays(1, &mesh.triVAO);
    glGenBuffers(1, &mesh.triVBO);
    glGenBuffers(1, &mesh.triEBO);
    glBindVertexArray(mesh.triVAO);
    mesh.nTriIndices = sizeof(jarBaseIndices) / sizeof(jarBaseIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.triVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.triEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(jarBaseVertices), jarBaseVertices, GL_STATIC_DRAW); // Load vertex attributes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(jarBaseIndices), jarBaseIndices, GL_STATIC_DRAW); // Load indices
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // back wall plane buffers
    glGenVertexArrays(1, &mesh.backPlaneVAO);
    glGenBuffers(1, &mesh.backPlaneVBO);
    glBindVertexArray(mesh.backPlaneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.backPlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backPlaneVerts), backPlaneVerts, GL_STATIC_DRAW); // Load vertex attributes
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // holder buffers
    glGenBuffers(1, &mesh.holderVBO);
    glGenBuffers(1, &mesh.holderEBO);
    glGenVertexArrays(1, &mesh.holderVAO);
    glBindVertexArray(mesh.holderVAO);
    mesh.nHolderIndices = sizeof(holderIndices) / sizeof(holderIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.holderVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.holderEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(holderVertices), holderVertices, GL_STATIC_DRAW); // Load vertex attributes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(holderIndices), holderIndices, GL_STATIC_DRAW); // Load indices
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // holder base buffers
    glGenBuffers(1, &mesh.holderBVBO);
    glGenBuffers(1, &mesh.holderBEBO);
    glGenVertexArrays(1, &mesh.holderBVAO);
    glBindVertexArray(mesh.holderBVAO);
    mesh.nHolderBottomIndices = sizeof(holderBottomIndices) / sizeof(holderBottomIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.holderBVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.holderBEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(holderBottomVertices), holderBottomVertices, GL_STATIC_DRAW); // Load vertex attributes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(holderBottomIndices), holderBottomIndices, GL_STATIC_DRAW); // Load indices
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // spoon handle buffers
    glGenBuffers(1, &mesh.spoonVBO);
    glGenBuffers(1, &mesh.spoonEBO);
    glGenVertexArrays(1, &mesh.spoonVAO);
    glBindVertexArray(mesh.spoonVAO);
    mesh.nSpoonHandleIndices = sizeof(spoonHandleIndices) / sizeof(spoonHandleIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.spoonVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.spoonEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spoonHandleVertices), spoonHandleVertices, GL_STATIC_DRAW); // Load vertex attributes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spoonHandleIndices), spoonHandleIndices, GL_STATIC_DRAW); // Load indices
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // snowman buffers
    glGenVertexArrays(1, &mesh.snowVAO);
    glGenBuffers(1, &mesh.snowVBO);
    glGenBuffers(1, &mesh.snowEBO);
    glBindVertexArray(mesh.snowVAO);
    mesh.nSnowIndices = sizeof(snowIndices) / sizeof(snowIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.snowVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.snowEBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snowVertices), snowVertices, GL_STATIC_DRAW); // Load vertex attributes
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(snowIndices), snowIndices, GL_STATIC_DRAW); // Load indices
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float)* (floatsPerVertex + floatsPerColor)));  //  
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

}

// destroy buffers
void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.boxVAO);
    glDeleteVertexArrays(1, &mesh.planeVAO);
    glDeleteBuffers(1, &mesh.boxVBO);
    glDeleteBuffers(1, &mesh.planeVBO);
    glDeleteVertexArrays(1, &mesh.triVAO);
    glDeleteBuffers(1, &mesh.triVBO);
    glDeleteBuffers(1, &mesh.holderVBO);
    glDeleteVertexArrays(1, &mesh.holderVAO);
    glDeleteBuffers(1, &mesh.snowVBO);
    glDeleteVertexArrays(1, &mesh.snowVAO);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int texWidth, texHeight, channels;
    unsigned char* image = SOIL_load_image(filename, &texWidth, &texHeight, &channels, SOIL_LOAD_RGB);


    if (image)
    {
        flipImageVertically(image, texWidth, texHeight, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        //stbi_image_free(image);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
