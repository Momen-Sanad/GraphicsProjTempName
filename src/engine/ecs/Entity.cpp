#include "Entity.hpp"
#include "AnimationComponent.hpp"

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
    
    // Clear skinned renderers (we don't own them, just references)
    skinned_renderers.clear();
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
// Model Data management
// ------------------------------------------------------------
void Entity::setModelData(std::shared_ptr<ModelData> data) {
    model_data = data;
}

void Entity::addSkinnedRenderer(SkinnedMeshRenderer* renderer) {
    if (renderer) {
        skinned_renderers.push_back(renderer);
    }
}

void Entity::clearSkinnedRenderers() {
    skinned_renderers.clear();
}

void Entity::setupSkinnedModel(std::shared_ptr<ModelData> data, SkinnedMaterial* mat) {
    model_data = data;
    skinned_material = mat;
    
    if (!data) return;
    
    // Clear existing skinned renderers
    clearSkinnedRenderers();
    
    // Create and upload renderers for each mesh
    for (auto& skinned_mesh : data->meshes) {
        SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer();
        renderer->upload(skinned_mesh);
        addSkinnedRenderer(renderer);
    }
}

AnimationComponent* Entity::getAnimationComponent() {
    return getComponent<AnimationComponent>();
}

const AnimationComponent* Entity::getAnimationComponent() const {
    return getComponent<AnimationComponent>();
}

// ------------------------------------------------------------
// Draw entity (uses Material's uniform accessor to set MVP)
// ------------------------------------------------------------
void Entity::draw(const glm::mat4& viewProj) {
    // Draw basic mesh if available
    if (material && mesh) {
        // Let the material bind the shader and common uniforms
        material->setup();

        // Compute world matrix + MVP
        glm::mat4 M  = getWorldMatrix();
        glm::mat4 MVP = viewProj * M;

        // Use Material's public method to get uniform location if available.
        GLint loc = -1;
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

    // Draw skinned meshes if available
    drawSkinned(viewProj);

    // Draw children recursively
    for (Entity* c : children)
        c->draw(viewProj);
}

// ------------------------------------------------------------
// Draw skinned meshes
// ------------------------------------------------------------
void Entity::drawSkinned(const glm::mat4& viewProj) {
    if (!hasSkinnedRendering()) return;

    // Setup material (this will also set bone matrices)
    skinned_material->setup();

    // Compute world matrix + MVP
    glm::mat4 M = getWorldMatrix();
    glm::mat4 MVP = viewProj * M;

    // Set MVP uniform
    GLint mvp_loc = skinned_material->getUniform("MVP");
    if (mvp_loc != -1) {
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &MVP[0][0]);
    }

    // Set model matrix uniform (needed for lighting calculations)
    GLint model_loc = skinned_material->getUniform("model");
    if (model_loc != -1) {
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &M[0][0]);
    }

    // Set uIsAnimated uniform
    GLint animated_loc = skinned_material->getUniform("uIsAnimated");
    if (animated_loc != -1) {
        glUniform1i(animated_loc, GL_TRUE);
    }

    // Draw all skinned mesh renderers
    for (SkinnedMeshRenderer* renderer : skinned_renderers) {
        if (renderer) {
            renderer->draw();
        }
    }
}

// ------------------------------------------------------------
// Add component to the entity
// ------------------------------------------------------------
void Entity::addComponent(Component* component) {
    if (component) {
        components.push_back(component);
    }
}

// ------------------------------------------------------------
// Remove component from the entity
// ------------------------------------------------------------
void Entity::removeComponent(Component* component) {
    components.erase(
        std::remove(components.begin(), components.end(), component),
        components.end()
    );
}

// ------------------------------------------------------------
// Update components and propagate to children
// ------------------------------------------------------------
void Entity::updateComponents(float deltaTime) {
    for (Component* c : components) {
        c->update(*this, deltaTime);
    }
    
    // Update children recursively
    for (Entity* child : children) {
        child->updateComponents(deltaTime);
    }
}

