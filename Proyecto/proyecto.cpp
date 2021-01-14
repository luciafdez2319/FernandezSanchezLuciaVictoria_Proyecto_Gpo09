// Std. Includes
#include <string>
#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );
void animacion();

// Camera
Camera camera( glm::vec3( 0.0f, 0.0f, 3.0f ) );
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool active = true;
bool som;
int estado = 1;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//Variables para las animaciones, aquí tiene cada una de ellas los valores iniciales y que después podrían modificarse
float movAlX = -6.5f;
float movAlY = 3.0f;
float movAlZ = -7.5f;
float movPuY = 0.0;
float movAl = 0.0;
float movSilla = 2.0;
float sillaA = 165.0;
float rotPuerta = 0.0;

float movD = 0.0;
float movSillaD = 2.0f;
float sillaD = 0.0;


//Keyframes Variables que se utilizan para las animaciones complejas
float posSomY = 8.0f;
float posSomZ = -5.0f;
float rotSomX = 0.0f;
float rotSomY = 0.0f;

float posFloY = 3.5f;
float rotFloY = 0.0f;
float rotFloXZ = 0.0f;

//Variables para realizar correctamente el uso de las animaciones y solamente se realicen las que el usuario desee
#define MAX_FRAMES 2 // Solamente puede tener dos posibles estados
int i_max_steps = 100; //Variable utilizada para el sombrero
int i_max_steps2 = 500; //Variable utilizada para el florero
int i_curr_steps = 0;
bool playS = false;
bool playF = false;




typedef struct _frame
{ 

    //Variables para GUARDAR Key Frames


    //Sombrero
    float posSY;		
    float posSZ;
    float rotSX;
    float rotSY;

    float posIncSY;
    float posIncSZ;
    float rotIncSX;
    float rotIncSY;

    //Florero
    float posFY;
    float posIncFY;
    float rotFY;
    float rotIncFY;
    float rotFXZ;
    float rotIncFXZ;

}FRAME;


FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;	
int playIndex = 0;

/*Función que se encarga de guardar las posiciones, es por eso que el arreglo de los keyframes se encuentra en 0 y en 1, en 0 
Unicamente se guardan las posiciones iniciales mientras que en 1 se guardan las posiciones modificadas. */
void saveFrame(void)
{
 
    KeyFrame[0].posSY = posSomY;
    KeyFrame[0].posSZ = posSomZ;
    KeyFrame[0].rotSX = rotSomX;
    KeyFrame[0].rotSY= rotSomY;

    KeyFrame[1].posSY = 3.8;
    KeyFrame[1].posSZ = -4.4f;
    KeyFrame[1].rotSX = 45.0f;
    KeyFrame[1].rotSY = -180.0f;

    KeyFrame[0].posFY = posFloY;
    KeyFrame[0].rotFY = rotFloY;
    KeyFrame[0].rotFXZ = rotFloXZ;

    KeyFrame[1].posFY = 6.0f;  //6 
    KeyFrame[1].rotFY = -90.0f;  //-90
    KeyFrame[1].rotFXZ = -40.0f; //-40

}

/*Función que se realizar la interpolación de las animaciones con Keyframes*/
void interpolation(void)
{
    //Sombrero
   KeyFrame[playIndex].posIncSY = (KeyFrame[playIndex + 1].posSY - KeyFrame[playIndex].posSY) / i_max_steps;
   KeyFrame[playIndex].posIncSZ = (KeyFrame[playIndex + 1].posSZ - KeyFrame[playIndex].posSZ) / i_max_steps;
   KeyFrame[playIndex].rotIncSX = (KeyFrame[playIndex + 1].rotSX - KeyFrame[playIndex].rotSX) / i_max_steps;
   KeyFrame[playIndex].rotIncSY = (KeyFrame[playIndex + 1].rotSY - KeyFrame[playIndex].rotSY) / i_max_steps;

   //Florero
   KeyFrame[playIndex].posIncFY = (KeyFrame[playIndex + 1].posFY - KeyFrame[playIndex].posFY) / i_max_steps2;
   KeyFrame[playIndex].rotIncFY = (KeyFrame[playIndex + 1].rotFY - KeyFrame[playIndex].rotFY) / i_max_steps2;
   KeyFrame[playIndex].rotIncFXZ = (KeyFrame[playIndex + 1].rotFXZ - KeyFrame[playIndex].rotFXZ) / i_max_steps2;


}

