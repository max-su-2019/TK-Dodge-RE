#include "InputEvents.h"
#include "DataHandler.h"
#include "AnimationEvents.h"

namespace TKDodge
{
	std::uint32_t InputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		std::uint32_t index;
		switch (a_key) {
		case Key::kUp:
			index = 0;
			break;
		case Key::kDown:
			index = 1;
			break;
		case Key::kLeft:
			index = 2;
			break;
		case Key::kRight:
			index = 3;
			break;
		case Key::kStart:
			index = 4;
			break;
		case Key::kBack:
			index = 5;
			break;
		case Key::kLeftThumb:
			index = 6;
			break;
		case Key::kRightThumb:
			index = 7;
			break;
		case Key::kLeftShoulder:
			index = 8;
			break;
		case Key::kRightShoulder:
			index = 9;
			break;
		case Key::kA:
			index = 10;
			break;
		case Key::kB:
			index = 11;
			break;
		case Key::kX:
			index = 12;
			break;
		case Key::kY:
			index = 13;
			break;
		case Key::kLeftTrigger:
			index = 14;
			break;
		case Key::kRightTrigger:
			index = 15;
			break;
		default:
			index = kInvalid;
			break;
		}

		return index != kInvalid ? index + kGamepadOffset : kInvalid;
	}


	EventResult InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
	{
		using EventType = RE::INPUT_EVENT_TYPE;
		using DeviceType = RE::INPUT_DEVICE;

		if (!a_event) 
			return EventResult::kContinue;

		const auto ui = RE::UI::GetSingleton();
		const auto controlMap = RE::ControlMap::GetSingleton();
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerControls = RE::PlayerControls::GetSingleton();
		auto datahandler = DataHandler::GetSingleton();

		if (ui->GameIsPaused() || !controlMap->IsMovementControlsEnabled() || !controlMap ->IsFightingControlsEnabled() || !playerCharacter || !playerControls || !playerControls->attackBlockHandler ||
			!playerControls->attackBlockHandler->inputEventHandlingEnabled || !playerControls->movementHandler || !playerControls->movementHandler->inputEventHandlingEnabled) 
			return EventResult::kContinue;
		for (auto event = *a_event; event; event = event->next) {
			if (event->eventType != EventType::kButton)
				continue;

			const auto button = static_cast<RE::ButtonEvent*>(event);
			if (!button->IsPressed())
				continue;

			auto key = button->GetIDCode();
			switch (button->device.get()) {
			case DeviceType::kMouse:
				key += kMouseOffset;
				break;
			case DeviceType::kKeyboard:
				key += kKeyboardOffset;
				break;
			case DeviceType::kGamepad:
				key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
			default:
				continue;
			}

			if (( button->IsPressed()) && key == datahandler->settings->dodgeKey) {
				logger::debug("Dodge Key Press!");
				const std::string dodge_event = DataHandler::GetSingleton()->GetDodgeEvent();
				if (!dodge_event.empty() && playerCharacter->GetWeaponState() == RE::WEAPON_STATE::kDrawn && playerCharacter->GetSitSleepState() == RE::SIT_SLEEP_STATE::kNormal 
					&& playerCharacter->GetKnockState() == RE::KNOCK_STATE_ENUM::kNormal && playerCharacter->GetFlyState() == RE::FLY_STATE::kNone && !playerCharacter->IsSneaking() && !playerCharacter->IsSwimming() 
					&& !playerCharacter->IsStagger() && !playerCharacter->IsInJumpState() && playerCharacter->GetActorValue(RE::ActorValue::kStamina) > datahandler->settings->dodgeStamina){
					logger::debug(FMT_STRING("{} Trigger!"), dodge_event);
					bool IsDodging = false;
					if (playerCharacter->GetGraphVariableBool("bIsDodging", IsDodging) && IsDodging) {
						logger::debug("Player is already dodging!");
						return EventResult::kContinue;
					}

					playerCharacter->SetGraphVariableInt("iStep", datahandler->settings->istepDodge);
					playerCharacter->SetGraphVariableFloat("TKDR_IframeDuration", 0.3f);
					playerCharacter->NotifyAnimationGraph(dodge_event);
					return EventResult::kContinue;
				} else
					logger::debug("No Dodge Event Get!");
			}
		}

		return EventResult::kContinue;
	}

}



