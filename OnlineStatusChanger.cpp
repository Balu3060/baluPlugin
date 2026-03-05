#include "OnlineStatusChanger.h"

BAKKESMOD_PLUGIN(StatusOverrider, "MMR tracker By Baluuu._.", "1.0", 0)

void StatusOverrider::onLoad()
{
    cvarManager->registerCvar("mmr_enabled", "1", "Enable Tracker", true, true, 0, true, 1);
    cvarManager->registerCvar("mmr_save_progress", "0", "Save across sessions", true, true, 0, true, 1);
    cvarManager->registerCvar("mmr_x_pos", "100", "X Position", true, true, 0, true, 2000);
    cvarManager->registerCvar("mmr_y_pos", "100", "Y Position", true, true, 0, true, 2000);
    cvarManager->registerCvar("mmr_scale", "1.0", "Scale", true, true, 0.5, true, 5.0);
    cvarManager->registerCvar("mmr_opacity", "150", "Bg Opacity", true, true, 0, true, 255);
    cvarManager->registerCvar("mmr_rounding", "5", "Corner Rounding", true, true, 0, true, 50);

    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchFinished", std::bind(&StatusOverrider::OnMatchEnd, this, std::placeholders::_1));
    
    gameWrapper->RegisterDrawable(std::bind(&StatusOverrider::Render, this, std::placeholders::_1));
}

void StatusOverrider::OnMatchEnd(std::string eventName)
{
    ServerWrapper server = gameWrapper->GetCurrentGameState();
    if (!server) return;

    auto pri = gameWrapper->GetPlayerController().GetPRI();
    if (!pri) return;

    int team = pri.GetTeamNum();
    int winner = server.GetMatchWinner().GetTeamNum();

    if (team == winner) {
        stats.totalWins++;
        stats.streak = (stats.streak < 0) ? 1 : stats.streak + 1;
    } else {
        stats.totalLosses++;
        stats.streak = (stats.streak > 0) ? -1 : stats.streak - 1;
    }
}

void StatusOverrider::onUnload() {}
