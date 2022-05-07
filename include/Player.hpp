#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/vec4.hpp>

class Player
{
private:
    float move_speed;
    glm::vec4 head_position;
public:
    Player();

    void MoveForewards();
    void MoveBackwards();
    void MoveLeftwards();
    void MoveRightwards();
};

#endif // PLAYER_HPP
