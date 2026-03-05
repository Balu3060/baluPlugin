#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/includes.h"
#include <filesystem>
#include <map>
#include <string>

class baluPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    virtual void onLoad() override;
    virtual void onUnload() override;

    void OnMatchEnd(std::string eventName);
    void UpdateMMR();
    void PollMMR();
    void Render(CanvasWrapper canvas);
    void SaveData();
    void LoadData();

private:
    struct MMRData {
        int totalWins = 0;
        int totalLosses = 0;
        int streak = 0;
        float sessionMMRChange = 0.0f;
        float lastKnownMMR = -1.0f;
    };

    std::map<int, MMRData> playlistStats;
    int currentPlaylist = 0;

    std::string GetPlaylistName(int id);
};
