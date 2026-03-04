#include "OnlineStatusChanger.h"

BAKKESMOD_PLUGIN(OnlineStatusChanger, "OnlineStatusChanger", "1.0", 0)

void OnlineStatusChanger::onLoad()
{
    cvarManager->registerCvar("cl_online_status_enable", "1", "Enable Online Status Changer", true, true, 0, true, 1);
    cvarManager->registerCvar("cl_online_status_override", "0", "Overrides online status", true, true, 0, true, 3);

    gameWrapper->HookEvent("Function TAGame.PRI_TA.SetOnlineStatus", std::bind(&OnlineStatusChanger::SetOnlineStatus, this, std::placeholders::_1));
}

void OnlineStatusChanger::onUnload()
{
}

void OnlineStatusChanger::SetOnlineStatus(std::string eventName)
{
    CVarWrapper enableCvar = cvarManager->getCvar("cl_online_status_enable");
    if (!enableCvar || !enableCvar.getBoolValue()) return;

    ServerWrapper server = gameWrapper->GetCurrentGameState();
    if (!server) return;

    PlayerControllerWrapper pc = gameWrapper->GetPlayerController();
    if (!pc) return;

    PriWrapper pri = pc.GetPRI();
    if (!pri) return;

    CVarWrapper statusCvar = cvarManager->getCvar("cl_online_status_override");
    if (!statusCvar) return;

    int statusValue = statusCvar.getIntValue();
    pri.SetOnlineStatus(statusValue);
}