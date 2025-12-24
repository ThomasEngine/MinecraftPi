#include "InputManager.h"
#include "Player.h"
#include "ICommand.h"
#include "IInput.h"
#include "Input.h"
// hel
InputManager::InputManager(const Input* input)
    : m_Input(input) {
}

// Update based on current input states
void InputManager::Update(float deltaTime) {
    const IKeyboard& keyboard = m_Input->GetKeyboard();
    for (const auto& pair : m_KeyToAction) {
        m_ActionStates[pair.second] = keyboard.GetKey(pair.first);
    }
}

// Bind a key to an action name
void InputManager::BindAction(Key key, const std::string& actionName) {
    m_KeyToAction[key] = actionName;
}

// Bind a command to an action name
void InputManager::BindCommand(const std::string& actionName, std::unique_ptr<ICommand> command) {
    m_ActionToCommand[actionName] = std::move(command);
}

// Check if an action is currently active
bool InputManager::IsActionActive(const std::string& actionName) const {
    auto it = m_ActionStates.find(actionName);
    return it != m_ActionStates.end() && it->second;
}

// Execute commands that are active
void InputManager::ProcessCommands(Player& player, float deltaTime) {
    for (const auto& pair : m_ActionToCommand) {
        if (IsActionActive(pair.first)) {
            pair.second->Execute(player, deltaTime);
        }
    }
}
