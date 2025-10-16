#pragma once
#include <atomic>

// A minimal Unity-like base class.
// Derive from this and override Start() and Update(float dt).
class MonoBehaviour {


private:

    bool m_enabled = true;
    bool m_started = false;

public:

    MonoBehaviour() = default;
    virtual ~MonoBehaviour() = default;

    // Called once before the first Update if the component is enabled.
    virtual void Start() {}

    // Called every frame while the component is enabled. dt = delta time in seconds.
    virtual void Update(float dt) {}

    // Optional hooks
    virtual void OnEnable() {}
    virtual void OnDisable() {}
    virtual void OnExit() {}
    // Enable/disable the behaviour at runtime
    void SetEnabled(bool enabled) {

        if (enabled && !m_enabled) 
        {
            m_enabled = true;
            OnEnable();
        }
        else if (!enabled && m_enabled) 
        {
            m_enabled = false;
            OnDisable();
        }
    }

    bool IsEnabled() const { return m_enabled; }

    // internal used by Application to check whether Start() was already called
    bool IsStarted() const { return m_started; }
    void MarkStarted() { m_started = true; }


};
