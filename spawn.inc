#if defined _systems_spawn_included
    #endinput
#endif
#define _systems_spawn_included

#include <YSI_Coding\y_hooks>

hook OnPlayerSpawn(playerid) {
    if(!PlayerData[playerid][pLogged]) {
        Kick(playerid);
        return 1;
    }

    SetCameraBehindPlayer(playerid);
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    return 1;
}