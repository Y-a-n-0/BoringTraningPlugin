#pragma once
#include<iostream>
#pragma comment (lib,"pluginsdk")
#include"bakkesmod/plugin/bakkesmodplugin.h"
#include"bakkesmod/plugin/bakkesmodsdk.h"
#include"bakkesmod/imgui/imgui.h"
#include"bakkesmod/imgui/imgui_searchablecombo.h"
#include"bakkesmod/imgui/imgui_rangeslider.h"
#include"bakkesmod/imgui/imconfig.h"
#include"bakkesmod/plugin/PluginSettingsWindow.h"
#include"bakkesmod/plugin/pluginwindow.h"

#define PI 3.1415926



class BoringTraningPlugin : public BakkesMod::Plugin::BakkesModPlugin , public BakkesMod::Plugin::PluginSettingsWindow
{
public:
	void onLoad()override;
	void onUnload()override;


	void SetCarRotation();

	void SlomotionWhenOnAir();


	void RenderSettings()override;
	std::string GetPluginName()override;
	void SetImGuiContext(uintptr_t ctx)override;


private:
	Vector Loc_Ball;
	Vector Loc_Car;
	Vector Pointing;
	Rotator Rot_Car;

	float horizontalDistance;

	float m_Yaw;
	float m_Pitch;
	float m_Roll = 0.0f;

	bool IsKeepPointing;
	bool PointAT = false;
private:
	bool IsOnAir = false;
	float GameSpeed;
	bool IsStartSlomo;
};

