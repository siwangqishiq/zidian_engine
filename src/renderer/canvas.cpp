#include "renderer/canvas.h"
#include "glm/gtc/matrix_transform.hpp"

namespace zidian{
    void ICanvas::resetState(){
        state_ = CanvasState();
        stateStack_.clear();
    }

    int ICanvas::save(){
        stateStack_.push_back(state_);
        return static_cast<int>(stateStack_.size());
    }

    void ICanvas::restore(){
        if (stateStack_.empty()) {
            return;
        }

        state_ = stateStack_.back();
        stateStack_.pop_back();
    }

    void ICanvas::restoreToCount(int count){
        while (static_cast<int>(stateStack_.size()) >= count &&!stateStack_.empty()) {
            restore();
        }//end while
    }

    void ICanvas::translate(float dx, float dy){
        state_.matrix = glm::translate(
            state_.matrix,
            glm::vec3(dx, dy, 0.0f)
        );

        state_.matrixIndex = UINT32_MAX;
    }

    void ICanvas::scale(float sx, float sy){
        state_.matrix = glm::scale(
            state_.matrix,
            glm::vec3(sx, sy, 1.0f)
        );

        state_.matrixIndex = UINT32_MAX;
    }

    void ICanvas::scale(float sx,float sy,float px,float py){
        translate(px, py);
        scale(sx, sy);
        translate(-px, -py);
    }

    void ICanvas::rotate(float degrees){
        state_.matrix = glm::rotate(
            state_.matrix,
            glm::radians(degrees),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        state_.matrixIndex = UINT32_MAX;
    }

    void ICanvas::rotate(float degrees,float px,float py){
        translate(px,py);
        rotate(degrees);
        translate(-px,-py);
    }

    void ICanvas::concat(const glm::mat4& matrix){
        state_.matrix *= matrix;
        state_.matrixIndex = UINT32_MAX;
    }
    
    void ICanvas::setMatrix(const glm::mat4& matrix){
        state_.matrix = matrix;
        state_.matrixIndex = UINT32_MAX;
    }

    uint32_t ICanvas::getMatrixIndex(){
        if (state_.matrixIndex == UINT32_MAX) {
            state_.matrixIndex = addTransform(state_.matrix);
        }

        return state_.matrixIndex;
    }
}