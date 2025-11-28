#include "Entity.hpp"

// ------------------------------------------------------------
// Destructor → does NOT delete children (EntityManager owns them)
// ------------------------------------------------------------
Entity::~Entity() {
    // detach from parent
    if (parent) {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    parent = nullptr;
    children.clear();
}

// ------------------------------------------------------------
// Set parent
// ------------------------------------------------------------
void Entity::setParent(Entity* newParent) {
    if (parent == newParent) return;

    // remove from old parent
    if (parent) {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    parent = newParent;

    if (newParent)
        newParent->children.push_back(this);
}

// ------------------------------------------------------------
// Local matrix: T * R * S
// ------------------------------------------------------------
glm::mat4 Entity::getLocalMatrix() const {
    glm::mat4 T = glm::translate(glm::mat4(1.f), position);
    glm::mat4 R = glm::mat4_cast(rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
    return T * R * S;
}

// ------------------------------------------------------------
// Recursive world matrix
// ------------------------------------------------------------
glm::mat4 Entity::getWorldMatrix() const {
    if (parent)
        return parent->getWorldMatrix() * getLocalMatrix();
    else
        return getLocalMatrix();
}

// ------------------------------------------------------------
// Draw entity
// ------------------------------------------------------------
void Entity::draw(const glm::mat4& viewProj) {
    if (material && mesh) {
        material->setup();

        glm::mat4 M  = getWorldMatrix();
        glm::mat4 MVP = viewProj * M;

        GLint loc = material->getShader()->getUniformLocation("uMVP");
        if (loc != -1)
            glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);

        mesh->draw();
    }

    // Draw children
    for (Entity* c : children)
        c->draw(viewProj);
}
