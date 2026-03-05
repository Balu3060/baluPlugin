#include "OnlineStatusChanger.h"
#include "imgui/imgui.h"

void StatusOverrider::SetImGuiContext(uintptr_t ctx)
{
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

std::string StatusOverrider::GetPluginName()
{
    return "Status Overrider";
}

void StatusOverrider::RenderSettings()
{
    CVarWrapper enableCvar = cvarManager->getCvar("cl_status_enable");
    if (!enableCvar) return;

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable Plugin", &enabled))
    {
        enableCvar.setValue(enabled);
    }

    CVarWrapper statusCvar = cvarManager->getCvar("cl_status_override");
    if (!statusCvar) return;

    int statusValue = statusCvar.getIntValue();
    if (ImGui::SliderInt("Status (0-3)", &statusValue, 0, 3))
    {
        statusCvar.setValue(statusValue);
    }

    CVarWrapper stringCvar = cvarManager->getCvar("cl_status_string");
    if (!stringCvar) return;

    std::string textValue = stringCvar.getStringValue();
    char buffer[256];
    strncpy_s(buffer, sizeof(buffer), textValue.c_str(), _TRUNCATE);

    if (ImGui::InputText("Custom Text", buffer, sizeof(buffer)))
    {
        stringCvar.setValue(std::string(buffer));
    }
}
