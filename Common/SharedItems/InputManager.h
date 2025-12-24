#pragma once
#include <map>
#include <memory>
#include <string>

enum class Key;
enum class MouseButtons;
class Input;
class ICommand;
class Player;
class InputManager {
public:
    InputManager(const Input* input);

    void Update(float deltaTime);
    void BindAction(Key key, const std::string& actionName);
    void BindAction(MouseButtons button, const std::string& actionName);
    void BindCommand(const std::string& actionName, std::unique_ptr<ICommand> command);
    bool IsKeyboardActionActive(const std::string& actionName) const;
	bool IsMouseActionActive(const std::string& actionName) const;
    void ProcessCommands(Player& player, float deltaTime);

private:
    const Input* m_Input;
    std::map<Key, std::string> m_KeyToAction;
    std::map<std::string, std::unique_ptr<ICommand>> m_ActionToCommand;
	std::map<MouseButtons, std::string> m_MouseButtonToAction;
    std::map<std::string, bool> m_MouseActionStates;
    std::map<std::string, bool> m_KeyboardActionStates;
};
