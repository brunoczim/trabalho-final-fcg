#ifndef MATRIX_STACK_HPP
#define MATRIX_STACK_HPP

#include <stack>
#include <glm/mat4x4.hpp>

class MatrixStack {
private:
    std::stack<glm::mat4> matrices;

public:
    void push(glm::mat4 matrix);

    void pop(glm::mat4 &matrix);
};

#endif // MATRIX_STACK_HPP