/*Función que se encarga de reestablecer los valores iniciales a los diferentes objetos con el fin de que su posición o rotación 
  no se encuentren editadas.*/
void resetElements(void)
{
    //Variable auxiliar que cambia exclusivamente la posición ya sea del sombrero (manual) o florero (automática), dependiendo
    //De las instrucciones del usuario
    if (som) {

        posSomY = KeyFrame[0].posSY;
        posSomZ = KeyFrame[0].posSZ;
        rotSomX = KeyFrame[0].rotSX;
        rotSomY = KeyFrame[0].rotSY;
    }
    else {

        posFloY = KeyFrame[0].posFY;
        rotFloY = KeyFrame[0].rotFY;
        rotFloXZ = KeyFrame[0].rotFXZ;
    }
    

}

/*Funcipón principal que se encarga de cargar las animaciones, luces, objetos entre otros con el fin de que se encuentren 
  listas para que sean utilizadas si es que se requiere.*/
int main( )
{
    // Init GLFW
    glfwInit( );
    
    

    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "FernandezSanchezLuciaVictoria_Proyecto_Gpo09", nullptr, nullptr );

    //La siguiente función se encarga de tener una mejor visualización de la cámara y por ende, mejor manejo de la misma.
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    //glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup and compile our shaders
    Shader shader( "Shaders/modelLoading.vs", "Shaders/modelLoading.frag" );
    
    // Load models PASO 1
    Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
    
    // Load textures Carga del SkyBox
    vector<const GLchar*> faces;
    faces.push_back("SkyBox/right.tga");
    faces.push_back("SkyBox/left.tga");
    faces.push_back("SkyBox/top.tga");
    faces.push_back("SkyBox/bottom.tga");
    faces.push_back("SkyBox/back.tga");
    faces.push_back("SkyBox/front.tga");

    GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

    //Carga de los modelos previamente diseñados
    Model silla((char*)"Models/Sillas/Silla_Atras.obj");
    Model mesa((char*)"Models/Mesa/mesa.obj");
    Model cuarto((char*)"Models/Casa/cuarto.obj");

    Model puerta((char*)"Models/Puerta/Puerta.obj");
    //Model puerta((char*)"Models/Door/Door.obj");

    Model florero((char*)"Models/Florero/Vase.obj");
    Model plato((char*)"Models/Plato/Plate.obj");
    Model cama((char*)"Models/Cama/Bed.obj");
    Model almohadan((char*)"Models/Almohada/Pillow.obj");
    Model perchero((char*)"Models/Perchero/Hook.obj");
    Model almohada((char*)"Models/Almohada/Pillow1.obj");
    Model sombrero((char*)"Models/Sombrero/Hat.obj");
    Model piso((char*)"Models/Casa/Piso/Piso.obj");

    //Inicialización
    for (int i = 0; i < MAX_FRAMES; i++)
    {
        //Variables del Sombrero
        KeyFrame[i].posSY = 0;
        KeyFrame[i].posIncSY = 0;
        KeyFrame[i].posSZ = 0;
        KeyFrame[i].posIncSZ = 0;
        KeyFrame[i].rotSX = 0;
        KeyFrame[i].rotIncSX = 0;
        KeyFrame[i].rotSY = 0;
        KeyFrame[i].rotIncSY = 0;

        //Variables del Florero
        KeyFrame[i].posFY = 0;
        KeyFrame[i].posIncFY = 0;
        KeyFrame[i].rotFY = 0;
        KeyFrame[i].rotIncFY = 0;
        KeyFrame[i].rotFXZ = 0;
        KeyFrame[i].rotIncFXZ = 0;

    }
    
    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    

     saveFrame();



    Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
    Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");

    GLfloat vertices[] =
    {
        // Positions            // Normals              // Texture Coords
            
        -10.0f, 11.0f-1.0f, -7.0f-2.0f,    0.0f, 0.0f,-1.0f,		0.0f,0.0f,
        -10.0f, 11.0f - 1.0f,-7.0f + 2.0f,	   0.0f, 0.0f,-1.0f,		1.0f,0.0f,
        -10.0f,  11.0f + 1.0f, -7.0f + 2.0f,     0.0f, 0.0f,-1.0f,		1.0f,1.0f,
        -10.0f, 11.0f + 1.0f, -7.0f - 2.0f,    0.0f, 0.0f,-1.0f,		0.0f,1.0f,



    };

    GLuint indices[] =
    {  
        // Note that we start from 0!
        0,1,3,
        1,2,3,
      


    };

    // Draw in wireframe
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Texture Coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // Load textures
    GLuint texture1;
    glGenTextures(1, &texture1);

    int textureWidth, textureHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // Diffuse map
    image = stbi_load("images/proyecto.jpg", &textureWidth, &textureHeight, &nrChannels, 0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(image);

    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);

    // Bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);


    // SkyBox
    GLuint skyboxVBO, skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    
    // Game loop
    while( !glfwWindowShouldClose( window ) )
    {

        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents( );
        DoMovement( );
        animacion();
        
        // Clear the colorbuffer
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        shader.Use( );
        
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );        

        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 0.5f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glBindVertexArray(VAO);
        // Draw the loaded model
        glm::mat4 model(1);
        //model = glm::translate( model, glm::vec3( 0.0f, 5.0f, 0.0f ) ); 
		glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
	

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
       

        //Silla Atrás
        model = glm::mat4(1); 
        model = glm::translate(model, glm::vec3(-7.0f, 1.80f, movSilla));
        model = glm::rotate(model, glm::radians(sillaA), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        silla.Draw(shader);


        //Silla Delante
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(movSillaD, 1.80f, 8.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        silla.Draw(shader);


        //Mesa
        model = glm::mat4(1);
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        mesa.Draw(shader);

        //Base del cuarto
        model = glm::mat4(1);
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        cuarto.Draw(shader);

        //Piso para tener mejor perspectiva
        model = glm::mat4(1);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        piso.Draw(shader);

        //Puerta, contiene animación sencilla
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(10.1f, 0.0f, -2.3f));
        model = glm::rotate(model, glm::radians(movPuY), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        puerta.Draw(shader);
        
        //Florero, contiene animación compleja
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-6.74f, posFloY, 6.2f));
        model = glm::rotate(model, glm::radians(rotFloY), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotFloXZ), glm::vec3(1.0f, 0.0f, 1.0f));
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        florero.Draw(shader);

        //Plato
        model = glm::mat4(1);
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        plato.Draw(shader);

        //Cama
        model = glm::mat4(1);
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        cama.Draw(shader);

        //Almohada
        model = glm::mat4(1);
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        almohadan.Draw(shader);

        //Perchero
        model = glm::mat4(1);
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
        perchero.Draw(shader);

        //Almohada, contiene animación sencilla
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(movAlX, movAlY, movAlZ));
        model = glm::rotate(model, glm::radians(movAl), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        almohada.Draw(shader);

        //Sombrero, contiene animación compleja
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-8.5f, posSomY, posSomZ));
        model = glm::rotate(model, glm::radians(rotSomX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotSomY), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sombrero.Draw(shader);

        
        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
        SkyBoxshader.Use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
        glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default


        // Swap the buffers
        glfwSwapBuffers( window );
    }
    
    glDeleteVertexArrays(1, &skyboxVAO);

    glfwTerminate( );
    return 0;
}

