#pragma once
#include <map>
#include <memory>
#include <string>
#include "ICommand.h"
#include "IInput.h"
#include "Input.h"

class InputManager {
public:
    InputManager(const Input* input);

    void Update(float deltaTime);
    void BindAction(Key key, const std::string& actionName);
    void BindCommand(const std::string& actionName, std::unique_ptr<ICommand> command);
    bool IsActionActive(const std::string& actionName) const;
    void ProcessCommands(Player& player, float deltaTime);

private:
    const Input* m_Input;
    std::map<Key, std::string> m_KeyToAction;
    std::map<std::string, std::unique_ptr<ICommand>> m_ActionToCommand;
    std::map<std::string, bool> m_ActionStates;
};
