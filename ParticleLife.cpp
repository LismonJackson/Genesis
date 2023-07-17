#include "raylib.h"
#include <math.h>
#include <cstdio>
#include <thread>




// Convert integer to character string
char s1[20];
char* get_Str(int i) {
    
    sprintf(s1, "%i", i);
    return s1;
}

float getRandomFloat(float min, float max) {
    int beforeFloatingPoint = GetRandomValue(static_cast<int>(min), static_cast<int>(max));
    int afterFloatingPoint;
    if ((beforeFloatingPoint < static_cast<int>(max)) || beforeFloatingPoint > static_cast<int>(min)) {
        afterFloatingPoint == 0;
    } else {
        afterFloatingPoint = GetRandomValue(1, 99);
    }
    float randomFloat = static_cast<float>(beforeFloatingPoint) + static_cast<float>(afterFloatingPoint) / 100.0f;
    return randomFloat;
}



// GLOBAL VARIABLES

// height and width settings
const float screenWidth = 1920;
const float screenHeight = 1080;
const int sliderWidth = 190;
const int sliderHeight = 20;
int boundryWidthLeft = 220;

// resistanace to deformation:
float viscosity = 0.98f; // Could range from 0 to 1

// force Radius for each particle:
const float radius = 200; // Could range from 1 to 500 - This is the radius in which

// force lines visibility:
bool showLines = false;

// total Number of particles for each group:
int num_of_particles = 700;

// radius or size of particle
const int particleSize = 1;

// check if screen is maximized
bool is_fullscreen = false;

// g values for particle groups - also, intial slider values
float yy = 0.0f;
float yr = 0.0f;
float yb = 0.0f;
float yg = 0.0f;

float by = 0.0f;
float br = 0.0f;
float bb = 0.0f;
float bg = 0.0f;

float gy = 0.0f;
float gr = 0.0f;
float gb = 0.0f;
float gg = 0.0f;

float ry = 0.0f;
float rr = 0.0f;
float rb = 0.0f;
float rg = 0.0f;


// Camera Zoom speed - in/out with the mouse wheel
float zoomSpeed = 0.1f;

// Camera movement speed - with the arrow keys
float cameraSpeed = 25.0f;

// GUI ELEMENTS

// slider element
typedef struct Slider {
    // Existing variables
    int rect1_width;
    int rect1_height;
    int rect2_width;
    int rect2_height;
    int rect3_width;
    int rect3_height;
    float percentage;
    int pos_x;
    int pos_y;
    char text[10]; // Increase the size to hold negative numbers and decimal points
    bool is_focused;
    int mouseX;
    int mouseY;
    float *iterator;

    // Additional variables for range
    float min_value;
    float max_value;

    Slider(int x_pos, int y_pos, int width, int height, float *iterPtr, float minVal, float maxVal) {
        // Initialize existing variables
        this->rect1_width = width;
        this->rect1_height = height;
        this->rect2_width = width - 6;
        this->rect2_height = height - 2;
        this->rect3_width = 2;
        this->rect3_height = rect2_height;
        this->pos_x = x_pos;
        this->pos_y = y_pos;
        this->percentage = 0.0f;
        this->iterator = iterPtr;

        // Initialize additional variables
        this->min_value = minVal;
        this->max_value = maxVal;

        // Convert float to char array with 2 decimal places
        snprintf(text, sizeof(text), "%.2f", percentage);

        Draw_Slider(x_pos, y_pos);
    }

    void Draw_Slider(int x, int y) {
        this->percentage = *(this->iterator);
        float temp = ((percentage - min_value) / (max_value - min_value)) * rect1_width;
        this->rect2_width = temp - 6;
        this->pos_x = x;
        this->pos_y = y;

        snprintf(text, sizeof(text), "%.2f", *(this->iterator));

        DrawRectangle(x, y, rect1_width, rect1_height, WHITE);
        DrawRectangle(x + 3, y + 1, rect2_width, rect2_height, ORANGE);

        DrawText(text, x + rect1_width / 2 - 20, y + rect1_height / 2 - 10, 20, BLACK);
    }

    void Activate_Slider() {
        this->mouseX = GetMouseX();
        this->mouseY = GetMouseY();
        if (IsMouseButtonDown(0))
            if ((mouseX > pos_x) && (mouseY > pos_y) && (mouseX < (pos_x + rect1_width)) && (mouseY < (pos_y + rect1_height))) {
                float temp = ((float)(mouseX - pos_x - 3) / (float)(rect1_width - 6)) * (max_value - min_value) + min_value;
                if (temp < min_value) temp = min_value; // Clamp value to min
                if (temp > max_value) temp = max_value; // Clamp value to max
                Update_Slider(temp);
            }
    }

    void Update_Slider(float per) {
        *(this->iterator) = per;
        float temp = ((*(this->iterator) - min_value) / (max_value - min_value)) * rect1_width;
        this->rect2_width = temp - 6;
        this->pos_x = pos_x;
        this->pos_y = pos_y;
        this->percentage = *(this->iterator);

        snprintf(text, sizeof(text), "%.2f", percentage);

        DrawRectangle(pos_x, pos_y, rect1_width, rect1_height, WHITE);
        DrawRectangle(pos_x + 3, pos_y + 1, rect2_width, rect2_height, ORANGE);
        DrawRectangle(pos_x + rect2_width + 3, pos_y + 1, rect3_width, rect3_height, BLACK);

        DrawText(text, pos_x + rect1_width / 2 - 20, pos_y + rect1_height / 2 - 10, 20, BLACK);
    }
} Slider;


