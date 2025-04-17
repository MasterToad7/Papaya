#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <pthread.h>

// settings
uint32_t screenSize = 512;
uint8_t setting_visualType;

GtkApplication *app;
GtkWidget *window;

GtkWidget *n_entry;
GtkWidget *l_entry;
GtkWidget *m_entry;

GtkWidget *scale_x_entry;
GtkWidget *scale_y_entry;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

char* getFileGTK(const char* filename, const char* filename2, const char* filename3);

static void on_activate(GtkApplication *app);
static void on_graph_button_clicked();
static void on_radial_button_clicked();
static void on_slice_button_clicked();

int run_gl();

int main(int argc, char **argv)
{
    app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(on_activate), NULL);
    g_application_run (G_APPLICATION(app), argc, argv);
}

int run_gl()
{
    // tell glfw to LOCK IN
    GLFWwindow* window;
    if (!glfwInit())
    {
        return -1;
    }

    // create glfw window
    window = glfwCreateWindow(screenSize, screenSize, "Papaya", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // tell glad to LOCK IN
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }     

    // create the vertex shader from source
    uint32_t vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    char graph_buffer[256];
    char radial_buffer[256];
    char harmonic_buffer[256];

    if (setting_visualType == 0)
    {
        sprintf(graph_buffer, "/org/papaya/shaders/general/graph.frag");
    } else if (setting_visualType == 1)
    {
        sprintf(graph_buffer, "/org/papaya/shaders/general/radial.frag");
    } else if (setting_visualType == 2)
    {
        sprintf(graph_buffer, "/org/papaya/shaders/general/slice.frag");
    }

    sprintf(radial_buffer, "/org/papaya/shaders/orbitals/radial/%s%s.frag", gtk_entry_buffer_get_text(gtk_entry_get_buffer(n_entry)), gtk_entry_buffer_get_text(gtk_entry_get_buffer(l_entry)));
    sprintf(harmonic_buffer, "/org/papaya/shaders/orbitals/harmonic/%s%s.frag", gtk_entry_buffer_get_text(gtk_entry_get_buffer(l_entry)), gtk_entry_buffer_get_text(gtk_entry_get_buffer(m_entry)));
    
    const char* fragmentShaderSource = getFileGTK(graph_buffer, radial_buffer, harmonic_buffer);

    // create the fragment shader from source
    uint32_t fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // create main shader program
    uint32_t shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    // setup uniforms
    float lineThickness = 1.0f;
    float position_x = -(float)screenSize/2.0f;
    float position_y = -(float)screenSize/2.0f;
    float scale_x = strtof(gtk_entry_buffer_get_text(gtk_entry_get_buffer(scale_x_entry)), NULL);
    float scale_y = strtof(gtk_entry_buffer_get_text(gtk_entry_get_buffer(scale_y_entry)), NULL);
    float intensity = 1.0f;
    int plane = 1;

    uint32_t lineThicknessLocation = glGetUniformLocation(shaderProgram, "lineThickness");
    uint32_t positionLocation = glGetUniformLocation(shaderProgram, "position");
    uint32_t scaleLocation = glGetUniformLocation(shaderProgram, "scale");
    uint32_t intensityLocation = glGetUniformLocation(shaderProgram, "intensity");
    uint32_t planeLocation = glGetUniformLocation(shaderProgram, "plane");

    // set constant uniforms
    glUniform1f(lineThicknessLocation, lineThickness);

    // define vertices of the quad
    float vertices[] = 
    {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };  

    // create vertex buffer
    uint32_t VAO;
    glGenVertexArrays(1, &VAO); 
    uint32_t VBO;
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    bool scaleKeyUpPressed = false;
    bool scaleKeyDownPressed = false;
    float initialScale_x = scale_x;
    float initialScale_y = scale_y;
    float scaleSpeedMax = 0.5f;
    float scaleSpeedMin = 0.1f;

    bool intensityKeyZPressed = false;
    bool intensityKeyXPressed = false;

    // main render loop
    glViewport(0, 0, screenSize, screenSize);
    while (!glfwWindowShouldClose(window))
    {
        float moveSpeed = 3.0f;
        float movePosition_x = 0.0f;
        float movePosition_y = 0.0f;

        // check for input
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !intensityKeyZPressed)
        {
            intensity += 1.0f;
            intensityKeyZPressed = true;
        }
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !intensityKeyXPressed && intensity > 1.0f)
        {
            intensity -= 1.0f;
            intensityKeyXPressed = true;
        }
        if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE && intensityKeyZPressed)
        {
            intensityKeyZPressed = false;
        }
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE && intensityKeyXPressed)
        {
            intensityKeyXPressed = false;
        }
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            movePosition_y += 1.0f;
        }
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            movePosition_y -= 1.0f;
        }
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            movePosition_x += 1.0f;
        }
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            movePosition_x -= 1.0f;
        }
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && scaleKeyUpPressed != true)
        {
            scaleKeyUpPressed = true;
            if(scale_x >= initialScale_x && scale_y >= initialScale_y)
            {
                scale_x += scaleSpeedMax * initialScale_x;
                scale_y += scaleSpeedMax * initialScale_y;
            } else 
            {
               scale_x += scaleSpeedMin * initialScale_x;
               scale_y += scaleSpeedMin * initialScale_y; 
            }
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && scaleKeyDownPressed != true)
        {
            scaleKeyDownPressed = true;
            if(scale_x > initialScale_x && scale_y > initialScale_y)
            {
                scale_x -= scaleSpeedMax * initialScale_x;
                scale_y -= scaleSpeedMax * initialScale_y;
            }
            else if (scale_x > scaleSpeedMin * initialScale_x && scale_y > scaleSpeedMin * initialScale_y)
            {
               scale_x -= scaleSpeedMin * initialScale_x;
               scale_y -= scaleSpeedMin * initialScale_y; 
            }
        }
        if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            plane = 1;
        }
        if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            plane = 2;
        }
        if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            plane = 3;
        }
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && scaleKeyUpPressed == true)
        {
            scaleKeyUpPressed = false;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE && scaleKeyDownPressed == true)
        {
            scaleKeyDownPressed = false;
        }

        float movePositionLength = sqrt(pow(movePosition_x, 2)+pow(movePosition_y, 2));
        if(movePositionLength != 0)
        {
            position_x += (movePosition_x/movePositionLength) * moveSpeed;
            position_y += (movePosition_y/movePositionLength) * moveSpeed;
        }

        // START RENDER CODE
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // set non constant uniforms
        glUniform2f(positionLocation, position_x, position_y);
        glUniform2f(scaleLocation, scale_x, scale_y);
        glUniform1f(intensityLocation, intensity);
        glUniform1i(planeLocation, plane);

        glDrawArrays(GL_QUADS, 0, 4);
        // END RENDER CODE

        // swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up clean up everybody do your share
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    pthread_exit(NULL);
    return 0;
}

