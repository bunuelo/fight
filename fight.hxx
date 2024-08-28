#ifndef FIGHT__HXX
#define FIGHT__HXX

class PlayerAction;

class Player {
public:
    int index;
    int joystick_index;
    float x, y;
    float vx, vy;
    float r, g, b;
    bool on_ground;
    
    Player();
    const char*   name        ();
    PlayerAction* get_action  ();
    int           write_vertices(GLfloat* vertices);
};

class Universe {
public:
    
    Player* players[2];
    GLuint program_object;
    
    bool initialize();
    void destroy();
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


#endif // FIGHT__HXX
