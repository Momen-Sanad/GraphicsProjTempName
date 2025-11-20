#ifndef MONOBEHAVIOUR_HPP
#define MONOBEHAVIOUR_HPP

class MonoBehaviour {
public:
    virtual ~MonoBehaviour() = default;
    virtual void Start() = 0;
    virtual void Update(float dt) = 0;
    virtual void OnExit() = 0;
};

#endif // MONOBEHAVIOUR_HPP
