#include "pch.h"
#include "BoringTraningPlugin.h"

using namespace std;

BAKKESMOD_PLUGIN(BoringTraningPlugin, "Boring traning plugin", "0.6", PLUGINTYPE_FREEPLAY)

void BoringTraningPlugin::onLoad()
{
	cvarManager->log("plugin on");

	/*****************************************************************************************/

	cvarManager->setBind("F", "PointCar");

	cvarManager->registerCvar("KeepPointing", "0.0f", "Keep the car pointing to ball");

	cvarManager->registerNotifier("PointCar", [this](vector<string> command) {
		if (PointAT) PointAT = false;
		else PointAT = true;

		
		this->SetCarRotation();

		}, "Point the car head to ball", 0);

	/*****************************************************************************************/

	cvarManager->registerCvar("OnAirGameSpeed", "0.5f", "Speed when you are on the air");

	cvarManager->registerCvar("IsStartSlomo", "0.0f", "If is start on air slow motion");

	this->SlomotionWhenOnAir();
}

void BoringTraningPlugin::onUnload()
{
	cvarManager->removeBind("F");
	cvarManager->removeNotifier("PointCar");
	cvarManager->removeCvar("KeepPointing");
	cvarManager->removeCvar("OnAirGameSpeed");
	cvarManager->removeCvar("IsStartSlomo");
}

void BoringTraningPlugin::SetCarRotation()
{
	if (PointAT || !IsKeepPointing && gameWrapper->IsInFreeplay())
	{
		auto GameWP = gameWrapper;

		auto ServerWP = GameWP->GetGameEventAsServer();

		if (!GameWP->IsInFreeplay())
			return;

		if (ServerWP.GetGameBalls().Count() == 0)
			return;


		if (GameWP->GetGameEventAsServer().GetGameBalls().Count() == 0)return;

		auto BallWP = ServerWP.GetGameBalls().Get(0);
		if (BallWP.IsNull())return;
		auto CarWP = ServerWP.GetGameCar();
		if (CarWP.IsNull())return;

		this->Loc_Ball = BallWP.GetLocation();
		this->Loc_Car = CarWP.GetLocation();


		this->Pointing = Loc_Ball - Loc_Car;

		m_Yaw = atan2f(Pointing.Y, Pointing.X) * (180.0f / PI);

		horizontalDistance = sqrtf(Pointing.X * Pointing.X + Pointing.Y * Pointing.Y);
		m_Pitch = atan2f(Pointing.Z, horizontalDistance) * (180.0f / PI);

		this->Rot_Car.Pitch = (short)(m_Pitch * 182.044444f);
		this->Rot_Car.Yaw = (short)(m_Yaw * 182.044444f);
		this->Rot_Car.Roll = CarWP.GetRotation().Roll;

		CarWP.SetRotation(Rot_Car);
		CarWP.SetAngularVelocity({ 0.0f,0.0f,0.0f }, false);

		if (IsKeepPointing)
		{
			gameWrapper->SetTimeout([this](GameWrapper* gp)
				{
					this->SetCarRotation();
				}, 0.01f);
		}

		this->cvarManager->log("Rotating");
	}
	else return;
}

void BoringTraningPlugin::SlomotionWhenOnAir()
{
	if (gameWrapper->IsInFreeplay())
	{
		auto Car = gameWrapper->GetGameEventAsServer().GetCars().Get(0);
		auto GameWP = gameWrapper;

		if (!GameWP->IsInFreeplay())return;
		if (GameWP->GetGameEventAsServer().GetGameBalls().Count() == 0) return;

		GameSpeed = cvarManager->getCvar("OnAirGameSpeed").getFloatValue();

		if (!Car.IsOnGround() && !Car.IsOnWall())
		{
			IsOnAir = true;
		}
		else IsOnAir = false;

		if (IsOnAir && IsStartSlomo)
		{
			GameWP->GetGameEventAsServer().SetGameSpeed(this->GameSpeed);
		}
		else GameWP->GetGameEventAsServer().SetGameSpeed(1.0f);

		GameWP->SetTimeout([this](GameWrapper* GW) {
			this->SlomotionWhenOnAir();
			}, 0.01f);
	}
	else return;
}

void BoringTraningPlugin::RenderSettings()
{
	ImGui::TextUnformatted("Chose any boring stuff you want");
	ImGui::TextUnformatted("You can go to \"Bindings\" seting up hotkeys");
	ImGui::TextUnformatted("------------------------------------------------- ");
	ImGui::TextUnformatted("Pointing car");

	if (ImGui::Button("Point To Ball"))
	{
		gameWrapper->Execute([this](GameWrapper* gp)
			{
				this->SetCarRotation();
			});
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Point the car head to ball");
	}
	ImGui::TextUnformatted("Press To Keep Pointing At Ball,Press Again To Stop Pointing");
	CVarWrapper KeepPointing = cvarManager->getCvar("KeepPointing");
	if (!KeepPointing)return;
	this->IsKeepPointing = KeepPointing.getBoolValue();
	if (ImGui::Checkbox("Keep Pointing To Car", &IsKeepPointing))
	{
		KeepPointing.setValue(IsKeepPointing);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Keep pointing the car to the ball while press the hotkey");
	}

	ImGui::TextUnformatted("------------------------------------------------- ");
	ImGui::TextUnformatted("On Air Slow Motion");

	CVarWrapper IsStartSlomoCvar = cvarManager->getCvar("IsStartSlomo");
	if (!IsStartSlomoCvar)return;
	this->IsStartSlomo = IsStartSlomoCvar.getBoolValue();
	if (ImGui::Checkbox("Start On Air Slow Motion", &IsStartSlomo))
	{
		IsStartSlomoCvar.setValue(IsStartSlomo);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Start slow motion when car is on the air");
	}

	CVarWrapper GameSpeedOnAir = cvarManager->getCvar("OnAirGameSpeed");
	if (!GameSpeedOnAir)return;
	GameSpeed = GameSpeedOnAir.getFloatValue();

	if (ImGui::SliderFloat("Game Speed On Air", &GameSpeed, 0.0f, 5.0f))
	{
		GameSpeedOnAir.setValue(GameSpeed);
	}
	if (ImGui::IsItemHovered())
	{
		string Introduction = "GameSpeed now is" + to_string(GameSpeed);
		ImGui::SetTooltip(Introduction.c_str());
	}

	if (ImGui::Button("Set Default"))
	{
		gameWrapper->Execute([this](GameWrapper* GW)
			{
				CVarWrapper GameSpeedOnAir = cvarManager->getCvar("OnAirGameSpeed");
				GameSpeedOnAir.setValue(0.5f);
			});
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Set GameSpeed to half normal speed");
	}

}

string BoringTraningPlugin::GetPluginName()
{
	return "Boring Traning Plugin";
}

void BoringTraningPlugin::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}
