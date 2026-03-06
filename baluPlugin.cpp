#include "baluPlugin.h"
#include <cstdio>
#include <fstream>
#include <sstream>

BAKKESMOD_PLUGIN(baluPlugin, "MMR tracker By Baluuu._.", "1.0", 0)

void baluPlugin::onLoad()
{
    cvarManager->registerCvar("mmr_enabled", "1", "", true, true, 0, true, 1);
    cvarManager->registerCvar("mmr_save_progress", "0", "", true, true, 0, true, 1);
    cvarManager->registerCvar("mmr_x_pos", "100", "", true, true, 0, true, 4000);
    cvarManager->registerCvar("mmr_y_pos", "100", "", true, true, 0, true, 4000);
    cvarManager->registerCvar("mmr_scale", "1.0", "", true, true, 0.5f, true, 5.0f);
    cvarManager->registerCvar("mmr_opacity", "150", "", true, true, 0, true, 255);
    cvarManager->registerCvar("mmr_display_mode", "11", "", true, true, 0, true, 100);

    cvarManager->registerNotifier("mmr_reset_pos", [this](std::vector<std::string> args) {
        cvarManager->getCvar("mmr_x_pos").setValue(100);
        cvarManager->getCvar("mmr_y_pos").setValue(100);
    }, "", PERMISSION_ALL);

    cvarManager->registerNotifier("mmr_reset_stats", [this](std::vector<std::string> args) {
        playlistStats.clear();
        SaveData();
    }, "", PERMISSION_ALL);

    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&baluPlugin::OnMatchEnd, this, std::placeholders::_1));
    gameWrapper->RegisterDrawable(std::bind(&baluPlugin::Render, this, std::placeholders::_1));

    LoadData();
    PollMMR();
}


void baluPlugin::onUnload()
{
    SaveData();
}

std::string baluPlugin::GetPlaylistName(int id) {
    switch (id) {
        case 1: return "Casual 1v1";
        case 2: return "Casual 2v2";
        case 3: return "Casual 3v3";
        case 10: return "Ranked 1v1";
        case 11: return "Ranked 2v2";
        case 13: return "Ranked 3v3";
        case 27: return "Hoops";
        case 28: return "Rumble";
        case 29: return "Dropshot";
        case 30: return "Snow Day";
        case 34: return "Tournaments";
        default: return "Unknown (" + std::to_string(id) + ")";
    }
}

void baluPlugin::SaveData()
{
    std::filesystem::path saveFile = gameWrapper->GetDataFolder() / "mmr_tracker_save.txt";
    
    if (!cvarManager->getCvar("mmr_save_progress").getBoolValue()) {
        if (std::filesystem::exists(saveFile)) std::filesystem::remove(saveFile);
        playlistStats.clear();
        return;
    }
    
    std::ofstream out(saveFile);
    if (out.is_open()) {
        for (const auto& pair : playlistStats) {
            out << pair.first << " " << pair.second.totalWins << " " 
                << pair.second.totalLosses << " " << pair.second.streak << " " 
                << pair.second.sessionMMRChange << "\n";
        }
        out.close();
    }
}

void baluPlugin::LoadData()
{
    std::filesystem::path saveFile = gameWrapper->GetDataFolder() / "mmr_tracker_save.txt";
    std::ifstream in(saveFile);
    if (in.is_open()) {
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            int id;
            MMRData d;
            if (iss >> id >> d.totalWins >> d.totalLosses >> d.streak >> d.sessionMMRChange) {
                playlistStats[id] = d;
            }
        }
        in.close();
    }
}

void baluPlugin::PollMMR()
{
    UpdateMMR();
    gameWrapper->SetTimeout([this](GameWrapper* gw) { PollMMR(); }, 2.0f);
}

void baluPlugin::OnMatchEnd(std::string eventName)
{
    ServerWrapper server = gameWrapper->GetCurrentGameState();
    if (!server) return;

    TeamWrapper winner = server.GetMatchWinner();
    if (!winner) return;

    auto pri = gameWrapper->GetPlayerController().GetPRI();
    if (!pri) return;

    int team = pri.GetTeamNum();
    int winnerTeam = winner.GetTeamNum();

    MMRWrapper mmrWrapper = gameWrapper->GetMMRWrapper();
    currentPlaylist = mmrWrapper.GetCurrentPlaylist();

    if (team == winnerTeam) {
        playlistStats[currentPlaylist].totalWins++;
        playlistStats[currentPlaylist].streak = (playlistStats[currentPlaylist].streak < 0) ? 1 : playlistStats[currentPlaylist].streak + 1;
    } else {
        playlistStats[currentPlaylist].totalLosses++;
        playlistStats[currentPlaylist].streak = (playlistStats[currentPlaylist].streak > 0) ? -1 : playlistStats[currentPlaylist].streak - 1;
    }
    
    SaveData();
}

