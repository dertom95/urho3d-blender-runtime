#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_NAKAMA_LEADERBOARD_RESULT, NakamaLeaderboardResult)
{
    URHO3D_PARAM(P_LEADERBOARDID, LeaderboardId); // CustomType
    URHO3D_PARAM(P_RECORDS, Records); // CustomType
    URHO3D_PARAM(P_OWNERRECORDS, OwnerRecords); // CustomType
    URHO3D_PARAM(P_NEXTCURSOR, NextCursor); // NextCursor
    URHO3D_PARAM(P_PREVCURSOR, PrevCursor); // NextCursor
}