/*Modifica la cámara, también se encarga de recibir los cambios en las teclas con el fin de realizar las animaciones. 
  Dichos modificadores dependen de las teclas apretadas.*/

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if ( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if ( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }

    //Puerta

    if (keys[GLFW_KEY_1])
    {

        while (movPuY != 90 && movPuY < 90) {
            movPuY += 0.1f;
        }
        

    }

    if (keys[GLFW_KEY_2])
    {
        while (movPuY != 0 && movPuY > 0) {
            movPuY -= 0.1f;
        }
           
    }
   
}

/*Función que se encarga de reproducir las animaciones complejas, es decir, con keyframes*/
void animacion()
{
 
    if (playS) {

        if (i_curr_steps >= i_max_steps) //end of animation between frames?
        {
            playIndex++;
            if (playIndex > FrameIndex - 2)	//end of total animation?
            {
                
                playIndex = 0;
                playS = false;

            }
            else //Next frame interpolations
            {
                i_curr_steps = 0; //Reset counter
                                  //Interpolation
                interpolation();
            }
        }
        else
        {
            //Draw animation

            posSomY += KeyFrame[playIndex].posIncSY;
            posSomZ += KeyFrame[playIndex].posIncSZ;

            rotSomX += KeyFrame[playIndex].rotIncSX;
            rotSomY += KeyFrame[playIndex].rotIncSY;

            i_curr_steps++;
        }  
        
    } 

    //Florero
    if (playF) {

        if (i_curr_steps >= i_max_steps2) //end of animation between frames?
        {
            playIndex++;
            if (playIndex > FrameIndex - 2)	//end of total animation?
            {
                resetElements();
                playIndex = 0;
                playF = false;
                //resetElements();
            }
            else //Next frame interpolations
            {
                i_curr_steps = 0; //Reset counter
                                  //Interpolation
                interpolation();
            }
        }
        else
        {
            //Draw animation

            posFloY += KeyFrame[playIndex].posIncFY;
            rotFloY += KeyFrame[playIndex].rotIncFY;
            rotFloXZ += KeyFrame[playIndex].rotIncFXZ;

            i_curr_steps++;
        }
       
    }
}

