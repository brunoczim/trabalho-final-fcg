#include "Player.hpp"
#include "blocks.hpp"

Player::Player():
    move_speed(0.2f),
    head_position(glm::vec4(0.0f, WORLD_SIZE_Y / 2.0f + 2.0f, 0.0f, 1.0f))
{
}

void Player::MoveForewards()
{
//    this->head_position += -this->WVector() * this->move_speed;
}

void Player::MoveBackwards()
{
//    this->head_position += this->WVector() * this->move_speed;
}

void Player::MoveLeftwards()
{
//    this->head_position += -this->UVector() * this->move_speed;
}

void Player::MoveRightwards()
{
//    this->head_position += this->UVector() * this->move_speed;
}