// button element
typedef struct Button {
    int border_width = 2;
    Color button_color = LIGHTGRAY;
    Color border_color = BLACK;
    Color text_color = BLACK;
    int text_size = 20;
    int text_padding = 10; // New field to store text padding value
    char* text = "Button";
    int x_pos;
    int y_pos;
    int b_width;
    int b_height;
    float mouseX;
    float mouseY;
    int text_width;
    int text_x;
    int text_y;
    bool button_state = false;
    

    // Constructor with custom border color, button color, text color, text size, and text padding
   Button(char* text, int x, int y, Color border_color, Color button_color, Color text_color, int text_size, int text_padding) {
    this->border_width = 2;
    this->border_color = border_color;
    this->button_color = button_color;
    this->text_color = text_color;
    this->text_size = text_size;
    this->text_padding = text_padding;
    this->text = text;
    this->x_pos = x;
    this->y_pos = y;

    // Calculate button width and height based on text dimensions
    this->text_width = MeasureText(text, text_size);
    this->b_width = text_width + 2 * border_width + 2 * text_padding;
    this->b_height = text_size + 2 * border_width + 2 * text_padding;

    // Center text horizontally and vertically, with added padding
    this->text_x = x + b_width/2 - text_width/2;
    this->text_y = y + b_height/2 - text_size/2 - text_padding/2;
    
    }
    
    
    void draw_Button() {
        // Draw button border, body and text
        DrawRectangle(x_pos, y_pos, b_width, b_height, border_color);
        DrawRectangle(x_pos + border_width, y_pos + border_width, b_width - 2*border_width, b_height - 2*border_width, button_color);
        DrawText(text, text_x, text_y, text_size, text_color);
    }
    
    // Check For button state
    bool is_Clicked() {
        this->mouseX = GetMouseX();
        this->mouseY = GetMouseY();
        bool mousePressedDown = IsMouseButtonPressed(0);
        if ((mouseX > x_pos) && (mouseX < x_pos+b_width) && (mouseY > y_pos-text_padding-border_width) && (mouseY < y_pos+b_height-text_padding-border_width)) {
            DrawRectangle(x_pos, y_pos, b_width, b_height, ORANGE);
            DrawRectangle(x_pos + border_width, y_pos + border_width, b_width - 2*border_width, b_height - 2*border_width, GRAY);
            DrawText(text, text_x, text_y, text_size, WHITE);
            if (mousePressedDown) {
                // DrawText("+", 10, 10, 50, WHITE);
                button_state = true;
            } else {
                // DrawText("-", 10, 10, 50, WHITE);
                button_state = false;
            }
            
        }
        
        return button_state;
    }
    
} Button;



// Main particle struct
typedef struct Particle {
    float x; // x position
    float y; // y position
    float r; // Radius
    Color c;; // color
    float vx; // Velocity x 
    float vy; // Velocity y
    float fd; // Field view or radius at which the force starts acting 
} Particle;


// SIMULATION RELATED FUNCTIONS

void CreateParticles(Particle particles[], int num_of_particles, int radius, int screenWidth, int screenHeight, Color color)
{
    for (int i = 0; i < num_of_particles; i++) {
        // Assign posx, posy, radius and color to each particle. Radius is subtracted and added to avoid boundary overflow.
        particles[i] = {GetRandomValue(0 + radius, screenWidth - radius), GetRandomValue(0 + radius, screenHeight - radius), radius, color};
    }
}

