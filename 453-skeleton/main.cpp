#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <limits>
#include <functional>
#include <utility>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

struct WorldObject
{

    WorldObject(std::string texturePath, GLenum textureInterpolation) : texture(texturePath, textureInterpolation)
    {
    }

    Texture texture;

    CPU_Geometry cgeom;
    GPU_Geometry ggeom;

    std::vector<glm::vec3> original_verts;
    std::vector<glm::vec3> original_normals;

    // Standard PI value we'll use for angle/distance calculations
    float PI = 3.14159265359;

    float scaling_factor = 1;
    int radius = 1;
    float object_radius = radius * scaling_factor;
    float angle = glm::radians(0.0f); // rotation of planet
    float theta = 0.0f;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    void generateSpheres()
    {
        /* 
        Following formula used for x, y, z 

        x = sin(u) * cos(v)
        y = sin(u) * sin(v)
        z = cos(u)
        vertical angle so 0 <= u <= 180 (stack angle)
        horizontal angle so 0 <= v <= 360 (sector angle)

    */

        // Sphere size and triangle number
        int sector_count = 36;
        int stack_count = 18;

        // Values for vertices as well as their normals
        float x, y, z, xy;
        float nx, ny, nz;
        float length_normaliser = 1.0f / radius;

        // Values for texture coordinates
        float t0, t1;

        // Steps will determine how many triangles total are generated
        float sector_step = 2 * PI / sector_count;
        float stack_step = PI / stack_count;

        float sector_angle_1, stack_angle_1, sector_angle_2, stack_angle_2;

        // Variables which form points, textures, and normals for our 2 triangles
        glm::vec3 p1_vertex, p1_normals, p2_vertex, p2_normals, p3_vertex, p3_normals, p4_vertex, p4_normals;
        glm::vec2 p1_textures, p2_textures, p3_textures, p4_textures;

        // code creates sectors like below
        /*

    p1-----p2
    |       |
    |       |
    |       |
    p3------p4

    */
        // those sectors then converted to triangles
        /*

    p1---p2
    |   / |
    |  /  |
    | /   |
    p3----p4

    */

        for (int i = 0; i <= stack_count; i++)
        {
            for (int j = 0; j <= sector_count; j++)
            {
                stack_angle_1 = PI - (i * stack_step); // starting from 180 to 0
                sector_angle_1 = j * sector_step;      // starting from 0 to 360

                stack_angle_2 = PI - ((i + 1) * stack_step); // starting from 180 to 0
                sector_angle_2 = (j + 1) * sector_step;

                // p1
                xy = radius * sinf(stack_angle_1); // r * sin(u)
                z = radius * cosf(stack_angle_1);  // r * cos(u)

                // vertex position (x, y, z)
                x = xy * cosf(sector_angle_1); // r * sin(u) * cos(v)
                y = xy * sinf(sector_angle_1); // r * sin(u) * sin(v)
                p1_vertex = glm::vec3(x, y, z);

                // normalized vertex normal (nx, ny, nz)
                nx = x * length_normaliser;
                ny = y * length_normaliser;
                nz = z * length_normaliser;
                p1_normals = glm::vec3(nx, ny, nz);

                // vertex tex coord (t0, t1) range between [0, 1]
                t0 = (float)j / sector_count;
                t1 = (float)i / stack_count;
                p1_textures = glm::vec2(t0, t1);

                // p3
                xy = radius * sinf(stack_angle_2); // r * sin(u)
                z = radius * cosf(stack_angle_2);  // r * cos(u)

                // vertex position (x, y, z)
                x = xy * cosf(sector_angle_1); // r * sin(u) * cos(v)
                y = xy * sinf(sector_angle_1); // r * sin(u) * sin(v)
                p3_vertex = glm::vec3(x, y, z);

                // normalized vertex normal (nx, ny, nz)
                nx = x * length_normaliser;
                ny = y * length_normaliser;
                nz = z * length_normaliser;
                p3_normals = glm::vec3(nx, ny, nz);

                // vertex tex coord (t0, t1) range between [0, 1]
                t0 = (float)j / sector_count;
                t1 = (float)(i + 1) / stack_count;
                p3_textures = glm::vec2(t0, t1);

                // p2
                xy = radius * sinf(stack_angle_1); // r * sin(u)
                z = radius * cosf(stack_angle_1);  // r * cos(u)

                // vertex position (x, y, z)
                x = xy * cosf(sector_angle_2); // r * sin(u) * cos(v)
                y = xy * sinf(sector_angle_2); // r * sin(u) * sin(v)
                p2_vertex = glm::vec3(x, y, z);

                // normalized vertex normal (nx, ny, nz)
                nx = x * length_normaliser;
                ny = y * length_normaliser;
                nz = z * length_normaliser;
                p2_normals = glm::vec3(nx, ny, nz);

                // vertex tex coord (t0, t1) range between [0, 1]
                t0 = (float)(j + 1) / sector_count;
                t1 = (float)i / stack_count;
                p2_textures = glm::vec2(t0, t1);

                // p4
                xy = radius * sinf(stack_angle_2); // r * sin(u)
                z = radius * cosf(stack_angle_2);  // r * cos(u)

                // vertex position (x, y, z)
                x = xy * cosf(sector_angle_2); // r * sin(u) * cos(v)
                y = xy * sinf(sector_angle_2); // r * sin(u) * sin(v)
                p4_vertex = glm::vec3(x, y, z);

                // normalized vertex normal (nx, ny, nz)
                nx = x * length_normaliser;
                ny = y * length_normaliser;
                nz = z * length_normaliser;
                p4_normals = glm::vec3(nx, ny, nz);

                // vertex tex coord (t0, t1) range between [0, 1]
                t0 = (float)(j + 1) / sector_count;
                t1 = (float)(i + 1) / stack_count;
                p4_textures = glm::vec2(t0, t1);

                // First triangle
                cgeom.verts.push_back(p1_vertex);
                cgeom.verts.push_back(p3_vertex);
                cgeom.verts.push_back(p2_vertex);

                cgeom.normals.push_back(p1_normals);
                cgeom.normals.push_back(p3_normals);
                cgeom.normals.push_back(p2_normals);

                cgeom.textures.push_back(p1_textures);
                cgeom.textures.push_back(p3_textures);
                cgeom.textures.push_back(p2_textures);

                // Second triangle
                cgeom.verts.push_back(p2_vertex);
                cgeom.verts.push_back(p3_vertex);
                cgeom.verts.push_back(p4_vertex);

                cgeom.normals.push_back(p2_normals);
                cgeom.normals.push_back(p3_normals);
                cgeom.normals.push_back(p4_normals);

                cgeom.textures.push_back(p2_textures);
                cgeom.textures.push_back(p3_textures);
                cgeom.textures.push_back(p4_textures);
            }
        }
    }

