#pragma once

struct HealthComponent {
    int maxHP = 100;
    int hp = 100;
    float invulnTimer = 0.3f; // seconds left of invulnerability after hit
    float invulnDuration = 0.5f;
};
