#include "EntityManager.hpp"
#include <glm/gtc/matrix_inverse.hpp>

// ------------------------------------------------------------
// Create entity and store it
// ------------------------------------------------------------
Entity* EntityManager::createEntity() {
    // Create a new unique entity and add it to the entities list
    entities.push_back(std::make_unique<Entity>());
    
    // Get a raw pointer to the entity
    Entity* e = entities.back().get();

    // Add the new entity to the roots list by default
    roots.push_back(e); // Default behavior: the entity is considered a root entity
    return e;           // Return the raw pointer to the newly created entity
}

// ------------------------------------------------------------
// Overloaded creator (factory method)
// ------------------------------------------------------------
Entity* EntityManager::createEntityWithParams(
    Entity* parent,
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale,
    MeshRenderer* mesh,
    Material* material
) {
    // Call the original zero-argument entity creation function
    Entity* e = createEntity(); 

    // Set the entity's transform (position, rotation, and scale)
    e->setPosition(position);
    e->setRotation(rotation);
    e->setScale(scale);

    // Assign the mesh and material to the entity
    e->setMesh(mesh);
    e->setMaterial(material);

    // Set the parent of the entity, and remove it from the roots list if it has a parent
    if (parent) {
        e->setParent(parent);  // Set the parent entity
        // Remove from roots list if the entity is no longer a root
        roots.erase(std::remove(roots.begin(), roots.end(), e), roots.end());
    }
    
    return e;  // Return the newly created entity
}

// ------------------------------------------------------------
// Create a default cube (helper function, factory)
// ------------------------------------------------------------
Entity* createCube(
    EntityManager& manager,
    Entity* parent,
    const glm::vec3& pos,
    const glm::vec3& scale,
    const glm::quat& rot,
    MeshRenderer* mesh,
    Material* material
) {
    // Create a cube entity using the factory function with parameters
    return manager.createEntityWithParams(parent, pos, rot, scale, mesh, material);
}

// ------------------------------------------------------------
// Remove entity and unparent it
// ------------------------------------------------------------
void EntityManager::destroyEntity(Entity* entity) {
    if (!entity) return;  // If entity is null, nothing to destroy

    // Remove from the root list if the entity is in it
    roots.erase(std::remove(roots.begin(), roots.end(), entity), roots.end());

    // Unset the parent of the entity (if it has one)
    if (entity->getParent())
        entity->setParent(nullptr);

    // Unset the parent references for all of the entity's children
    for (Entity* child : entity->getChildren())
        child->setParent(nullptr);

    // Remove the entity from the list of managed entities
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [&](const std::unique_ptr<Entity>& e){ return e.get() == entity; }
        ),
        entities.end()
    );
}

// ------------------------------------------------------------
// Clear all entities and reset the root list
// ------------------------------------------------------------
void EntityManager::clear() {
    entities.clear();  // Remove all entities
    roots.clear();     // Remove all root entities
}

// ------------------------------------------------------------
// Draw all root-level entities (children will be drawn recursively)
// ------------------------------------------------------------
void EntityManager::drawAll(const glm::mat4& viewProj) {
    // Iterate through each root entity and draw it
    for (Entity* e : roots)
        e->draw(viewProj);
}

// ------------------------------------------------------------
// Render a single entity recursively (including children)
// ------------------------------------------------------------
void EntityManager::renderEntityRecursive(Entity* e, const glm::mat4& VP)
{
    if (!e) return;

    MeshRenderer* mesh = e->getMesh();
    Material* material = e->getMaterial();

    if (mesh && material) {
        std::shared_ptr<Shader> shader = material->getShader();
        if (!shader) return;

        shader->use();

        // -----------------------------
        // Material (textures, params)
        // -----------------------------
        material->setup();

        // -----------------------------
        // Transforms
        // -----------------------------
        glm::mat4 model = e->getWorldMatrix();

        // New-style (lighting shaders)
        if (shader->hasUniform("viewProj"))
            shader->setUniform("viewProj", VP);

        if (shader->hasUniform("model"))
            shader->setUniform("model", model);

        if (shader->hasUniform("normalMatrix")) {
            glm::mat3 normalMatrix =
                glm::transpose(glm::inverse(glm::mat3(model)));
            shader->setUniform("normalMatrix", normalMatrix);
        }

        // Old-style fallback (MVP shaders)
        if (shader->hasUniform("MVP")) {
            glm::mat4 MVP = VP * model;
            shader->setUniform("MVP", MVP);
        }

        // -----------------------------
        // Draw
        // -----------------------------
        mesh->draw();
    }

    // -----------------------------
    // Skinned meshes
    // -----------------------------
    if (e->hasSkinnedRendering()) {
        SkinnedMaterial* skinned_material = e->getSkinnedMaterial();
        if (skinned_material) {
            GLboolean cull_enabled = glIsEnabled(GL_CULL_FACE);
            if (cull_enabled) {
                glDisable(GL_CULL_FACE);
            }

            skinned_material->setup();

            glm::mat4 model = e->getWorldMatrix();
            glm::mat4 MVP = VP * model;

            GLint mvp_loc = skinned_material->getUniform("MVP");
            if (mvp_loc != -1)
                glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &MVP[0][0]);

            GLint model_loc = skinned_material->getUniform("model");
            if (model_loc != -1)
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);

            GLint animated_loc = skinned_material->getUniform("uIsAnimated");
            if (animated_loc != -1)
                glUniform1i(animated_loc, GL_TRUE);

            for (SkinnedMeshRenderer* renderer : e->getSkinnedRenderers()) {
                if (renderer)
                    renderer->draw();
            }

            if (cull_enabled) {
                glEnable(GL_CULL_FACE);
            }
        }
    }

    // -----------------------------
    // Children
    // -----------------------------
    for (Entity* child : e->getChildren()) {
        renderEntityRecursive(child, VP);
    }
}
