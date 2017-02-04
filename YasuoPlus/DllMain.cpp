#include "PluginSDK.h"
#include "YasuoPlus.hpp"
#include "DllMain.h"
#include "Utils.hpp"


PluginSetup("YasuoPlus");

PLUGIN_EVENT(void) OnRender();
PLUGIN_EVENT(void) OnGameUpdate();
PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args);
PLUGIN_EVENT(void) AfterAttack(IUnit* Source, IUnit* Target);
PLUGIN_EVENT(void) OnDash(UnitDash* Args);

PluginBase *plugin;

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	// Initializes global interfaces for core access
	PluginSDKSetup(PluginSDK);
	Utils::CreateConsoleWindow();
	plugin = new YasuoPlus();

	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, AfterAttack);
	GEventManager->AddEventHandler(kEventOnDash, OnDash);
	plugin->OnLoad();
}

// Called when plugin is unloaded
PLUGIN_API void OnUnload()
{
	plugin->OnUnLoad();
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, AfterAttack);
	GEventManager->RemoveEventHandler(kEventOnDash, OnDash);
	delete plugin;
}

PLUGIN_EVENT(void) OnRender()
{
	plugin->OnRender();
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	plugin->OnGameUpdate();
}

PLUGIN_EVENT(void) AfterAttack(IUnit* Source, IUnit* Target)
{
	plugin->AfterAttack(Source, Target);
}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	plugin->OnGapCloser(Args);
}

PLUGIN_EVENT(void) OnDash(UnitDash* Args)
{
	plugin->OnDash(Args);
}