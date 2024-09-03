#ifndef FIGHT__HXX
#define FIGHT__HXX

class Universe;
class PlayerAction;

class Player {
public:
    Universe* universe;
    
    int   index;
    int   joystick_index;
    float x, y;
    float vx, vy;
    float body_size;
    float r, g, b;
    int   facing_x;
    bool  on_ground;
    int   max_hit_points;
    int   hit_points;
    bool  punch_started;
    float punch_started_time;
    bool  punch_landed;
    bool  kick_started;
    float kick_started_time;
    bool  kick_landed;
    bool  block_low_started;
    float block_low_started_time;
    bool  block_low_landed;
    bool  block_high_started;
    float block_high_started_time;
    bool  block_high_landed;
    bool  stun_started;
    float stun_started_time;
    
    Player(Universe* universe);
    const char*   name                    ();
    void          reset                   ();
    PlayerAction* get_action              ();
    int           write_body_vertices     (GLfloat* vertices);
    int           write_power_bar_vertices(GLfloat* vertices);
    void          take_damage             (int hit_points);
    void          force_away              (float x, float y);
    void          force_away              (float x, float y, float velocity);
    void          position_distance       (float x, float y, float distance);
};

class Universe {
public:
    Player* players[2];
    GLuint program_object;
    double _now;
    
    bool initialize();
    void destroy();
    double now();
    bool all_players_present();
    void perform_actions();
    void render(GLFWwindow* window);
};

class Event {
public:
    virtual ~Event();
    virtual int write(std::ostream& os);
    virtual void destroy();
    virtual void perform();
};

class CompoundEvent : public Event {
public:
    std::list<Event*> events;
    
    virtual int write(std::ostream& os);
    virtual void destroy();
    virtual void perform();
};

class PlayerAction : public CompoundEvent {
public:
    Player* player;
    virtual int write(std::ostream& os);
};

class MoveLeftPlayerAction : public PlayerAction {
public:
    virtual int write  (std::ostream& os);
    void        perform();
};

class MoveRightPlayerAction : public PlayerAction {
public:
    virtual int write(std::ostream& os);
    virtual void perform();
};

class JumpPlayerAction : public PlayerAction {
public:
    virtual int write(std::ostream& os);
    virtual void perform();
};

class DuckPlayerAction : public PlayerAction {
public:
    virtual int write(std::ostream& os);
    virtual void perform();
};

class KickPlayerAction : public PlayerAction {
public:
    virtual int write(std::ostream& os);
    virtual void perform();
};

class PunchPlayerAction : public PlayerAction {
public:
    virtual int write(std::ostream& os);
    virtual void perform();
};

class BlockLowPlayerAction : public PlayerAction {
public:
    virtual int write(std::ostream& os);
    virtual void perform();
};

class BlockHighPlayerAction : public PlayerAction {
public:
    virtual int write(std::ostream& os);
    virtual void perform();
};


#endif // FIGHT__HXX
