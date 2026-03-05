#include "OnlineStatusChanger.h"
#include <cstdio>
#include <fstream>

BAKKESMOD_PLUGIN(StatusOverrider, "MMR tracker By Baluuu._.", "1.0", 0)

    

void StatusOverrider::onLoad()
{
    cvarManager->registerCvar("mmr_enabled", "1", "", true, true, 0, true, 1);
    cvarManager->registerCvar("mmr_save_progress", "0", "", true, true, 0, true, 1);
    cvarManager->registerCvar("mmr_x_pos", "100", "X Position", true, true, 0, false, 0);
    cvarManager->registerCvar("mmr_y_pos", "100", "Y Position", true, true, 0, false, 0);
    cvarManager->registerCvar("mmr_scale", "1.0", "", true, true, 0.5, true, 5.0);
    cvarManager->registerCvar("mmr_opacity", "150", "", true, true, 0, true, 255);
    cvarManager->registerCvar("mmr_rounding", "5", "", true, true, 0, true, 50);
    cvarManager->registerNotifier("mmr_reset_pos", [this](std::vector<std::string> args) {
        cvarManager->getCvar("mmr_x_pos").setValue(100);
        cvarManager->getCvar("mmr_y_pos").setValue(100);
    }, "Resets tracker position to default", PERMISSION_ALL);
    
    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&StatusOverrider::OnMatchEnd, this, std::placeholders::_1));
    gameWrapper->RegisterDrawable(std::bind(&StatusOverrider::Render, this, std::placeholders::_1));

    LoadData();
    PollMMR();
}

void StatusOverrider::onUnload()
{
    SaveData();
}

void StatusOverrider::SaveData()
{
    std::filesystem::path saveFile = gameWrapper->GetDataFolder() / "mmr_tracker_save.txt";
    
    if (!cvarManager->getCvar("mmr_save_progress").getBoolValue()) {
        if (std::filesystem::exists(saveFile)) {
            std::filesystem::remove(saveFile);
        }
        stats = {0, 0, 0};
        sessionMMRChange = 0.0f;
        return;
    }
    
    std::ofstream out(saveFile);
    if (out.is_open()) {
        out << stats.totalWins << " " << stats.totalLosses << " " << stats.streak << " " << sessionMMRChange;
        out.close();
    }
}

void StatusOverrider::LoadData()
{
    if (!cvarManager->getCvar("mmr_save_progress").getBoolValue()) return;

    std::filesystem::path saveFile = gameWrapper->GetDataFolder() / "mmr_tracker_save.txt";
    std::ifstream in(saveFile);
    if (in.is_open()) {
        in >> stats.totalWins >> stats.totalLosses >> stats.streak >> sessionMMRChange;
        in.close();
    }
}

void StatusOverrider::PollMMR()
{
    UpdateMMR();
    gameWrapper->SetTimeout([this](GameWrapper* gw) {
        PollMMR();
    }, 2.0f);
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
    
    SaveData();
}

void StatusOverrider::UpdateMMR()
{
    MMRWrapper mmrWrapper = gameWrapper->GetMMRWrapper();
    int playlist = mmrWrapper.GetCurrentPlaylist();
    auto uid = gameWrapper->GetUniqueID();

    float currentMMR = mmrWrapper.GetPlayerMMR(uid, playlist);
    if (currentMMR <= 0) return;

    if (lastKnownMMR < 0 || playlist != lastPlaylist) {
        lastKnownMMR = currentMMR;
        lastPlaylist = playlist;
    } else {
        float change = currentMMR - lastKnownMMR;
        if (change != 0) {
            sessionMMRChange += change;
            lastKnownMMR = currentMMR;
            SaveData();
        }
    }
}

void StatusOverrider::Render(CanvasWrapper canvas)
{
    if (!cvarManager->getCvar("mmr_enabled").getBoolValue()) return;

    Vector2 screenRes = canvas.GetSize();
    float scale = cvarManager->getCvar("mmr_scale").getFloatValue();
    int x = cvarManager->getCvar("mmr_x_pos").getIntValue();
    int y = cvarManager->getCvar("mmr_y_pos").getIntValue();
    int opacity = cvarManager->getCvar("mmr_opacity").getIntValue();

    // Define box dimensions once
    int boxW = (int)(200 * scale);
    int boxH = (int)(150 * scale);

    // Auto-correction: keeps the box inside the screen
    if (x + boxW > screenRes.X) x = screenRes.X - boxW;
    if (y + boxH > screenRes.Y) y = screenRes.Y - boxH;
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    // Draw Background
    canvas.SetPosition(Vector2{x, y});
    canvas.SetColor(0, 0, 0, (char)opacity);
    canvas.FillBox(Vector2{boxW, boxH});

    // Header
    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(10 * scale)});
    canvas.SetColor(255, 255, 255, 255);
    canvas.DrawString("MMR Tracker By Baluuu._.", scale, scale);

    // Streak
    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(40 * scale)});
    if (stats.streak > 0) canvas.SetColor(0, 255, 0, 255);
    else if (stats.streak < 0) canvas.SetColor(255, 0, 0, 255);
    else canvas.SetColor(255, 255, 255, 255);
    canvas.DrawString("Streak: " + std::to_string(stats.streak), scale, scale);

    // Wins
    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(70 * scale)});
    canvas.SetColor(0, 150, 255, 255);
    canvas.DrawString("Wins: " + std::to_string(stats.totalWins), scale, scale);

    // Losses
    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(95 * scale)});
    canvas.SetColor(255, 50, 50, 255);
    canvas.DrawString("Losses: " + std::to_string(stats.totalLosses), scale, scale);

    // Session MMR (Blue)
    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(120 * scale)});
    canvas.SetColor(0, 150, 255, 255);
    char mmrText[64];
    if (sessionMMRChange >= 0) {
        snprintf(mmrText, sizeof(mmrText), "Session MMR: +%.1f", sessionMMRChange);
    } else {
        snprintf(mmrText, sizeof(mmrText), "Session MMR: %.1f", sessionMMRChange);
    }
    canvas.DrawString(mmrText, scale, scale);
}
