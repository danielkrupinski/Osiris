#pragma once

class KeyBind {
public:
    enum KeyCode {
        A = 0,
        B,
        C,
        INSERT,
        MOUSE1,
        MOUSE2,
        MOUSE3,
        MOUSE4,
        MOUSE5,
        MOUSEWHEEL_DOWN,
        MOUSEWHEEL_UP,
        NONE
    };

    KeyBind(KeyCode keyCode) noexcept;
    KeyBind(const char* keyName) noexcept;

    bool operator==(KeyCode keyCode) const noexcept { return this->keyCode == keyCode; }
    bool operator==(const KeyBind& other) const noexcept { return this->keyCode == other.keyCode; }

    const char* toString() const noexcept;
    bool isPressed() const noexcept;
    bool isDown() const noexcept;

    void setToPressedKey() noexcept;
private:
    KeyCode keyCode;
};
