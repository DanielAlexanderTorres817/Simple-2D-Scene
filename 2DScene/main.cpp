/**
* Author: Daniel Torres
* Assignment: Simple 2D Scene
* Date due: 2023-09-20, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'



#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"               // We'll talk about these later in the course
#include "stb_image.h"

enum AppStatus { RUNNING, TERMINATED };

// Our window dimensions
constexpr int WINDOW_WIDTH = 640 * 1.5,     //should be times 2.5
WINDOW_HEIGHT = 480 * 1.5;                  //same as above

// Background color components
constexpr float BG_RED = 0.0f,
BG_BLUE = 0.0f,
BG_GREEN = 0.0f,
BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// Our shader filepaths; these are necessary for a number of things
// Not least, to actually draw our shapes 
// We'll have a whole lecture on these later
constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//loading the textures for the objects
constexpr char EARTH_SPRITE[] = "earth_transparent.png";
constexpr char TRAVELER_SPRITE[] = "destiny_traveler_transparent.png";
constexpr char SHIP_SPRITE[] = "ship.png";
constexpr char BG_SPRITE[] = "spacebg.png";
constexpr char GHOST_SPRITE[] = "ghost_transparent.png";

//"animated" background here
constexpr char BG1_SPRITE[] = "bg1.png";
constexpr char BG2_SPRITE[] = "bg2.png";
constexpr char BG3_SPRITE[] = "bg3.png";
constexpr char BG4_SPRITE[] = "bg4.png";
constexpr char BG5_SPRITE[] = "bg5.png";
constexpr char BG6_SPRITE[] = "bg6.png";
constexpr char BG7_SPRITE[] = "bg7.png";
constexpr char BG8_SPRITE[] = "bg8.png";



constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1, // to be generated, that is
LEVEL_OF_DETAIL = 0, // mipmap reduction image level
TEXTURE_BORDER = 0; // this value MUST be zero

//texture IDs for the objects here
GLuint g_earth_texture_id;
GLuint g_traveler_texture_id;
GLuint g_ship_texture_id;
GLuint g_bg_texture_id;
GLuint g_ghost_texture_id;

//loading all the texture IDs for the pngs for the animations
GLuint g_bg1_texture_id;
GLuint g_bg2_texture_id;
GLuint g_bg3_texture_id;
GLuint g_bg4_texture_id;
GLuint g_bg5_texture_id;
GLuint g_bg6_texture_id;
GLuint g_bg7_texture_id;
GLuint g_bg8_texture_id;

// Array to hold all 8 texture IDs
GLuint g_bg_texture_ids[8]; 
//for switching
int g_background_idx = 0;
float g_bg_switch_time = 0.15f;
float g_prev_switch_time = 0.0f;


// Our object's fill colour
constexpr float TRIANGLE_RED = 1.0,
TRIANGLE_BLUE = 0.4,
TRIANGLE_GREEN = 0.4,
TRIANGLE_OPACITY = 1.0;

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;

ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,        // Defines the position (location and orientation) of the camera
g_earth_matrix,       // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
g_projection_matrix,  // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.
g_traveler_matrix,  //second obj model matrix
g_ship_matrix,     //third object matrix
g_bg_matrix,     //matrix for the background image(s)
g_ghost_matrix;


// for delta time: 
float g_previous_ticks = 0.0f;



//for keeping track of the travelers models orbit
float g_traveler_angle = 0.0f;            // angle  for the orbit
float g_traveler_orbit_radius = 2.3f;    // distance from the Earth to the Traveler
float g_traveler_orbit_speed = 7.5f;    // how fast the Traveler rotates around the Earth


//vector to hold the travelers position
glm::vec3 g_traveler_pos_vec = glm::vec3(0.0f, 0.0f, 0.0f);



//vector for keeping track of the earth obj rotation
glm::vec3 g_rotation_earth_vec = glm::vec3(0.0f, 0.0f, 1.0f);



//vector and variables for the ship movement
glm::vec3 g_ship_pos_vec = glm::vec3(0.0f, 0.0f, 0.0f);  // Start on the far left
float g_ship_speed = 1.0f;
float g_ship_angle = 0.0f;  
float g_ellipse_x_radius = 6.0f;  
float g_ellipse_z_radius = 1.0f;   //trying to simulate depth

//render shenanigans
enum renderStatus { SHIP, GHOST };
renderStatus g_render_status = GHOST;







//load texture implementation
GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Simple 2D Scene",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    if (g_display_window == nullptr)
    {
        std::cerr << "ERROR: SDL Window could not be created.\n";
        g_app_status = TERMINATED;

        SDL_Quit();
        exit(1);
    }

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // Load the shaders for handling textures
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    // loading object images here
    g_earth_texture_id = load_texture(EARTH_SPRITE);
    g_traveler_texture_id = load_texture(TRAVELER_SPRITE);
    g_ship_texture_id = load_texture(SHIP_SPRITE);
    //g_bg_texture_id = load_texture(BG_SPRITE);
    g_ghost_texture_id = load_texture(GHOST_SPRITE);

    //now loading the animated pngs 
    g_bg_texture_ids[0] = load_texture(BG1_SPRITE);
    g_bg_texture_ids[1] = load_texture(BG2_SPRITE);
    g_bg_texture_ids[2] = load_texture(BG3_SPRITE);
    g_bg_texture_ids[3] = load_texture(BG4_SPRITE);
    g_bg_texture_ids[4] = load_texture(BG5_SPRITE);
    g_bg_texture_ids[5] = load_texture(BG6_SPRITE);
    g_bg_texture_ids[6] = load_texture(BG7_SPRITE);
    g_bg_texture_ids[7] = load_texture(BG8_SPRITE);
    


    // Initialise our camera
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    // Load up our shaders
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    // Initialise our view, model, and projection matrices
    g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_earth_matrix = glm::mat4(1.0f);  // earth model matrix
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    g_traveler_matrix = glm::mat4(1.0f); //traveler object model matrix 
    g_bg_matrix = glm::mat4(1.0f);
    g_ship_matrix = glm::mat4(1.0f);
    g_ghost_matrix = glm::mat4(1.0f); //ghost object matrix




    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    // Notice we haven't set our model matrix yet!

    g_shader_program.set_colour(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);

    // Each object has its own unique ID
    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);


    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // basic transformations for the title and logo that only happen once, so I elected to leave it out of update
    //bg
    g_bg_matrix = glm::translate(g_bg_matrix, glm::vec3(0.0f, 0.0f, -1.0f));  
    g_bg_matrix = glm::scale(g_bg_matrix, glm::vec3(10.0f, 10.0f, 1.0f));      // adjust scaling to cover the screen


}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}

void update() {

    float g_ticks = (float)SDL_GetTicks() / 1000.0f;  // get the current number of g_ticks
    float g_delta_time = g_ticks - g_previous_ticks;       // the delta time is the difference from the last frame
    g_previous_ticks = g_ticks;

    //now to make the earth object rotate along the z axis
    g_rotation_earth_vec.z += 5.0 * g_delta_time;           //slow rotation, entirely intentional
    g_earth_matrix = glm::mat4(1.0f);
    g_earth_matrix = glm::translate(g_earth_matrix, glm::vec3(0.0f, 0.0f, 0.0f)); //for positioning sake, want the earth to be a little above center, so no delta time
    g_earth_matrix = glm::rotate(g_earth_matrix, glm::radians(g_rotation_earth_vec.z), glm::vec3(0.0f, 0.0f, 1.0f));
    g_earth_matrix = glm::scale(g_earth_matrix, glm::vec3(4.0f, 4.0f, 1.0f));  // Scale the Earth by 4.0 times, this is a constant thing so no delta time



    //now handling the orbit of the traveler
    //taking care of delta time and converting angle to radians
    g_traveler_angle += g_traveler_orbit_speed * g_delta_time;
    float angle_radians = glm::radians(g_traveler_angle);

    //get the x and y coordinates using cos and sin
    g_traveler_pos_vec.x = g_traveler_orbit_radius * cos(angle_radians);
    g_traveler_pos_vec.y = g_traveler_orbit_radius * sin(angle_radians);     

    g_traveler_matrix = glm::mat4(1.0f);
    g_traveler_matrix = glm::translate(g_traveler_matrix, g_traveler_pos_vec);
    g_traveler_matrix = glm::scale(g_traveler_matrix, glm::vec3(0.65f, 0.65f, 1.0f));   //just shrinking the traveler, so no delta time needed


    //ship transformations 
    g_ship_angle += g_ship_speed * g_delta_time;

    g_ship_pos_vec.x = g_ellipse_x_radius * glm::cos(g_ship_angle);  // left to right movement
    g_ship_pos_vec.z = (1.0f * 0.5f) * glm::sin(g_ship_angle);  // depth movement (closer/farther)
    

   
    g_ship_matrix = glm::mat4(1.0f);
    g_ship_matrix = glm::translate(g_ship_matrix, g_ship_pos_vec);

    
    float ship_scale_factor = 1.0f + (g_ship_pos_vec.z);
    g_ship_matrix = glm::scale(g_ship_matrix, glm::vec3(ship_scale_factor, ship_scale_factor, 0.0f));

    //if the ghost is off screen, render the ship instead (set the correct flag variables) and vice versa
    if (g_ship_pos_vec.x < -5.5f) { g_render_status = SHIP; }
    else if (g_ship_pos_vec.x > 5.5f) { g_render_status = GHOST; }


    //ghost transformations
    g_ghost_matrix = glm::mat4(1.0f);
    g_ghost_matrix = glm::scale(g_ship_matrix, glm::vec3(0.25f, 0.25f, 0.0f));

    //now taking care of the background
    g_prev_switch_time += g_delta_time;

    // check if it's time to switch the background
    if (g_prev_switch_time >= g_bg_switch_time) {
        g_background_idx += 1;
        g_background_idx %= 8;
        g_prev_switch_time = 0.0f;
    }
   





}
void draw_object(glm::mat4& g_object_model_matrix, GLuint& g_object_texture_id)
{
    g_shader_program.set_model_matrix(g_object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
        0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_bg_matrix, g_bg_texture_ids[g_background_idx]);

    //conditional rendering here
    if (g_render_status == SHIP) { draw_object(g_ship_matrix, g_ship_texture_id); }

    draw_object(g_earth_matrix, g_earth_texture_id);
    draw_object(g_traveler_matrix, g_traveler_texture_id);

    
    if (g_render_status == GHOST) { draw_object(g_ghost_matrix, g_ghost_texture_id); }
    
   
    
    
    
    

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);

}

void shutdown() { SDL_Quit(); }

/**
 Start here—we can see the general structure of a game loop without worrying too much about the details yet.
 */
int main(int argc, char* argv[])
{
    // Initialise our program—whatever that means
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();  // If the player did anything—press a button, move the joystick—process it
        update();         // Using the game's previous state, and whatever new input we have, update the game's state
        render();         // Once updated, render those changes onto the screen
    }

    shutdown();  // The game is over, so let's perform any shutdown protocols
    return 0;
}