void Draw_And_Update_Particles(Particle particles[], int num_of_particles, int radius) {
    for (int i = 0; i < num_of_particles; i++) {
        particles[i].r = radius; // Updating Radius 
        DrawCircle(particles[i].x, particles[i].y, particles[i].r, particles[i].c); 
    }
}

void Draw_And_Update_Particles(Particle particles[], int num_of_particles) {
    for (int i = 0; i < num_of_particles; i++) {
        DrawCircle(particles[i].x, particles[i].y, particles[i].r, particles[i].c); 
    }
}


void rule(Particle group1[], Particle group2[], float g, int num_of_particles) {
    for (int i = 0; i < num_of_particles; i++) {
        Particle a = group1[i];
        float fx = 0;
        float fy = 0;
        
        // Calculate forces between particles
        for (int j = 0; j < num_of_particles; j++) {
            Particle b = group2[j];
            
            // Calculate the distance between particles squared
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            float distSq = dx * dx + dy * dy;
            
            // Apply force if particles are within the radius
            if (distSq != 0 && distSq < (radius * radius)) {
                float invDist = 1.0 / sqrtf(distSq);
                fx += dx * invDist;
                fy += dy * invDist;
            }
            
            
            if (showLines && (distSq <= (radius * radius))) {
            Color particleColor = a.c;
            DrawLine(a.x, a.y, b.x, b.y, particleColor);
            DrawLine(b.x, b.y, a.x, a.y, particleColor);

            }      

            
        }
        
        // Update velocity using the calculated forces
        a.vx = (a.vx + fx * g) * (1.0 - viscosity);
        a.vy = (a.vy + fy * g) * (1.0 - viscosity);
        
        // Update position based on velocity
        a.x += a.vx;
        a.y += a.vy;
        
        // Handle boundary conditions
        if (a.x < 0 || a.x > screenWidth - boundryWidthLeft || a.y < 0 || a.y > screenHeight) {
            a.vx = -a.vx;
            a.x += a.vx;
            a.vy = -a.vy;
            a.y += a.vy;
        }
        
        
        // Update the particle in group1 array
        group1[i] = a;
    }
}


void randomizeParticleForces() {
    yy = getRandomFloat(-10.0f, 10.0f);
    yr = getRandomFloat(-10.0f, 10.0f);
    yb = getRandomFloat(-10.0f, 10.0f);
    yg = getRandomFloat(-10.0f, 10.0f);

    by = getRandomFloat(-10.0f, 10.0f);
    br = getRandomFloat(-10.0f, 10.0f);
    bb = getRandomFloat(-10.0f, 10.0f);
    bg = getRandomFloat(-10.0f, 10.0f);

    gy = getRandomFloat(-10.0f, 10.0f);
    gr = getRandomFloat(-10.0f, 10.0f);
    gb = getRandomFloat(-10.0f, 10.0f);
    gg = getRandomFloat(-10.0f, 10.0f);

    ry = getRandomFloat(-10.0f, 10.0f);
    rr = getRandomFloat(-10.0f, 10.0f);
    rb = getRandomFloat(-10.0f, 10.0f);
    rg = getRandomFloat(-10.0f, 10.0f);
}



