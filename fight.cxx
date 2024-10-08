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
    this->max_hit_points = 10;
    this->reset();
}

const char* Player::name() {
    if (index == 0) {
        return "Player One";
    } else {
        return "Player Two";
    }
}

void Player::reset() {
    this->on_ground = false;
    if (this->index == 0) {
        this->x = -0.4;
        this->y = 0;
        this->facing_x = 1;
        this->r = 1;
        this->g = 0;
        this->b = 0;
        this->body_size = 0.5;
    } else {
        this->x = 0.4;
        this->y = 0;
        this->facing_x = -1;
        this->r = 0;
        this->g = 0;
        this->b = 1;
        this->body_size = 0.5;
    }
    this->vx = 0;
    this->vy = 0;
    this->hit_points = this->max_hit_points;
    this->punch_started = false;
    this->punch_started_time = 0;
    this->punch_landed = false;
    this->kick_started = false;
    this->kick_started_time = 0;
    this->kick_landed = false;
    this->block_low_started = false;
    this->block_low_started_time = 0;
    this->block_low_landed = false;
    this->block_high_started = false;
    this->block_high_started_time = 0;
    this->block_high_landed = false;
    this->stun_started = false;
    this->stun_started_time = 0;
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
            PlayerAction* player_action = new KickPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_B]) {
            //printf("%s: B Button Pressed\n", name);
            PlayerAction* player_action = new PunchPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_X]) {
            //printf("%s: X Button Pressed\n", this->name());
            PlayerAction* player_action = new BlockLowPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
        }
        if (state.buttons[GLFW_GAMEPAD_BUTTON_Y]) {
            //printf("%s: Y Button Pressed\n", this->name());
            PlayerAction* player_action = new BlockHighPlayerAction();
            player_action->player = this;
            action->events.push_back(player_action);
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
         0.0f,                    -0.25f * this->body_size, 0.0f,
         0.10f * this->body_size,  0.5f * this->body_size,  0.0f,
        -0.25f * this->body_size,  0.5f * this->body_size,  0.0f,
         0.0f,                     0.25f * this->body_size, 0.0f,
        -0.25f * this->body_size, -0.5f * this->body_size,  0.0f,
         0.10f * this->body_size, -0.5f * this->body_size,  0.0f,
    };
    GLfloat stun_core_template_vertices[] = {
         0.0f,                     0.0f * this->body_size, 0.0f,
         0.10f * this->body_size,  0.75f * this->body_size,  0.0f,
        -0.25f * this->body_size,  0.75f * this->body_size,  0.0f,
         0.0f,                     0.25f * this->body_size, 0.0f,
        -0.25f * this->body_size, -0.5f * this->body_size,  0.0f,
         0.10f * this->body_size, -0.5f * this->body_size,  0.0f,
    };
    int punch_vertex_count = 3;
    GLfloat punch_template_vertices[] = {
        0.0f,                   0.0f,                    0.0f,
        0.5f * this->body_size, 0.1f * this->body_size, 0.0f,
        0.35f * this->body_size, 0.5f * this->body_size,  0.0f,
    };
    int kick_vertex_count = 3;
    GLfloat kick_template_vertices[] = {
        0.0f,                    -0.0f,                    0.0f,
        0.5f * this->body_size,  -0.1f * this->body_size, 0.0f,
        0.35f * this->body_size, -0.5f * this->body_size,  0.0f,
    };
    int block_low_vertex_count = 3;
    GLfloat block_low_template_vertices[] = {
        0.0f,                    -0.0f,                    0.0f,
        0.25f * this->body_size, -0.05f * this->body_size, 0.0f,
        0.25f * this->body_size, -0.5f * this->body_size,  0.0f,
    };
    int block_high_vertex_count = 3;
    GLfloat block_high_template_vertices[] = {
        0.0f,                    0.0f,                    0.0f,
        0.25f * this->body_size, 0.05f * this->body_size, 0.0f,
        0.25f * this->body_size, 0.5f * this->body_size,  0.0f,
    };
    if (this->stun_started) {
        for (int i = 0; i < core_vertex_count; i ++) {
            vertices[i * 3 + 0] = stun_core_template_vertices[i * 3 + 0];
            vertices[i * 3 + 1] = stun_core_template_vertices[i * 3 + 1];
            vertices[i * 3 + 2] = stun_core_template_vertices[i * 3 + 2];
        }
    } else {
        for (int i = 0; i < core_vertex_count; i ++) {
            vertices[i * 3 + 0] = core_template_vertices[i * 3 + 0];
            vertices[i * 3 + 1] = core_template_vertices[i * 3 + 1];
            vertices[i * 3 + 2] = core_template_vertices[i * 3 + 2];
        }
    }
    if (this->punch_started) {
        vertex_count += punch_vertex_count;
        for (int i = 0; i < punch_vertex_count; i ++) {
            vertices[(core_vertex_count + i) * 3 + 0] = punch_template_vertices[i * 3 + 0];
            vertices[(core_vertex_count + i) * 3 + 1] = punch_template_vertices[i * 3 + 1];
            vertices[(core_vertex_count + i) * 3 + 2] = punch_template_vertices[i * 3 + 2];
        }
    } else if (this->kick_started) {
        vertex_count += kick_vertex_count;
        for (int i = 0; i < kick_vertex_count; i ++) {
            vertices[(core_vertex_count + i) * 3 + 0] = kick_template_vertices[i * 3 + 0];
            vertices[(core_vertex_count + i) * 3 + 1] = kick_template_vertices[i * 3 + 1];
            vertices[(core_vertex_count + i) * 3 + 2] = kick_template_vertices[i * 3 + 2];
        }
    } else if (this->block_low_started) {
        vertex_count += block_low_vertex_count;
        for (int i = 0; i < block_low_vertex_count; i ++) {
            vertices[(core_vertex_count + i) * 3 + 0] = block_low_template_vertices[i * 3 + 0];
            vertices[(core_vertex_count + i) * 3 + 1] = block_low_template_vertices[i * 3 + 1];
            vertices[(core_vertex_count + i) * 3 + 2] = block_low_template_vertices[i * 3 + 2];
        }
    } else if (this->block_high_started) {
        vertex_count += block_high_vertex_count;
        for (int i = 0; i < block_high_vertex_count; i ++) {
            vertices[(core_vertex_count + i) * 3 + 0] = block_high_template_vertices[i * 3 + 0];
            vertices[(core_vertex_count + i) * 3 + 1] = block_high_template_vertices[i * 3 + 1];
            vertices[(core_vertex_count + i) * 3 + 2] = block_high_template_vertices[i * 3 + 2];
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

void Player::take_damage(int hit_points) {
    this->hit_points -= hit_points;
    if (this->hit_points <= 0) {
        this->hit_points = 0;
    }
}

void Player::force_away(float x, float y, float velocity) {
    float xd = x - this->x;
    float yd = y - this->y;
    xd += 0.1 * (-0.5 + (float)(rand() % RAND_MAX) / (float)RAND_MAX);
    yd += 0.1 * (-0.5 + (float)(rand() % RAND_MAX) / (float)RAND_MAX);
    float len = sqrt(xd * xd + yd * yd);
    float nx;
    float ny;
    if (len == 0) {
        nx = 0;
        ny = -1;
    } else {
        nx = xd / len;
        ny = yd / len;
    }
    this->vx = -velocity * nx;
    this->vy = -velocity * ny;
}

void Player::position_distance(float x, float y, float distance) {
    float xd = x - this->x;
    float yd = y - this->y;
    float len = sqrt(xd * xd + yd * yd);
    float nx;
    float ny;
    if (len == 0) {
        nx = 0;
        ny = -1;
    } else {
        nx = xd / len;
        ny = yd / len;
    }
    this->x = x - distance * nx;
    this->y = y - distance * ny;
    this->vx = 0;
    this->vy = 0;
}

bool Universe::initialize() {
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = new Player(this);
        players[player_index] = player;
        player->index = player_index;
        player->reset();
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
        "  gl_FragColor = vec4 ( 1.0, 0.25, 0.0, 1.0 );\n"
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
    for (int player_i = 1; player_i < 2; player_i ++) {
        Player* i_player = players[player_i];
        for (int player_j = 0; player_j < player_i; player_j ++) {
            Player* j_player = players[player_j];
            float collision_distance = 0.125 * i_player->body_size + 0.125 * j_player->body_size;
            float xd = j_player->x - i_player->x;
            float yd = j_player->y - i_player->y;
            if (xd * xd + yd * yd < collision_distance * collision_distance) {
                i_player->position_distance(j_player->x, j_player->y, 1.01 * collision_distance);
                j_player->position_distance(i_player->x, i_player->y, 1.02 * collision_distance);
                //i_player->take_damage(1);
                //j_player->take_damage(1);
            }
        }
    }
    // gravity, friction
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = players[player_index];
        player->vy -= 0.002;
        if (player->on_ground) {
            if (player->vx > 0.002 * player->body_size) {
                player->vx -= 0.002 * player->body_size;
            } else if (player->vx < -0.002 * player->body_size) {
                player->vx += 0.002 * player->body_size;
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
        if (player->x < -1.0 + 0.5 * player->body_size) {
            player->x = -1.0 + 0.5 * player->body_size;
            player->vx = 0;
        }
        if (player->x > 1.0 - 0.5 * player->body_size) {
            player->x = 1.0 - 0.5 * player->body_size;
            player->vx = 0;
        }
        if (player->y < -1.0 + 0.5 * player->body_size) {
            player->y = -1.0 + 0.5 * player->body_size;
            player->vy = 0;
            player->on_ground = true;
        } else {
            player->on_ground = false;
        }
        if (player->y > 1.0 - 0.5 * player->body_size) {
            player->y = 1.0 - 0.5 * player->body_size;
            player->vy = 0;
        }
    }
    // kick, punch
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = players[player_index];
        if (player->punch_started) {
            if (! player->punch_landed) {
                for (int player_j = 0; player_j < 2; player_j ++) {
                    if (player_index != player_j) {
                        Player* other_player = players[player_j];
                        float player_hand_x = player->x + (player->facing_x * 0.05 * player->body_size);
                        float player_hand_y = player->y + 0.1 * player->body_size;
                        float dx = other_player->x - player_hand_x;
                        float dy = other_player->y - player_hand_y;
                        float punch_distance = 0.5 * player->body_size + 0.5 * other_player->body_size;
                        if (dx * dx + dy * dy < punch_distance * punch_distance) {
                            player->punch_landed = true;
                            if (!other_player->stun_started && !other_player->block_high_started) {
                                if (player->on_ground) {
                                    other_player->force_away(player_hand_x, player_hand_y, 0.06 * player->body_size);
                                    other_player->vy += 0.12 * player->body_size;
                                    other_player->take_damage(1);
                                } else {
                                    other_player->force_away(player_hand_x, player_hand_y, 0.06 * player->body_size);
                                    other_player->vy += 0.12 * player->body_size;
                                    other_player->take_damage(3);
                                }
                                printf("Stun started.\n");
                                other_player->stun_started = true;
                                other_player->stun_started_time = this->_now;
                            }
                        }
                    }
                }
            }
            if (this->_now - player->punch_started_time > 0.4) {
                player->punch_started = false;
                printf("Punch ended.\n");
            }
        } else if (player->kick_started) {
            if (! player->kick_landed) {
                for (int player_j = 0; player_j < 2; player_j ++) {
                    if (player_index != player_j) {
                        Player* other_player = players[player_j];
                        float player_foot_x = player->x + (player->facing_x * 0.025 * player->body_size);
                        float player_foot_y = player->y - 0.05 * player->body_size;
                        float dx = other_player->x - player_foot_x;
                        float dy = other_player->y - player_foot_y;
                        float kick_distance = 0.5 * player->body_size + 0.5 * other_player->body_size;
                        if (dx * dx + dy * dy < kick_distance * kick_distance) {
                            player->kick_landed = true;
                            if (! other_player->stun_started && !other_player->block_low_started) {
                                if (player->on_ground) {
                                    other_player->force_away(player_foot_x, player_foot_y, 0.06 * player->body_size);
                                    other_player->vy += 0.06;
                                    other_player->take_damage(4);
                                } else {
                                    other_player->force_away(player_foot_x, player_foot_y, 0.06 * player->body_size);
                                    other_player->vy += 0.06;
                                    other_player->take_damage(8);
                                }
                                printf("Stun started.\n");
                                other_player->stun_started = true;
                                other_player->stun_started_time = this->_now;
                            }
                        }
                    }
                }
            }
            if (player->universe->_now - player->kick_started_time > 0.8) {
                player->kick_started = false;
                printf("Kick ended.\n");
            }
        } else if (player->block_low_started) {
            if (player->universe->_now - player->block_low_started_time > 0.8) {
                player->block_low_started = false;
                printf("BlockLow ended.\n");
            }
        } else if (player->block_high_started) {
            if (player->universe->_now - player->block_high_started_time > 0.8) {
                player->block_high_started = false;
                printf("BlockHigh ended.\n");
            }
        } else if (player->stun_started) {
            if (player->universe->_now - player->stun_started_time > 1.6) {
                player->stun_started = false;
                printf("Stun ended.\n");
            }
        }
    }
    // end of game
    int loss_count = 0;
    for (int player_index = 0; player_index < 2; player_index ++) {
        Player* player = players[player_index];
        if (player->hit_points == 0) {
            loss_count ++;
        }
    }
    if (loss_count > 0) {
        printf("Round Over.\n");
        for (int player_index = 0; player_index < 2; player_index ++) {
            Player* player = players[player_index];
            player->reset();
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
    if (this->player->on_ground &&
        !this->player->punch_started &&
        !this->player->kick_started &&
        !this->player->block_low_started &&
        !this->player->block_high_started &&
        !this->player->stun_started) {
        if (this->player->vx > -0.03 * this->player->body_size) {
            this->player->vx -= 0.01 * this->player->body_size;
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
    if (this->player->on_ground &&
        !this->player->punch_started &&
        !this->player->kick_started &&
        !this->player->block_low_started &&
        !this->player->block_high_started &&
        !this->player->stun_started) {
        if (this->player->vx < 0.03 * this->player->body_size) {
            this->player->vx += 0.01 * this->player->body_size;
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
    if (this->player->on_ground && !this->player->stun_started) {
        bool action_beyond_warmup = false;
        float warmup_period = 10.0 / 60;
        if (this->player->punch_started) {
            if (this->player->universe->_now - this->player->punch_started_time > warmup_period) {
                action_beyond_warmup = true;
            }
        }
        if (this->player->kick_started) {
            if (this->player->universe->_now - this->player->kick_started_time > warmup_period) {
                action_beyond_warmup = true;
            }
        }
        if (this->player->block_low_started) {
            if (this->player->universe->_now - this->player->block_low_started_time > warmup_period) {
                action_beyond_warmup = true;
            }
        }
        if (this->player->block_high_started) {
            if (this->player->universe->_now - this->player->block_high_started_time > warmup_period) {
                action_beyond_warmup = true;
            }
        }
        if (! action_beyond_warmup) {
            this->player->vy = 0.07;
            if (this->player->vx < 0) {
                this->player->vx = -0.125 * this->player->vy;
            } else if (this->player->vx > 0) {
                this->player->vx = 0.125 * this->player->vy;
            }
        }
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
    if (!this->player->punch_started &&
        !this->player->kick_started &&
        !this->player->block_low_started &&
        !this->player->block_high_started &&
        !this->player->stun_started) {
        this->player->kick_started = true;
        this->player->kick_started_time = this->player->universe->now();
        this->player->kick_landed = false;
        if (this->player->on_ground) {
            this->player->vx = 0;
        }
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
    if (!this->player->punch_started &&
        !this->player->kick_started &&
        !this->player->block_low_started &&
        !this->player->block_high_started &&
        !this->player->stun_started) {
        this->player->punch_started = true;
        this->player->punch_started_time = this->player->universe->now();
        this->player->punch_landed = false;
        if (this->player->on_ground) {
            this->player->vx = 0;
        }
        printf("Punch started.\n");
    }
}

int BlockLowPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: BlockLowPlayerAction}");
    return index;
}

void BlockLowPlayerAction::perform() {
    //printf("%s: BlockLow\n", this->player->name());
    if (!this->player->punch_started && !this->player->block_low_started && !this->player->stun_started) {
        this->player->block_low_started = true;
        this->player->block_low_started_time = this->player->universe->now();
        this->player->block_low_landed = false;
        if (this->player->on_ground) {
            this->player->vx = 0;
        }
        printf("BlockLow started.\n");
    }
}

int BlockHighPlayerAction::write(std::ostream& os) {
    int index = 0;
    index += stream_write(os, "{type: BlockHighPlayerAction}");
    return index;
}

void BlockHighPlayerAction::perform() {
    //printf("%s: BlockHigh\n", this->player->name());
    if (!this->player->punch_started && !this->player->block_high_started && !this->player->stun_started) {
        this->player->block_high_started = true;
        this->player->block_high_started_time = this->player->universe->now();
        this->player->block_high_landed = false;
        if (this->player->on_ground) {
            this->player->vx = 0;
        }
        printf("BlockHigh started.\n");
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
