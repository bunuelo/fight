#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <string>
#include <list>
#include <iostream>
#include <sstream>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "fight.hxx"

inline float absf(float f) {
    return f < 0 ? -f : f;
}

int stream_write(std::ostream& os, const char* str) {
    int index = 0;
    index += strlen(str);
    os << str;
    return index;
}

GLuint LoadShader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            printf("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

Event::~Event() {
}

int Event::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: Event}");
    return index;
}

void Event::destroy() {
}
    
void Event::perform() {
}

int CompoundEvent::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: CompoundEvent, events: [");
    for (std::list<Event*>::iterator it = this->events.begin(); it != this->events.end(); it ++) {
        if (it != this->events.begin()) {
            index += stream_write(os, ", ");
        }
        index += (*it)->write(os);
    }
    index += stream_write(os, "]}");
    return index;
}

void CompoundEvent::destroy() {
    for (std::list<Event*>::iterator it = this->events.begin(); it != this->events.end(); it ++) {
        (*it)->destroy();
        delete *it;
    }        
}

void CompoundEvent::perform() {
    for (std::list<Event*>::iterator it = this->events.begin(); it != this->events.end(); it ++) {
        (*it)->perform();
    }
}

int PlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: PlayerAction, events: [");
    for (std::list<Event*>::iterator it = this->events.begin(); it != this->events.end(); it ++) {
        if (it != this->events.begin()) {
            index += stream_write(os, ", ");
        }
        index += (*it)->write(os);
    }
    index += stream_write(os, "]}");
    return index;
}

Player::Player(Universe* universe) {
    this->universe = universe;
    this->joystick_index = -1;
    this->x = 0;
    this->y = 0;
    this->vx = 0;
    this->vy = 0;
    this->r = 1;
    this->g = 1;
    this->b = 1;
    this->on_ground = false;
    this->max_hit_points = 10;
    this->hit_points = this->max_hit_points;
    this->punch_started = false;
    this->punch_started_time = 0;
    this->kick_started = false;
    this->kick_started_time = 0;
}

const char* Player::name() {
    if (index == 0) {
        return "Player One";
    } else {
        return "Player Two";
    }
}

PlayerAction* Player::get_action() {
    PlayerAction* action = new PlayerAction();
    action->player = this;
    //const char* name = glfwGetGamepadName(joystick_index);
    //printf("Joystick %d is Gamepad: %s\n", joystick_index, name);
    GLFWgamepadstate state;
    if (glfwGetGamepadState(joystick_index, &state)) {
        if (state.buttons[GLFW_GAMEPAD_BUTTON_A]) {
            //printf("%s: A Button Pressed\n", name);
            PlayerAction* player_action = new PunchPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_B]) {
            //printf("%s: B Button Pressed\n", name);
            PlayerAction* player_action = new KickPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_X]) {
            //printf("%s: X Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_Y]) {
            //printf("%s: Y Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]) {
            //printf("%s: Left Bumper Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]) {
            //printf("%s: Right Bumper Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_BACK]) {
            //printf("%s: Back Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_START]) {
            //printf("%s: Start Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE]) {
            //printf("%s: Guide Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB]) {
            //printf("%s: Left Thumb Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]) {
            //printf("%s: Right Thumb Button Pressed\n", name);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) {
            //printf("%s: Up Button Pressed\n", name);
            PlayerAction* player_action = new JumpPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
            //printf("%s: Down Button Pressed\n", name);
            PlayerAction* player_action = new DuckPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) {
            //printf("%s: Right Button Pressed\n", name);
            PlayerAction* player_action = new MoveRightPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) {
            //printf("%s: Left Button Pressed\n", name);
            PlayerAction* player_action = new MoveLeftPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (absf(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]) > 0.5) {
            //printf("%s: Left X Axis = %f\n", name, state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
        }
        if (absf(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) > 0.5) {
            //printf("%s: Left Y Axis = %f\n", name, state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
        }
        if (absf(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) > 0.5) {
            //printf("%s: Right X Axis = %f\n", name, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
        }
        if (absf(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]) > 0.5) {
            //printf("%s: Right Y Axis = %f\n", name, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
        }
        if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.5) {
            //printf("%s: Left Trigger Axis = %f\n", name, state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
        }
        if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.5) {
            //printf("%s: Right Trigger Axis = %f\n", name, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
        }
    }
    
    return action;
}

