#pragma once
#include "PluginSDK.h"

using namespace std;

class Menu
{
private:
	IMenu* MainMenu;
	IMenu* Combo;
	IMenu* Flee;
	IMenu* Drawing;
	IMenu* LaneClear;
	IMenu* LastHit;
	IMenu* Harass;
	IMenu* Misc;
	IMenu* WindWall;
public:
	//Combo
	IMenuOption* ComboEPathfinder;
	IMenuOption* ComboRUse;
	IMenuOption* ComboREnemiesCount;
	IMenuOption* ComboRUseOnLanding;
	IMenuOption* ComboDontTowerDive;
	//Flee
	IMenuOption* FleeFleeKey;
	IMenuOption* FleeFleeStack;
	//Drawing
	IMenuOption* DrawingQ;
	IMenuOption* DrawingE;
	IMenuOption* DrawingR;
	//LastHit
	IMenuOption* LastHitQ;
	IMenuOption* LastHitE;
	//LaneClear
	IMenuOption* LaneClearQ;
	IMenuOption* LaneClearE;
	//Harass
	IMenuOption* HarassQ;
	IMenuOption* HarassUnderTower;


	Menu()
	{
		MainMenu = GPluginSDK->AddMenu("YasuoPlus");
		Combo = MainMenu->AddMenu("Combo");
		Flee = MainMenu->AddMenu("Flee");
		Drawing = MainMenu->AddMenu("Drawing");
		LaneClear = MainMenu->AddMenu("LaneClear");
		LastHit = MainMenu->AddMenu("LastHit");
		Harass = MainMenu->AddMenu("Harass");
		WindWall = MainMenu->AddMenu("WindWall");

		//Combo
		ComboEPathfinder = Combo->CheckBox("Use W pathfinder", true);
		ComboRUse = Combo->CheckBox("Use smart R", true); 
		ComboREnemiesCount = Combo->AddInteger("R enemies count", 1, 5, 2);
		ComboRUseOnLanding = Combo->CheckBox("R use on landing", true);
		ComboDontTowerDive = Combo->CheckBox("E dont tower dive", true);

		//Flee
		FleeFleeKey = Flee->AddKey("Flee button", 'Z');
		FleeFleeStack = Flee->CheckBox("Flee stack Q", true);

		//Drawing
		DrawingQ = Drawing->CheckBox("Draw Q range", true);
		DrawingE = Drawing->CheckBox("Draw E range", true);
		DrawingR = Drawing->CheckBox("Draw R range", true);

		//LastHit
		LastHitQ = LastHit->CheckBox("Use Q", true);
		LastHitE = LastHit->CheckBox("Use E", true);

		//LaneClear
		LaneClearQ = LaneClear->CheckBox("Use Q", true);
		LaneClearE = LaneClear->CheckBox("Use E", true);

		//Harass
		HarassQ = Harass->CheckBox("Use Q", true);
		HarassUnderTower = Harass->CheckBox("Harass under tower", false);
	}

	~Menu()
	{
		MainMenu->Remove();
	}

};