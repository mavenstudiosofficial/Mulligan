#pragma once

namespace EventKeys
{
    inline const FName DISTURBANCE_CHANGED(TEXT("disturbance_changed"));
    inline const FName INSTABILITY_CHANGED(TEXT("instability_changed"));
    inline const FName REACH_TURN(TEXT("reach_turn"));
    inline const FName OPEN_UI_PANEL(TEXT("open_ui_panel"));
    inline const FName CLOSE_UI_PANEL(TEXT("close_ui_panel"));
}

namespace BlackboardKeys
{
    inline const FName DISTURBANCE(TEXT("disturbance"));
    inline const FName INSTABILITY(TEXT("instability"));
    inline const FName VALUE_CHANGED(TEXT("value_changed"));
    inline const FName ID(TEXT("id"));
    inline const FName NAME(TEXT("name"));
    inline const FName TURN(TEXT("turn"));
    inline const FName TIME(TEXT("time"));
}