int Player::write_body_vertices(GLfloat* vertices) {
    int vertex_count = 0;
    int core_vertex_count = 6;
    vertex_count += core_vertex_count;
    GLfloat core_template_vertices[] = {
        0.0f, -0.125f, 0.0f,
        0.05f,  0.25f, 0.0f,
        -0.125f, 0.25f, 0.0f,
        0.0f, 0.125f, 0.0f,
        -0.125f, -0.25f, 0.0f,
        0.05f, -0.25f, 0.0f,
    };
    int punch_vertex_count = 3;
    GLfloat punch_template_vertices[] = {
        0.0f,  0.0f, 0.0f,
        0.25f, 0.125f, 0.0f,
        0.15f, 0.25f, 0.0f,
    };
    int kick_vertex_count = 3;
    GLfloat kick_template_vertices[] = {
        0.0f,  0.0f, 0.0f,
        0.25f, -0.125f, 0.0f,
        0.15f, -0.25f, 0.0f,
    };
    for (int i = 0; i < core_vertex_count; i ++) {
        vertices[i * 3 + 0] = core_template_vertices[i * 3 + 0];
        vertices[i * 3 + 1] = core_template_vertices[i * 3 + 1];
        vertices[i * 3 + 2] = core_template_vertices[i * 3 + 2];
    }
    if (this->punch_started) {
        vertex_count += punch_vertex_count;
        for (int i = 0; i < punch_vertex_count; i ++) {
            vertices[(core_vertex_count + i) * 3 + 0] = punch_template_vertices[i * 3 + 0];
            vertices[(core_vertex_count + i) * 3 + 1] = punch_template_vertices[i * 3 + 1];
            vertices[(core_vertex_count + i) * 3 + 2] = punch_template_vertices[i * 3 + 2];
        }
    }
    if (this->kick_started) {
        vertex_count += kick_vertex_count;
        for (int i = 0; i < kick_vertex_count; i ++) {
            vertices[(core_vertex_count + i) * 3 + 0] = kick_template_vertices[i * 3 + 0];
            vertices[(core_vertex_count + i) * 3 + 1] = kick_template_vertices[i * 3 + 1];
            vertices[(core_vertex_count + i) * 3 + 2] = kick_template_vertices[i * 3 + 2];
        }
    }
    int triangle_count = vertex_count / 3;
    for (int i = 0; i < triangle_count; i ++) {
        if (this->facing_x == -1) {
            vertices[i * 3 * 3 + 0] = -vertices[i * 3 * 3 + 0];
            vertices[i * 3 * 3 + 1] =  vertices[i * 3 * 3 + 1];
            vertices[i * 3 * 3 + 2] =  vertices[i * 3 * 3 + 2];
            float x = -vertices[i * 3 * 3 + 3];
            float y =  vertices[i * 3 * 3 + 4];
            float z =  vertices[i * 3 * 3 + 5];
            vertices[i * 3 * 3 + 3] = -vertices[i * 3 * 3 + 6];
            vertices[i * 3 * 3 + 4] =  vertices[i * 3 * 3 + 7];
            vertices[i * 3 * 3 + 5] =  vertices[i * 3 * 3 + 8];
            vertices[i * 3 * 3 + 6] = x;
            vertices[i * 3 * 3 + 7] = y;
            vertices[i * 3 * 3 + 8] = z;
        }
        vertices[i * 3 * 3 + 0] += this->x;
        vertices[i * 3 * 3 + 1] += this->y;
        vertices[i * 3 * 3 + 3] += this->x;
        vertices[i * 3 * 3 + 4] += this->y;
        vertices[i * 3 * 3 + 6] += this->x;
        vertices[i * 3 * 3 + 7] += this->y;
    }
    return vertex_count;
}

int Player::write_power_bar_vertices(GLfloat* vertices) {
    int vertex_count = 6;
    GLfloat template_vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
    };
    float power_percent = (float)this->hit_points / (float)this->max_hit_points;
    for (int i = 0; i < vertex_count; i ++) {
        vertices[i * 3 + 0] = power_percent * template_vertices[i * 3 + 0] * 0.5  + 1.3 * this->index - 0.9;
        vertices[i * 3 + 1] = template_vertices[i * 3 + 1] * 0.125 + 0.75;
        vertices[i * 3 + 2] = template_vertices[i * 3 + 2];
    }
    return vertex_count;
}


