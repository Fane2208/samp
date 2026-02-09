#if defined _systems_speedometer_included
    #endinput
#endif
#define _systems_speedometer_included

#include <YSI_Coding\y_hooks>

new PlayerText:SpeedoTD[MAX_PLAYERS];

hook OnPlayerConnect(playerid) {
    // Creăm TextDraw-ul într-un stil minimalist, deasupra hărții (minimap)
    SpeedoTD[playerid] = CreatePlayerTextDraw(playerid, 65.0, 315.0, "_");
    PlayerTextDrawBackgroundColor(playerid, SpeedoTD[playerid], 255);
    PlayerTextDrawFont(playerid, SpeedoTD[playerid], 1);
    PlayerTextDrawLetterSize(playerid, SpeedoTD[playerid], 0.23, 1.1);
    PlayerTextDrawColor(playerid, SpeedoTD[playerid], -1);
    PlayerTextDrawSetOutline(playerid, SpeedoTD[playerid], 1);
    PlayerTextDrawSetProportional(playerid, SpeedoTD[playerid], 1);
    PlayerTextDrawSetShadow(playerid, SpeedoTD[playerid], 1);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    PlayerTextDrawDestroy(playerid, SpeedoTD[playerid]);
    return 1;
}

// Task de actualizare (300ms - echilibru bun între fluiditate și performanță)
task UpdateSpeedometer[300]() {
    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;

        if(IsPlayerInAnyVehicle(i)) {
            new vehid = GetPlayerVehicleID(i);
            
            // 1. Calculăm viteza (KM/H)
            new Float:vx, Float:vy, Float:vz;
            GetVehicleVelocity(vehid, vx, vy, vz);
            new speed = floatround(VectorSize(vx, vy, vz) * 181.0); // 181.0 este constanta standard pentru KM/H

            // 2. Verificăm Status (Lock)
            new engine, lights, alarm, doors, bonnet, boot, objective;
            GetVehicleParamsEx(vehid, engine, lights, alarm, doors, bonnet, boot, objective);
            new lock_status[16];
            if(doors == 1) format(lock_status, sizeof(lock_status), "~r~LOCKED");
            else format(lock_status, sizeof(lock_status), "~g~UNLOCKED");

            // 3. Obținem KM (Odometer) - doar dacă este mașină personală
            new Float:vKM = 0.0;
            for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
                if(PlayerVehicle[i][v][pvSpawned] && PlayerVehicle[i][v][pvGameID] == vehid) {
                    vKM = PlayerVehicle[i][v][pvKM];
                    break;
                }
            }

            // 4. Formatăm string-ul final
            // Stil: Alb (~w~) și Mov (~p~) conform temei tale
            new str[256];
            format(str, sizeof(str), 
                "~w~VITEZA: ~p~%d KM/H~n~"\
                "~w~STATUS: %s~n~"\
                "~w~FUEL: ~p~%.0f%%~n~"\
                "~w~ODO: ~p~%.1f KM", 
                speed, lock_status, VehicleFuel[vehid], vKM);

            PlayerTextDrawSetString(i, SpeedoTD[i], str);
            PlayerTextDrawShow(i, SpeedoTD[i]);
        }
        else {
            // Dacă nu e în vehicul, ascundem TextDraw-ul
            PlayerTextDrawHide(i, SpeedoTD[i]);
        }
    }
}