static void on_graph_button_clicked()
{
    setting_visualType = 0;

    pthread_t thread1;
    
    // Creating a new thread. 
    pthread_create(&thread1, NULL, run_gl, NULL);

    pthread_join(thread1, NULL);
}

static void on_radial_button_clicked()
{
    setting_visualType = 1;

    pthread_t thread1;
    
    // Creating a new thread. 
    pthread_create(&thread1, NULL, run_gl, NULL);

    pthread_join(thread1, NULL);
}

static void on_slice_button_clicked()
{
    setting_visualType = 2;

    pthread_t thread1;
    
    // Creating a new thread. 
    pthread_create(&thread1, NULL, run_gl, NULL);

    pthread_join(thread1, NULL);
}

static void on_activate(GtkApplication *app) 
{
    // create widgets
    window = gtk_application_window_new(app);
    GtkWidget *box = gtk_box_new(1, 2);

    GtkWidget *n_box = gtk_box_new(0, 2);
    GtkWidget *l_box = gtk_box_new(0, 2);
    GtkWidget *m_box = gtk_box_new(0, 2);

    GtkWidget *scale_x_box = gtk_box_new(0, 2);
    GtkWidget *scale_y_box = gtk_box_new(0, 2);

    GtkWidget *graph_button;
    GtkWidget *radial_button;
    GtkWidget *slice_button;

    GtkWidget *n_entry_label;
    GtkWidget *l_entry_label;
    GtkWidget *m_entry_label;

    graph_button = gtk_button_new_with_label("Radial Graph");
    g_signal_connect_swapped(graph_button, "clicked", G_CALLBACK(on_graph_button_clicked), window);
    radial_button = gtk_button_new_with_label("Radial Density");
    g_signal_connect_swapped(radial_button, "clicked", G_CALLBACK(on_radial_button_clicked), window);
    slice_button = gtk_button_new_with_label("Vizualize Orbital");
    g_signal_connect_swapped(slice_button, "clicked", G_CALLBACK(on_slice_button_clicked), window);

    n_entry = gtk_entry_new();
    l_entry = gtk_entry_new();
    m_entry = gtk_entry_new();

    n_entry_label = gtk_label_new("n:");
    l_entry_label = gtk_label_new("l:");
    m_entry_label = gtk_label_new("m:");

    GtkWidget *scale_x_entry_label;
    GtkWidget *scale_y_entry_label;

    scale_x_entry = gtk_entry_new();
    scale_y_entry = gtk_entry_new();

    scale_x_entry_label = gtk_label_new("scale x:");
    scale_y_entry_label = gtk_label_new("scale y:");
    
    gtk_box_append(n_box, n_entry_label);
    gtk_box_append(l_box, l_entry_label);
    gtk_box_append(m_box, m_entry_label);

    gtk_box_append(n_box, n_entry);
    gtk_box_append(l_box, l_entry);
    gtk_box_append(m_box, m_entry);

    gtk_box_append(scale_x_box, scale_x_entry_label);
    gtk_box_append(scale_y_box, scale_y_entry_label);

    gtk_box_append(scale_x_box, scale_x_entry);
    gtk_box_append(scale_y_box, scale_y_entry);

    gtk_box_append(box, scale_x_box);
    gtk_box_append(box, scale_y_box);

    gtk_box_append(box, n_box);
    gtk_box_append(box, l_box);
    gtk_box_append(box, m_box);

    gtk_box_append(box, graph_button);
    gtk_box_append(box, radial_button);
    gtk_box_append(box, slice_button);

    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_window_present(GTK_WINDOW(window));
}

char* getFileGTK(const char* filename, const char* filename2, const char* filename3)
{
    gsize size1;
    gsize size2;
    gsize size3;
    const char* file1 = g_bytes_get_data(g_resources_lookup_data(filename, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL), &size1);
    const char* file2 = g_bytes_get_data(g_resources_lookup_data(filename2, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL), &size2);
    const char* file3 = g_bytes_get_data(g_resources_lookup_data(filename3, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL), &size3);
    
    gsize total_size = size1 + size2 + size3;
    guint8 *combined = g_malloc(total_size + 1);

    memcpy(combined, file1, size1);
    memcpy(combined + size1, file2, size2);
    memcpy(combined + size1 + size2, file3, size3);
    combined[total_size] = '\0';
    GBytes *combined_bytes = g_bytes_new_take(combined, total_size);

    char* output = g_bytes_get_data(combined_bytes, NULL);
    return output;
}