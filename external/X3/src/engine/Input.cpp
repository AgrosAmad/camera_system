#include "Input.h"

Input& Input::Inst()
{
    static Input inp;
    return inp;
}

void Input::ClearAll()
{
    ClearKeysDown();
    ClearKeysPressed();
    ClearButtonsDown();
    ClearButtonsPressed();
    ClearMouseMotion();
    ClearMouseScrollMotion();
}

void Input::KeyCallback(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) KeyPressed(key, true);

    bool key_down = false;

    if (action == GLFW_PRESS) key_down = true;
    else if (action == GLFW_RELEASE) key_down = false;
    else return;

    KeyDown(key, key_down);
}

void Input::MouseButtonCallback(int button, int action, int mods)
{
    if (action == GLFW_PRESS) ButtonPressed(button, true);

    bool button_down = false;

    if (action == GLFW_PRESS) button_down = true;
    else if (action == GLFW_RELEASE) button_down = false;
    else return;

    ButtonDown(button, button_down);
}

void Input::MouseScrollCallback(double x, double y)
{
    mMouseScrollX = x;
    mMouseScrollY = y;
}

void Input::MouseCallback(double x, double y)
{
    mMouseXPrev = mMouseX;
    mMouseYPrev = mMouseY;

    mMouseX = x;
    mMouseY = y;
}

bool Input::MouseHoveredRec(const glm::vec2& rMin, const glm::vec2& rMax)
{
    return mMouseX >= rMin.x && mMouseX <= rMax.x && mMouseY >= rMin.y && mMouseY <= rMax.y;
}
