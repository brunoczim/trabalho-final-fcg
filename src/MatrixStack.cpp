#include "MatrixStack.hpp"
#include "matrices.hpp"

void MatrixStack::push(glm::mat4 matrix)
{
    matrices.push(matrix);
}

void MatrixStack::pop(glm::mat4 &matrix)
{
    if (matrices.empty()) {
        matrix = Matrix_Identity();
    } else {
        matrix = matrices.top();
        matrices.pop();
    }
}
