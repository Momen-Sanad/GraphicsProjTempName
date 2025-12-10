#include "Entity.hpp"

// ------------------------------------------------------------
// Destructor -> does NOT delete children (EntityManager owns them)
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

    // remove from old parent if exists
    if (parent) {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    parent = newParent;

    if (parent) {
        // avoid duplicates
        if (std::find(parent->children.begin(), parent->children.end(), this) == parent->children.end())
            parent->children.push_back(this);
    }
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
// Draw entity (uses Material's uniform accessor to set MVP)
// ------------------------------------------------------------
void Entity::draw(const glm::mat4& viewProj) {
    if (material && mesh) {
        // Let the material bind the shader and common uniforms
        material->setup();

        // Compute world matrix + MVP
        glm::mat4 M  = getWorldMatrix();
        glm::mat4 MVP = viewProj * M;

        // Use Material's public method to get uniform location if available.
        // Many of your files use "MVP" as the uniform name in main.cpp — use that.
        GLint loc = -1;
        // Try Material's public uniform getter if it exists
        // (we assume Material exposes getUniform(const std::string&) -> GLint)
        // If not present, you can fallback to accessing shader's uniform (less preferred).
        try {
            loc = material->getUniform("MVP");
        } catch(...) {
            // fallback: try shader directly if your Material exposes it
            if (material->getShader()) {
                loc = material->getShader()->getUniformLocation("MVP");
            }
        }

        if (loc != -1)
            glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);

        mesh->draw();
    }

    // Draw children recursively
    for (Entity* c : children)
        c->draw(viewProj);
}