int main(void) {
    // Initialization
    InitWindow(screenWidth, screenHeight, "Particle Life");
    
    // Declare the camera settings
    Camera2D camera;
    camera.target = { 0, 0 };
    camera.offset = { 1, 1 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    const float maxZoom = 2.0f;
    const float minZoom = 1.0f;
    const Rectangle cameraBounds = { 0, 0, screenWidth * 2, screenHeight * 2 };
    
    // Target FPS
    SetTargetFPS(60);

    // Particle types
    Particle yellow[num_of_particles];
    Particle red[num_of_particles];
    Particle green[num_of_particles];
    Particle blue[num_of_particles];
    
    // Create Particles and assign them random positions.
    CreateParticles(yellow, num_of_particles, particleSize, screenWidth - boundryWidthLeft, screenHeight, ORANGE);
    CreateParticles(red, num_of_particles, particleSize, screenWidth - boundryWidthLeft, screenHeight, RED);
    CreateParticles(green, num_of_particles, particleSize, screenWidth - boundryWidthLeft, screenHeight, GREEN);
    CreateParticles(blue, num_of_particles, particleSize, screenWidth - boundryWidthLeft, screenHeight, BLUE);
     
    
    // Create an instance of the Slider for each of the above group
    Slider slider_yy(screenWidth-200, 60, 190, 18, &yy, -10.00f, 10.00f);
    Slider slider_yr(screenWidth-200, 100, 190, 18, &yr, -10.00f, 10.00f);
    Slider slider_yb(screenWidth-200, 140, 190, 18, &yb, -10.00f, 10.00f);
    Slider slider_yg(screenWidth-200, 180, 190, 18, &yg, -10.00f, 10.00f);
    
    Slider slider_by(screenWidth-200, 240, 190, 18, &by, -10.00f, 10.00f);
    Slider slider_br(screenWidth-200, 280, 190, 18, &br, -10.00f, 10.00f);
    Slider slider_bb(screenWidth-200, 320, 190, 18, &bb, -10.00f, 10.00f);
    Slider slider_bg(screenWidth-200, 360, 190, 18, &bg, -10.00f, 10.00f);

    Slider slider_gy(screenWidth-200, 420, 190, 18, &gy, -10.00f, 10.00f);
    Slider slider_gr(screenWidth-200, 460, 190, 18, &gr, -10.00f, 10.00f);
    Slider slider_gb(screenWidth-200, 500, 190, 18, &gb, -10.00f, 10.00f);
    Slider slider_gg(screenWidth-200, 540, 190, 18, &gg, -10.00f, 10.00f);

    Slider slider_ry(screenWidth-200, 600, 190, 18, &ry, -10.00f, 10.00f);
    Slider slider_rr(screenWidth-200, 640, 190, 18, &rr, -10.00f, 10.00f);
    Slider slider_rb(screenWidth-200, 680, 190, 18, &rb, -10.00f, 10.00f);
    Slider slider_rg(screenWidth-200, 720, 190, 18, &rg, -10.00f, 10.00f);
    
    // Create an instance of the viscosity Slider
    Slider slider_viscosity(screenWidth-200, 800, 190, 18, &viscosity, 0.00f, 1.00f);
    
    // creating instance of  buttons 
    Button quitButton = Button("quit", screenWidth-200, screenHeight-100, BLACK, ORANGE, BLACK, 20, 4);
    Button showLinesButton = Button("show lines", screenWidth-200, screenHeight-140, BLACK, BLUE, WHITE, 20, 4);
    Button randomizeButton = Button("randomize", screenWidth-200, screenHeight-180, BLACK, PURPLE, WHITE, 20, 4);
    
    
    while (!WindowShouldClose())
    {
              
        // Camera Functions
        
        if (IsKeyDown(KEY_R)) // Reset camera
        {
            camera.target = { 0, 0 };
            camera.zoom = 1.0f;
        }


        camera.zoom += (GetMouseWheelMove() * zoomSpeed);
        if (camera.zoom > maxZoom)
            camera.zoom = maxZoom;
        else if (camera.zoom < minZoom)
            camera.zoom = minZoom;

        // Move the camera with arrow keys
        
        if (IsKeyDown(KEY_RIGHT))
            camera.target.x += cameraSpeed;
        if (IsKeyDown(KEY_LEFT))
            camera.target.x -= cameraSpeed;
        if (IsKeyDown(KEY_DOWN))
            camera.target.y += cameraSpeed;
        if (IsKeyDown(KEY_UP))
            camera.target.y -= cameraSpeed;
        
        if (camera.target.x < cameraBounds.x)
            camera.target.x = cameraBounds.x;
        else if (camera.target.x > cameraBounds.width - screenWidth)
            camera.target.x = cameraBounds.width - screenWidth;
        if (camera.target.y < cameraBounds.y)
            camera.target.y = cameraBounds.y;
        else if (camera.target.y > cameraBounds.height - screenHeight)
            camera.target.y = cameraBounds.height - screenHeight;
        
        
        // Rules    
        // Rules for yello
        rule(yellow, yellow, yy, num_of_particles);
        rule(yellow, red, yr, num_of_particles);
        rule(yellow, blue, yb, num_of_particles);
        rule(yellow, green, yg, num_of_particles);
        // Rules for blue
        rule(blue, yellow, by, num_of_particles);
        rule(blue, red, br, num_of_particles);
        rule(blue, blue, bb, num_of_particles);
        rule(blue, green, bg, num_of_particles);
        // Rules for green
        rule(green, yellow, gy, num_of_particles);
        rule(green, red, gr, num_of_particles);
        rule(green, blue, gb, num_of_particles);
        rule(green, green, gg, num_of_particles);
        // rules for red
        rule(red, yellow, ry, num_of_particles);
        rule(red, red, rr, num_of_particles);
        rule(red, blue, rb, num_of_particles);
        rule(red, green, rg, num_of_particles);
        
        // Activate Slider functionality
        slider_yy.Activate_Slider();
        slider_yr.Activate_Slider();
        slider_yb.Activate_Slider();
        slider_yg.Activate_Slider();
        
        slider_by.Activate_Slider();
        slider_br.Activate_Slider();
        slider_bb.Activate_Slider();
        slider_bg.Activate_Slider();
        
        slider_gy.Activate_Slider();
        slider_gr.Activate_Slider();
        slider_gb.Activate_Slider();
        slider_gg.Activate_Slider();
        
        slider_ry.Activate_Slider();
        slider_rr.Activate_Slider();
        slider_rb.Activate_Slider();
        slider_rg.Activate_Slider();
        
        slider_viscosity.Activate_Slider();
        
        
        if (quitButton.is_Clicked()) {
            CloseWindow();
        }
        
        
        if (is_fullscreen == false) {
            ToggleFullscreen();
            is_fullscreen = true;
        }
        
        if (showLinesButton.is_Clicked()) {
            if (showLines == true) {
                showLines = false;
            } else {
                showLines = true;
            }
        }
        
        if (randomizeButton.is_Clicked()) {
            randomizeParticleForces();
        }
        
        
        
        BeginDrawing();
        
            
        
        
            // Clear background
            ClearBackground(BLACK);
            
            // Draw Buttons
            quitButton.draw_Button();
            showLinesButton.draw_Button();
            randomizeButton.draw_Button();
            // Draw slidersand update slider GUI

            slider_yy.Draw_Slider(screenWidth-200, 60);
            slider_yr.Draw_Slider(screenWidth-200, 100);
            slider_yb.Draw_Slider(screenWidth-200, 140);
            slider_yg.Draw_Slider(screenWidth-200, 180);
                                  
            slider_by.Draw_Slider(screenWidth-200, 240);
            slider_br.Draw_Slider(screenWidth-200, 280);
            slider_bb.Draw_Slider(screenWidth-200, 320);
            slider_bg.Draw_Slider(screenWidth-200, 360);
                                  
            slider_gy.Draw_Slider(screenWidth-200, 420);
            slider_gr.Draw_Slider(screenWidth-200, 460);
            slider_gb.Draw_Slider(screenWidth-200, 500);
            slider_gg.Draw_Slider(screenWidth-200, 540);
                                  
            slider_ry.Draw_Slider(screenWidth-200, 600);
            slider_rr.Draw_Slider(screenWidth-200, 640);
            slider_rb.Draw_Slider(screenWidth-200, 680);
            slider_rg.Draw_Slider(screenWidth-200, 720);
            
            
            slider_viscosity.Draw_Slider(screenWidth-200, 800);
            
            
            
            // Draw slider labels
            DrawText("YELLOW x YELLOW", screenWidth-200, 50, 10, YELLOW);
            DrawText("YELLOW x RED", screenWidth-200, 90, 10, YELLOW);
            DrawText("YELLOW x BLUE", screenWidth-200, 130, 10, YELLOW);
            DrawText("YELLOW x GREEN", screenWidth-200, 170, 10, YELLOW);
            
            DrawText("BLUE x YELLOW", screenWidth-200, 230, 10, BLUE);
            DrawText("BLUE x RED", screenWidth-200, 270, 10, BLUE);
            DrawText("BLUE x BLUE", screenWidth-200, 310, 10, BLUE);
            DrawText("BLUE x GREEN", screenWidth-200, 350, 10, BLUE);
            
            DrawText("GREEN x YELLOW", screenWidth-200, 410, 10, GREEN);
            DrawText("GREEN x RED", screenWidth-200, 450, 10, GREEN);
            DrawText("GREEN x BLUE", screenWidth-200, 490, 10, GREEN);
            DrawText("GREEN x GREEN", screenWidth-200, 530, 10, GREEN);
            
            DrawText("RED x YELLOW", screenWidth-200, 590, 10, RED);
            DrawText("RED x RED", screenWidth-200, 630, 10, RED);
            DrawText("RED x BLUE", screenWidth-200, 670, 10, RED);
            DrawText("RED x GREEN", screenWidth-200, 710, 10, RED);
            
            
            DrawText("DRAG", screenWidth-200, 790, 10, WHITE);
            
            BeginMode2D(camera);
            
            // Draw and Update Particles
            Draw_And_Update_Particles(yellow, num_of_particles);
            Draw_And_Update_Particles(red, num_of_particles);
            Draw_And_Update_Particles(green, num_of_particles);
            Draw_And_Update_Particles(blue, num_of_particles);
            
            EndMode2D();

            
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();
    return 0;
}