void baluPlugin::UpdateMMR()
{
    MMRWrapper mmrWrapper = gameWrapper->GetMMRWrapper();
    currentPlaylist = mmrWrapper.GetCurrentPlaylist();
    auto uid = gameWrapper->GetUniqueID();

    float currentMMR = mmrWrapper.GetPlayerMMR(uid, currentPlaylist);
    if (currentMMR <= 0) return;

    MMRData& data = playlistStats[currentPlaylist];

    if (data.lastKnownMMR < 0) {
        data.lastKnownMMR = currentMMR;
    } else {
        float change = currentMMR - data.lastKnownMMR;
        if (change != 0) {
            data.sessionMMRChange += change;
            data.lastKnownMMR = currentMMR;
            SaveData();
        }
    }
}

void baluPlugin::Render(CanvasWrapper canvas)
{
    if (!cvarManager->getCvar("mmr_enabled").getBoolValue()) return;

    int displayPlaylist = cvarManager->getCvar("mmr_display_mode").getIntValue();
    MMRData data = playlistStats[displayPlaylist];
    std::string modeName = GetPlaylistName(displayPlaylist);

    Vector2 screenRes = canvas.GetSize();
    float scale = cvarManager->getCvar("mmr_scale").getFloatValue();
    int x = cvarManager->getCvar("mmr_x_pos").getIntValue();
    int y = cvarManager->getCvar("mmr_y_pos").getIntValue();
    int opacity = cvarManager->getCvar("mmr_opacity").getIntValue();

    int boxW = (int)(250 * scale);
    int boxH = (int)(200 * scale);

    if (x + boxW > screenRes.X) x = screenRes.X - boxW;
    if (y + boxH > screenRes.Y) y = screenRes.Y - boxH;
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    canvas.SetPosition(Vector2{x, y});
    canvas.SetColor(0, 0, 0, (char)opacity);
    canvas.FillBox(Vector2{boxW, boxH});

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(10 * scale)});
    canvas.SetColor(255, 255, 255, 255);
    canvas.DrawString("MMR Tracker By Baluuu._.", scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(35 * scale)});
    canvas.SetColor(255, 215, 0, 255); 
    canvas.DrawString("Mode: " + modeName, scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(65 * scale)});
    if (data.streak > 0) canvas.SetColor(0, 255, 0, 255);
    else if (data.streak < 0) canvas.SetColor(255, 0, 0, 255);
    else canvas.SetColor(255, 255, 255, 255);
    canvas.DrawString("Streak: " + std::to_string(data.streak), scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(95 * scale)});
    canvas.SetColor(0, 150, 255, 255);
    canvas.DrawString("Wins: " + std::to_string(data.totalWins), scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(120 * scale)});
    canvas.SetColor(255, 50, 50, 255);
    canvas.DrawString("Losses: " + std::to_string(data.totalLosses), scale, scale);

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(145 * scale)});
    canvas.SetColor(0, 150, 255, 255);
    char mmrText[64];
    if (data.sessionMMRChange >= 0) {
        snprintf(mmrText, sizeof(mmrText), "Session MMR: +%.1f", data.sessionMMRChange);
    } else {
        snprintf(mmrText, sizeof(mmrText), "Session MMR: %.1f", data.sessionMMRChange);
    }
    canvas.DrawString(mmrText, scale, scale);

    float mmrNeeded = -1.0f;
    if (data.lastKnownMMR >= 0) {
        mmrNeeded = GetMMRNeededForNextRank(displayPlaylist, data.lastKnownMMR);
    }

    canvas.SetPosition(Vector2{x + (int)(10 * scale), y + (int)(170 * scale)});
    
    if (mmrNeeded > 0) {
        canvas.SetColor(255, 215, 0, 255);
        char nextRankText[64];
        snprintf(nextRankText, sizeof(nextRankText), "untill next rank: %.1f mmr", mmrNeeded);
        canvas.DrawString(nextRankText, scale, scale);
    } else {
        canvas.SetColor(150, 150, 150, 255);
        canvas.DrawString("untill next rank: N/A", scale, scale);
    }


}


float baluPlugin::GetMMRNeededForNextRank(int playlist, float currentMMR) {
    if (playlist != 11) return -1.0f;

    std::vector<float> thresholds = {
        119, 138, 157, 168, 179, 198, 217, 229, 239, 258, 277, 
        293, 299, 318, 337, 352, 359, 378, 397, 411, 419, 438, 457, 
        472, 479, 498, 517, 532, 539, 558, 577, 586, 599, 618, 637, 
        641, 659, 678, 697, 706, 719, 738, 757, 764, 779, 798, 817, 
        827, 844, 868, 892, 914, 924, 948, 972, 993, 1004, 1028, 1052, 
        1075, 1094, 1128, 1162, 1195, 1214, 1248, 1282, 1314, 1334, 1368, 1402, 
        1435, 1462, 1498, 1537, 1575, 1601, 1642, 1677, 1715, 1745, 1788, 1832, 1861
    };

    for (float t : thresholds) {
        if (currentMMR < t) {
            return t - currentMMR;
        }
    }
    return -1.0f;
}