/*Se encarga de recibir los cambios en las teclas con el fin de realizar las animaciones.
  Dichos modificadores dependen de las teclas apretadas.*/
// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if ( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }

    //Almohada
    if (keys[GLFW_KEY_Y])
    {
            while (movAlZ <= -5.0) {
                movAlZ += 0.1f;
            }
   
    }

    if (keys[GLFW_KEY_U])
    {
        while(movAlZ >= -7.5) {
            movAlZ -= 0.1f;
        }
            
    }

    //Silla 
 
    if (keys[GLFW_KEY_3])
    {
 
        while(sillaA >= 4) {

             sillaA -= 1.0f;
         }

        while(movSilla <= 3.0) {
              movSilla += 0.1f;
        }

        while(movSillaD >= -5.8) {
            movSillaD -= 0.1f;
        }

    }

    //Silla de nuevo

    if (keys[GLFW_KEY_4])
    {

        while(sillaA <= 165) {
           sillaA += 1.0f;
         }


        while(movSilla >= 2) {
             movSilla -= 1.0f;
         }

        while (movSillaD <= 2.0) {
            movSillaD += 0.1f;
        }

    }

    //Sombrero
    if (keys[GLFW_KEY_L])
    {
        if (playS == false ) 
        {
            som = true;
            resetElements();
            //First Interpolation				
            interpolation();

            playS = true;
            playIndex = 0;
            i_curr_steps = 0;
        }
        else
        {
            playS = false;
        }

    }

    if (keys[GLFW_KEY_K])
    {
        if (playF == false)
        {
            som = false;
            resetElements();
            //First Interpolation				
            interpolation();


            playF = true;
            playIndex = 0;
            i_curr_steps = 0;

           
        }
        else
        {
            playF = false;
           
          
        }

    }
 
    
    if (keys[GLFW_KEY_O])
    {
      
        som = true;
        resetElements();


    }

}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if ( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}

