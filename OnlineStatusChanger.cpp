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

    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&StatusOverrider::OnMatchEnd, this, std::placeholders::_1));
    
    gameWrapper->RegisterDrawable(std::bind(&StatusOverrider::Render, this, std::placeholders::_1));
}

void StatusOverrider::OnMatchEnd(std::string eventName)
{
    ServerWrapper server = gameWrapper->GetCurrentGameState();
    if (!server) return;

    TeamWrapper winner = server.GetMatchWinner();
    if (!winner) return;

    auto pri = gameWrapper->GetPlayerController().GetPRI();
    if (!pri) return;

    int team = pri.GetTeamNum();
    int winnerTeam = winner.GetTeamNum();

    if (team == winnerTeam) {
        stats.totalWins++;
        stats.streak = (stats.streak < 0) ? 1 : stats.streak + 1;
    } else {
        stats.totalLosses++;
        stats.streak = (stats.streak > 0) ? -1 : stats.streak - 1;
    }
}

void StatusOverrider::onUnload() {}

void StatusOverrider::Render(CanvasWrapper canvas)
{
    if (!cvarManager->getCvar("mmr_enabled").getBoolValue()) return;

    int x = cvarManager->getCvar("mmr_x_pos").getIntValue();
    int y = cvarManager->getCvar("mmr_y_pos").getIntValue();
    float scale = cvarManager->getCvar("mmr_scale").getFloatValue();
    int opacity = cvarManager->getCvar("mmr_opacity").getIntValue();

    canvas.SetPosition(Vector2{x, y});
    canvas.SetColor(0, 0, 0, (char)opacity);
    canvas.FillBox(Vector2{(int)(200 * scale), (int)(120 * scale)});

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(10 * scale)});
    canvas.SetColor(255, 255, 255, 255);
    canvas.DrawString("MMR Tracker By Baluuu._.", scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(40 * scale)});
    if (stats.streak > 0) canvas.SetColor(0, 255, 0, 255);
    else if (stats.streak < 0) canvas.SetColor(255, 0, 0, 255);
    else canvas.SetColor(255, 255, 255, 255);
    canvas.DrawString("Streak: " + std::to_string(stats.streak), scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(70 * scale)});
    canvas.SetColor(0, 150, 255, 255);
    canvas.DrawString("Wins: " + std::to_string(stats.totalWins), scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(95 * scale)});
    canvas.SetColor(255, 50, 50, 255);
    canvas.DrawString("Losses: " + std::to_string(stats.totalLosses), scale, scale);
}
