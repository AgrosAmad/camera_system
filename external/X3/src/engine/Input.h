#pragma once

// STL
#include <cstring>

// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Singleton class to manage user inputs
class Input
{
public:

    // Calls the only instance
    static Input& Inst();
    
    // Clear inputs
    void ClearButtonsPressed() { memset(mButtonsPressed, 0, GLFW_MOUSE_BUTTON_LAST * sizeof(bool)); }
    void ClearButtonsDown() { memset(mButtons, 0, GLFW_MOUSE_BUTTON_LAST * sizeof(bool)); }
    void ClearKeysPressed() { memset(mKeysPressed, 0, GLFW_KEY_LAST * sizeof(bool)); }
    void ClearKeysDown() { memset(mKeysDown, 0, GLFW_KEY_LAST * sizeof(bool)); }
    void ClearMouseMotion() { mMouseXPrev = mMouseX; mMouseYPrev = mMouseY; }
    void ClearMouseScrollMotion() { mMouseScrollX = 0; mMouseScrollY = 0; }
    void ClearAll();

    // Callbacks
    void KeyCallback(int key, int scancode, int action, int mods);
    void MouseButtonCallback(int button, int action, int mods);
    void MouseScrollCallback(double x, double y);
    void MouseCallback(double x, double y);

    // Getters/Setters
    void KeyPressed(int k, bool state) { mKeysPressed[k] = state; }
    bool KeyPressed(int k) { return mKeysPressed[k]; }

    void KeyDown(int k, bool state) { mKeysDown[k] = state; }
    bool KeyDown(int k) { return mKeysDown[k]; }

    void ButtonPressed(int k, bool state) { mButtonsPressed[k] = state; }
    bool ButtonPressed(int k) { return mButtonsPressed[k]; }

    void ButtonDown(int b, bool state) { mButtons[b] = state; }
    bool ButtonDown(int b) { return mButtons[b]; }

    double MouseDeltaX() { return mMouseX - mMouseXPrev; }
    double MouseDeltaY() { return mMouseY - mMouseYPrev; }

    double MouseScrollX() { return mMouseScrollX; }
    double MouseScrollY() { return mMouseScrollY; }

    double MouseXprev() { return mMouseXPrev; }
    double MouseYprev() { return mMouseYPrev; }

    double MouseX() { return mMouseX; }
    double MouseY() { return mMouseY; }

    // Other methods
    bool MouseHoveredRec(const glm::vec2& rMin, const glm::vec2& rMax);
private:

    // Cache of inputs
    bool mButtonsPressed[GLFW_MOUSE_BUTTON_LAST];
    bool mButtons[GLFW_MOUSE_BUTTON_LAST];
    bool mKeysPressed[GLFW_KEY_LAST];
    bool mKeysDown[GLFW_KEY_LAST];

    // For mouse inputs
    double mMouseScrollX;
    double mMouseScrollY;
    double mMouseXPrev;
    double mMouseYPrev;
    double mMouseX;
    double mMouseY;

    // Private constructor to make class singleton
    Input() : mMouseX(0), mMouseY(0)
    {
        ClearAll();
    }
    Input(const Input&) = delete; // No copy constructor allowed
    void operator=(const Input&) = delete; // No copy assignment allowed

};