    void addCols()
    {
        cgeom.cols.resize(cgeom.verts.size(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void updateGPU()
    {
        ggeom.bind();
        ggeom.setVerts(cgeom.verts);
        ggeom.setCols(cgeom.cols);
        ggeom.setNormals(cgeom.normals);
        ggeom.setTextures(cgeom.textures);
    }

    void backUpCoords()
    {
        for (int i = 0; i < cgeom.verts.size(); i++)
        {
            original_verts.push_back(cgeom.verts[i]);
        }

        for (int i = 0; i < cgeom.normals.size(); i++)
        {
            original_normals.push_back(cgeom.normals[i]);
        }

        straightenGlobe();
        axialTilt();
    }

    void centerSpace()
    {
        float a = glm::radians(0.0f);
        float b = glm::radians(0.0f);

        glm::mat4 z_rotation{
            cosf(a), sinf(a), 0.0f, 0.0f,
            -sinf(a), cosf(a), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        glm::mat4 x_rotation{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cosf(b), sinf(b), 0.0f,
            0.0f, -sinf(b), cosf(b), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        for (int i = 0; i < original_verts.size(); i++)
        {
            original_verts[i] = z_rotation * x_rotation * glm::vec4(original_verts[i], 1.0f);
        }
    }

    void updateNormals()
    {
        float length_normaliser = 1.0f / object_radius;

        for (int i = 0; i < cgeom.normals.size(); i++)
        {
            cgeom.normals[i] = glm::vec3(cgeom.verts[i].x * length_normaliser, cgeom.verts[i].y * length_normaliser, cgeom.verts[i].z * length_normaliser);
        }
    }

    void transformVerts()
    {
        glm::mat4 scaling {
            scaling_factor, 0.0f, 0.0f, 0.0f,
            0.0f, scaling_factor, 0.0f, 0.0f,
            0.0f, 0.0f, scaling_factor, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        glm::mat4 rotation {
            cosf(angle), 0.0f, -sinf(angle), 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sinf(angle), 0.0f, cosf(angle), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        glm::mat4 test = glm::mat4(1.0f);

        glm::mat4 translation {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            position.x, position.y, position.z, 1.0f};

        for (int i = 0; i < cgeom.verts.size(); i++)
        {
            cgeom.verts[i] = translation * (rotation * (scaling * glm::vec4(original_verts[i], 1.0f)));
        }

        glm::mat4 reverse_rotation{
            cosf(-angle), 0.0f, -sinf(-angle), 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sinf(-angle), 0.0f, cosf(-angle), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        for (int i = 0; i < cgeom.verts.size(); i++)
        {
            cgeom.normals[i] = reverse_rotation * (glm::vec4(original_normals[i], 1.0f));
        }

        updateNormals();
        addCols();
        updateGPU();
    }

    void transformSun()
    {
        glm::mat4 scaling{
            scaling_factor, 0.0f, 0.0f, 0.0f,
            0.0f, scaling_factor, 0.0f, 0.0f,
            0.0f, 0.0f, scaling_factor, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        for (int i = 0; i < cgeom.verts.size(); i++)
        {
            cgeom.verts[i] = scaling * glm::vec4(original_verts[i], 1.0f);
        }

        updateNormals();
        addCols();
        updateGPU();
    }

    void straightenGlobe()
    {
        float a = glm::radians(75.0f);
        float b = glm::radians(-90.0f);

        glm::mat4 z_rotation{
            cosf(a), sinf(a), 0.0f, 0.0f,
            -sinf(a), cosf(a), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        glm::mat4 x_rotation{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cosf(b), sinf(b), 0.0f,
            0.0f, -sinf(b), cosf(b), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
        
        for (int i = 0; i < original_verts.size(); i++)
        {
            original_verts[i] = x_rotation * z_rotation * glm::vec4(original_verts[i], 1.0f);
        }

        updateNormals();
    }

    void axialTilt()
    {
        float a = glm::radians(-23.0f); // axial tilt 
        float b = glm::radians(10.0f);

        glm::mat4 z_rotation_1{
            cosf(a), sinf(a), 0.0f, 0.0f,
            -sinf(a), cosf(a), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        glm::mat4 x_rotation{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cosf(b), sinf(b), 0.0f,
            0.0f, -sinf(b), cosf(b), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        for (int i = 0; i < original_verts.size(); i++)
        {
            original_verts[i] = z_rotation_1 * x_rotation * glm::vec4(original_verts[i], 1.0f);
        }

        updateNormals();
    }

    void continueRotation(int speed, float differential)
    {
        angle += (2 * speed * differential * -0.0053);
    }

    void continueOrbit(glm::vec3 orbitting, int pace)
    {  

        float speed = pace * 0.005;
        glm::vec3 dist = glm::vec3(0.0f, 0.0f, 0.0f) - orbitting;

        glm::mat4 translateToOrigin{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            (-dist.x * 0.25), (-dist.y * 0.25), (-dist.z * 0.25), 1.0f};

        position = translateToOrigin * glm::vec4(position, 1.0f);
        position = glm::vec3(((position.x * cosf(speed)) - (position.y * sinf(speed))), ((position.x * sinf(speed)) + (position.y * cosf(speed))), position.z);

        glm::mat4 translateBack{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            (dist.x * 0.25), (dist.y * 0.25), (dist.z * 0.25), 1.0f};

        position = translateBack * glm::vec4(position, 1.0f);

    }
};

struct Movement
{
    bool earth_rotation = true;
    bool orbital_rotation = false;
    int speed = 1;
};

void orbitalInclination(WorldObject& ref, WorldObject& subject, float dist, float pitch, float yaw)
{
    float x = sinf(yaw) * cosf(pitch);
    float y = sinf(pitch);
    float z = cosf(yaw) * cosf(pitch);

    subject.position = glm::vec3(ref.position.x + (dist * x), ref.position.x + (dist * y), ref.position.x + (dist * z));
}

// EXAMPLE CALLBACKS
class Assignment4 : public CallbackInterface
{

public:
    Assignment4(Movement &system) : camera(0.0, 0.0, -2.0), aspect(1.0f), system(system)
    {
    }

    virtual void keyCallback(int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        {
            system.earth_rotation = !system.earth_rotation;
        }

        if (key == GLFW_KEY_E && action == GLFW_PRESS)
        {
            system.orbital_rotation = !system.orbital_rotation;
        }

        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        {
            if(system.speed > 1) system.speed -= 1;
        }

        if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        {
            if (system.speed < 5) system.speed += 1;
        }
    }
    virtual void mouseButtonCallback(int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                rightMouseDown = true;
            }
            else if (action == GLFW_RELEASE)
            {
                rightMouseDown = false;
            }
        }
    }
    virtual void cursorPosCallback(double xpos, double ypos)
    {
        if (rightMouseDown)
        {
            double dx = xpos - mouseOldX;
            double dy = ypos - mouseOldY;
            camera.incrementTheta(dy);
            camera.incrementPhi(dx);
        }
        mouseOldX = xpos;
        mouseOldY = ypos;
    }
    virtual void scrollCallback(double xoffset, double yoffset)
    {
        camera.incrementR(yoffset);
    }
    virtual void windowSizeCallback(int width, int height)
    {
        // The CallbackInterface::windowSizeCallback will call glViewport for us
        CallbackInterface::windowSizeCallback(width, height);
        aspect = float(width) / float(height);
    }

    void viewPipeline(ShaderProgram &sp)
    {
        glm::mat4 M = glm::mat4(1.0);
        glm::mat4 V = camera.getView();
        glm::mat4 P = glm::perspective(glm::radians(45.0f), aspect, 0.01f, 1000.f);

        GLint location = glGetUniformLocation(sp, "light");
        glm::vec3 light = camera.getPos();
        glUniform3fv(location, 1, glm::value_ptr(light));

        GLint uniMat = glGetUniformLocation(sp, "M");
        glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(M));
        uniMat = glGetUniformLocation(sp, "V");
        glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(V));
        uniMat = glGetUniformLocation(sp, "P");
        glUniformMatrix4fv(uniMat, 1, GL_FALSE, glm::value_ptr(P));
    }

    Camera camera;

private:
    bool rightMouseDown = false;
    float aspect;
    double mouseOldX;
    double mouseOldY;
    Movement &system;
};


int main()
{
    Log::debug("Starting main");

    // WINDOW
    glfwInit();
    Window window(800, 800, "CPSC 453-Assignment 4"); // can set callbacks at construction if desired

    GLDebug::enable();

    Movement solar_system;

    // CALLBACKS
    auto a4 = std::make_shared<Assignment4>(solar_system);
    window.setCallbacks(a4);

    ShaderProgram shader("shaders/test.vert", "shaders/test.frag");

    WorldObject earth("textures/earth.png", GL_LINEAR);
    WorldObject moon("textures/moon.png", GL_LINEAR);
    WorldObject sun("textures/sun.png", GL_LINEAR);
    WorldObject space("textures/space.png", GL_LINEAR);

    sun.generateSpheres();
    sun.backUpCoords();
    sun.scaling_factor = 0.15f;

    earth.generateSpheres();
    earth.backUpCoords();
    earth.scaling_factor = 0.017;
    float earth_distance_from_sun = 1.0f;
    orbitalInclination(sun, earth, earth_distance_from_sun, -25, -25);

    moon.generateSpheres();
    moon.backUpCoords();
    moon.scaling_factor = 0.0085;
    float moon_distance_from_earth = 0.6f;
    orbitalInclination(earth, moon, moon_distance_from_earth, -25, -25);

    space.generateSpheres();
    space.backUpCoords();
    space.centerSpace();
    space.scaling_factor = 4.0f;

    // RENDER LOOP
    while (!window.shouldClose())
    {
        glfwPollEvents();

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        a4->viewPipeline(shader);

        shader.use();

        // space drawing
        space.transformSun();
        space.ggeom.bind();

        space.texture.bind();
        glDrawArrays(GL_TRIANGLES, 0, GLsizei(space.cgeom.verts.size()));
        space.texture.unbind();

        // earth drawing
        earth.transformVerts();
        earth.ggeom.bind();

        earth.texture.bind();
        glDrawArrays(GL_TRIANGLES, 0, GLsizei(earth.cgeom.verts.size()));
        earth.texture.unbind();

        // sun drawing
        sun.transformSun();
        sun.ggeom.bind();

        sun.texture.bind();
        glDrawArrays(GL_TRIANGLES, 0, GLsizei(sun.cgeom.verts.size()));
        sun.texture.unbind();

        // moon drawing
        moon.transformVerts();
        moon.ggeom.bind();

        moon.texture.bind();
        glDrawArrays(GL_TRIANGLES, 0, GLsizei(moon.cgeom.verts.size()));
        moon.texture.unbind();

        if (solar_system.earth_rotation)
        {
            earth.continueRotation(solar_system.speed, 1);
            moon.continueRotation(solar_system.speed, 0.5);
        }

        if (solar_system.orbital_rotation)
        {
            earth.continueOrbit(sun.position, solar_system.speed);
            moon.continueOrbit(sun.position,solar_system.speed);
        }

        glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

        // Starting the new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Putting the text-containing window in the top-left of the screen.
        ImGui::SetNextWindowPos(ImVec2(5, 5));

        // Setting flags
        ImGuiWindowFlags textWindowFlags =
            ImGuiWindowFlags_NoMove |           // text "window" should not move
            ImGuiWindowFlags_NoResize |         // should not resize
            ImGuiWindowFlags_NoCollapse |       // should not collapse
            ImGuiWindowFlags_NoSavedSettings |  // don't want saved settings mucking things up
            ImGuiWindowFlags_AlwaysAutoResize | // window should auto-resize to fit the text
            ImGuiWindowFlags_NoBackground |     // window should be transparent; only the text should be visible
            ImGuiWindowFlags_NoDecoration |     // no decoration; only the text should be visible
            ImGuiWindowFlags_NoTitleBar;        // no title; only the text should be visible

        // Begin a new window with these flags. (bool *)0 is the "default" value for its argument.
        ImGui::Begin("I/O Info", (bool *)0, textWindowFlags);

        // Scale up text a little, and set its value
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Press Q to Pause Earth's Rotation");
        ImGui::Text("Press E to Pause Orbital Rotation");

        if (solar_system.orbital_rotation)
        {
            ImGui::Text("Orbital Rotation: Ongoing");
        }
        else
        {
            ImGui::Text("Orbital Rotation: Paused");
        }

        if (solar_system.earth_rotation)
        {
            ImGui::Text("Earth's Rotation: Ongoing");
        }
        else
        {
            ImGui::Text("Earth's Rotation: Paused");
        }

        // End the window.
        ImGui::End();

        ImGui::Render();                                        // Render the ImGui window
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing

        window.swapBuffers();
    }

    // ImGui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