bool Universe::initialize() {
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = new Player(this);
        players[player_index] = player;
        player->index = player_index;
        if (player_index == 0) {
            player->x = -0.5;
            player->facing_x = 1;
            player->r = 1;
            player->g = 0;
            player->b = 0;
        } else {
            player->x = 0.5;
            player->facing_x = -1;
            player->r = 0;
            player->g = 0;
            player->b = 1;
        }
    }
    
    GLchar vShaderStr[] =
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";
    
    GLchar fShaderStr[] =
        "precision mediump float;\n"
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
        "}                                            \n";
    
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    
    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    
    // Create the program object
    programObject = glCreateProgram();
    
    if (programObject == 0) {
        return false; // failure
    }
    
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vPosition");

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    
    if (!linked) {
        GLint infoLen = 0;
        
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            printf("Error linking program:\n%s\n", infoLog);
            
            free(infoLog);
        }
        
        glDeleteProgram(programObject);
        return false;
    }
    this->program_object = programObject;
    this->_now = 0;
    return true; // success
}

void Universe::destroy() {
    for (int player_index = 0; player_index < 2; player_index ++) {
        delete players[player_index];
    }
}

double Universe::now() {
    return this->_now;
}

bool Universe::all_players_present() {
    for (int joystick_index = GLFW_JOYSTICK_1; joystick_index < GLFW_JOYSTICK_LAST; joystick_index ++) {
        int is_gamepad = glfwJoystickIsGamepad(joystick_index);
        if (is_gamepad) {
            //const char* name = glfwGetGamepadName(joystick_index);
            //printf("Joystick %d is Gamepad: %s\n", joystick_index, name);
            bool gamepad_used = false;
            for (int player_index = 0; player_index < 2; player_index ++) {
                if (players[player_index]->joystick_index == joystick_index) {
                    gamepad_used = true;
                }
            }
            if (! gamepad_used) {
                for (int player_index = 0; player_index < 2; player_index ++) {
                    if (players[player_index]->joystick_index == -1) {
                        const char* name = glfwGetGamepadName(joystick_index);
                        printf("%s is Joystick %d: %s\n", players[player_index]->name(), joystick_index, name);
                        players[player_index]->joystick_index = joystick_index;
                        break;
                    }
                }
            }
        }
    }
    bool _all_players_present = true;
    for (int player_index = 0; player_index < 2; player_index ++) {
        if (players[player_index]->joystick_index == -1) {
            _all_players_present = false;
            printf("Waiting for %s.\n", players[player_index]->name());
        }
    }
    return _all_players_present;
}

void Universe::perform_actions() {
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = players[player_index];
        PlayerAction* player_action = player->get_action();
        //{
        //    std::ostringstream oss;
        //    player_action->write(oss);
        //    std::string oss__str = oss.str();
        //    printf("Player %d action = %s\n", player_index, oss__str.c_str());
        //}
        player_action->perform();
        player_action->destroy();
        delete player_action;
    }
    // collisions between players
    for (int player_i = 0; player_i < 2; player_i ++) {
        Player* i_player = players[player_i];
        for (int player_j = 0; player_j < player_i; player_j ++) {
            Player* j_player = players[player_j];
            float xd = j_player->x - i_player->x;
            float yd = j_player->y - i_player->y;
            xd += 0.1 * (-0.5 + (float)(rand() % RAND_MAX) / (float)RAND_MAX);
            yd += 0.1 * (-0.5 + (float)(rand() % RAND_MAX) / (float)RAND_MAX);
            if (xd * xd + yd * yd < 0.125 * 0.125) {
                float len = sqrt(xd * xd + yd * yd);
                float nx = xd / len;
                float ny = yd / len;
                i_player->vx = -0.025 * nx;
                i_player->vy = -0.025 * ny + 0.025;
                j_player->vx =  0.025 * nx;
                j_player->vy =  0.025 * ny + 0.025;
                i_player->hit_points -= 1;
                if (i_player->hit_points <= 0) {
                    i_player->hit_points = i_player->max_hit_points;
                }
                j_player->hit_points -= 1;
                if (j_player->hit_points <= 0) {
                    j_player->hit_points = j_player->max_hit_points;
                }
            }
        }
    }
    // gravity, friction
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = players[player_index];
        player->vy -= 0.002;
        if (player->on_ground) {
            if (player->vx > 0.001) {
                player->vx -= 0.001;
            } else if (player->vx < -0.001) {
                player->vx += 0.001;
            } else {
                player->vx = 0;
            }
        }
    }
    // boundaries, ground detection
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = players[player_index];
        player->x += player->vx;
        player->y += player->vy;
        if (player->x < -0.75) {
            player->x = -0.75;
            player->vx = 0;
        }
        if (player->x > 0.75) {
            player->x = 0.75;
            player->vx = 0;
        }
        if (player->y < -0.75) {
            player->y = -0.75;
            player->vy = 0;
            player->on_ground = true;
        } else {
            player->on_ground = false;
        }
        if (player->y > 0.75) {
            player->y = 0.75;
            player->vy = 0;
        }
    }
    // kick, punch
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = players[player_index];
        if (player->punch_started) {
            if (player->universe->_now - player->punch_started_time > 0.4) {
                player->punch_started = false;
                for (int player_j = 0; player_j < 2; player_j ++) {
                    if (player_index != player_j) {
                        Player* other_player = players[player_j];
                        
                    }
                }
                printf("Punch ended.\n");
            }
        }
        if (player->kick_started) {
            if (player->universe->_now - player->kick_started_time > 0.8) {
                player->kick_started = false;
                printf("Kick ended.\n");
            }
        }
    }
    this->_now += 1.0 / 60;
}

