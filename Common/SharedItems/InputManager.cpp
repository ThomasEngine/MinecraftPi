#include "InputManager.h"
#include "Player.h"
#include "ICommand.h"
#include "IInput.h"
#include "Input.h"
// helfff
InputManager::InputManager(const Input* input)
    : m_Input(input) {
}

// Update based on current input states
void InputManager::Update(float deltaTime) {
    const IKeyboard& keyboard = m_Input->GetKeyboard();
    const IMouse& mouse = m_Input->GetMouse();

    // Keyboard actions
    for (const auto& pair : m_KeyToAction) {
        m_KeyboardActionStates[pair.second] = keyboard.GetKey(pair.first);
    }

    // Mouse button actions
    for (const auto& pair : m_MouseButtonToAction) {
        m_MouseActionStates[pair.second] = mouse.GetButtonDown(pair.first);
    }
}


// Bind a key to an action name
void InputManager::BindAction(Key key, const std::string& actionName) {
    m_KeyToAction[key] = actionName;
}
void InputManager::BindAction(MouseButtons button, const std::string& actionName) {
	m_MouseButtonToAction[button] = actionName;
}

// Bind a command to an action name
void InputManager::BindCommand(const std::string& actionName, std::unique_ptr<ICommand> command) {
    m_ActionToCommand[actionName] = std::move(command);
}

// Check if an action is currently active
bool InputManager::IsKeyboardActionActive(const std::string& actionName) const {
    auto it = m_KeyboardActionStates.find(actionName);
    return it != m_KeyboardActionStates.end() && it->second;
}

bool InputManager::IsMouseActionActive(const std::string& actionName) const
{
	auto it = m_MouseActionStates.find(actionName);
	if (it == m_MouseActionStates.end()) printf("Action not found: %s\n", actionName.c_str());
    return  (it != m_MouseActionStates.end() && it->second);
}

// Execute commands that are active
void InputManager::ProcessCommands(Player& player, float deltaTime) {
    // 
    for (const auto& pair : m_ActionToCommand) {
        if (IsKeyboardActionActive(pair.first)) {
            pair.second->Execute(player, deltaTime);
        }
    }

}
