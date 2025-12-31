#pragma once

class Engine;

class GameLayer {
public:
    virtual ~GameLayer() = default;

    virtual void onInit(Engine& engine) {}
    virtual void onFixedUpdate(Engine& engine, float deltaSeconds) { (void)engine; (void)deltaSeconds; }
    virtual void onUpdate(Engine& engine, float deltaSeconds) { (void)engine; (void)deltaSeconds; }
    virtual void onRender(Engine& engine, float deltaSeconds) { (void)engine; (void)deltaSeconds; }
    virtual void onShutdown(Engine& engine) { (void)engine; }
};