void Universe::render(GLFWwindow* window) {
    GLfloat vertices[3 * 18];
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    // Clear color buffer to black
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Use the program object
    glUseProgram(this->program_object);
    
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = this->players[player_index];
        {
            int vertex_count = player->write_body_vertices(vertices);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        }
        {
            int vertex_count = player->write_power_bar_vertices(vertices);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        }
    }
}

int MoveLeftPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: MoveLeftPlayerAction}");
    return index;
}

void MoveLeftPlayerAction::perform() {
    //printf("%s: Move Left\n", this->player->name());
    if (this->player->on_ground) {
        if (this->player->vx > -0.015) {
            this->player->vx -= 0.01;
        }
        this->player->facing_x = -1;
    }
}

int MoveRightPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: MoveRightPlayerAction}");
    return index;
}

void MoveRightPlayerAction::perform() {
    //printf("%s: Move Right\n", this->player->name());
    if (this->player->on_ground) {
        if (this->player->vx < 0.015) {
            this->player->vx += 0.01;
        }
        this->player->facing_x = 1;
    }
}

int JumpPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: JumpPlayerAction}");
    return index;
}
    
void JumpPlayerAction::perform() {
    //printf("%s: Jump\n", this->player->name());
    if (this->player->on_ground) {
        this->player->vy = 0.06;
    }
}

int DuckPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: DuckPlayerAction}");
    return index;
}

void DuckPlayerAction::perform() {
    //printf("%s: Duck\n", this->player->name());
    if (this->player->on_ground) {
        this->player->vy -= 0.01;
    }
}

int KickPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: KickPlayerAction}");
    return index;
}

void KickPlayerAction::perform() {
    //printf("%s: Kick\n", this->player->name());
    if (!this->player->punch_started && !this->player->kick_started) {
        this->player->kick_started = true;
        this->player->kick_started_time = this->player->universe->now();
        printf("Kick started.\n");
    }
}

int PunchPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: PunchPlayerAction}");
    return index;
}

void PunchPlayerAction::perform() {
    //printf("%s: Punch\n", this->player->name());
    if (!this->player->punch_started && !this->player->kick_started) {
        this->player->punch_started = true;
        this->player->punch_started_time = this->player->universe->now();
        printf("Punch started.\n");
    }
}

int main(int argc, char** argv) {
    GLFWwindow* window;
    int width = 1920;
    int height = 1024;
    //int width = 800;
    //int height = 600;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(width, height, "Fight", glfwGetPrimaryMonitor(), NULL);
    //window = glfwCreateWindow(width, height, "Fight", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    Universe* universe = new Universe();
    if (! universe->initialize()) {
        return -1;
    }
    bool all_players_present = false;
    while (! all_players_present) {
        all_players_present = universe->all_players_present();
        if (! all_players_present) {
            sleep(1);
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
        if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            return 0;
        }
    }
    int done = 0;
    while (! done) {
        universe->perform_actions();
        universe->render(window);
        glfwPollEvents();
        glfwSwapBuffers(window);
        done = glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
    }
    universe->destroy();
    return 0;
}
