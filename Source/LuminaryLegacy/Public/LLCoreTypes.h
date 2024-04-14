#pragma once

UENUM()
enum class EViewType : uint8
{
    ThirdPerson,
    SideView
};

UENUM()
enum class ETPCameraType : uint8
{
    None,
    Default,
    TopDown,
    Shoulder
};