#ifndef APPLICATION
#define APPLICATION
#pragma once

#include "Camera.hpp"

class Application
{
public:
    void init();
    void tick();
    void shutdown();
    Camera camera;
};

#endif // APPLICATION_H
