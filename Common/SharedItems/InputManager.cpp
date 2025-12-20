#include "InputManager.h"

InputManager::InputManager(const Input* input)
    : m_Input(input) {
}

void InputManager::Update(float /*deltaTime*/) {
    const IKeyboard& keyboard = m_Input->GetKeyboard();
    for (const auto& pair : m_KeyToAction) {
        m_ActionStates[pair.second] = keyboard.GetKey(pair.first);
    }
}

void InputManager::BindAction(Key key, const std::string& actionName) {
    m_KeyToAction[key] = actionName;
}

void InputManager::BindCommand(const std::string& actionName, std::unique_ptr<ICommand> command) {
    m_ActionToCommand[actionName] = std::move(command);
}

bool InputManager::IsActionActive(const std::string& actionName) const {
    auto it = m_ActionStates.find(actionName);
    return it != m_ActionStates.end() && it->second;
}

void InputManager::ProcessCommands(Player& player, float deltaTime) {
    for (const auto& pair : m_ActionToCommand) {
        if (IsActionActive(pair.first)) {
            pair.second->Execute(player, deltaTime);
        }
    }
}
