#if defined _systems_fuel_included
    #endinput
#endif
#define _systems_fuel_included

#include <YSI_Coding\y_hooks>

#define MAX_FUEL            100.0
#define FUEL_CONSUMPTION    0.05  // Cât consumă la fiecare 5 secunde (ajustează după preferință)

hook OnGameModeInit() {
    // Initializăm toate vehiculele cu benzină plină la pornire
    for(new i = 0; i < MAX_VEHICLES; i++) VehicleFuel[i] = MAX_FUEL;
    return 1;
}

// Când o mașină personală este spawnată, îi setăm benzina din variabila salvată
hook OnVehicleSpawn(vehicleid) {
    // Căutăm dacă mașina este personală pentru a-i aplica benzina salvată
    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;
        for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
            if(PlayerVehicle[i][v][pvSpawned] && PlayerVehicle[i][v][pvGameID] == vehicleid) {
                VehicleFuel[vehicleid] = PlayerVehicle[i][v][pvFuel];
                return 1;
            }
        }
    }
    VehicleFuel[vehicleid] = MAX_FUEL; // Dacă nu e personală, o punem plină
    return 1;
}

// --- TASK CONSUM BENZINĂ (Optimizat: 5 secunde) ---
task FuelUpdate[5000]() {
    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;
        if(!IsPlayerInAnyVehicle(i) || GetPlayerState(i) != PLAYER_STATE_DRIVER) continue;

        new vehid = GetPlayerVehicleID(i);
        new engine, lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vehid, engine, lights, alarm, doors, bonnet, boot, objective);

        // Consumăm doar dacă motorul este pornit
        if(engine == 1) {
            if(VehicleFuel[vehid] > 0.0) {
                VehicleFuel[vehid] -= FUEL_CONSUMPTION;
                
                // Dacă a rămas fără benzină
                if(VehicleFuel[vehid] <= 0.0) {
                    VehicleFuel[vehid] = 0.0;
                    SetVehicleParamsEx(vehid, 0, lights, alarm, doors, bonnet, boot, objective); // Oprim motorul
                    GameTextForPlayer(i, "~r~Fara Benzina!", 3000, 3);
                    SendClientMessage(i, COLOR_RED, "Masina a ramas fara benzina si motorul s-a oprit!");
                }
            }
        }
    }
}

// --- COMANDĂ ALIMENTARE (/fill) ---
YCMD:fill(playerid, params[], help) {
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) 
        return SendClientMessage(playerid, COLOR_RED, "Trebuie sa fii la volan!");

    // Verificăm dacă este lângă o benzinărie (Exemplu: Grotti Gas Station)
    if(!IsPlayerInRangeOfPoint(playerid, 15.0, 1004.0, -939.0, 42.0)) 
        return SendClientMessage(playerid, COLOR_RED, "Nu esti la o benzinarie!");

    new vehid = GetPlayerVehicleID(playerid);
    if(VehicleFuel[vehid] >= 95.0) return SendClientMessage(playerid, COLOR_RED, "Rezervorul este deja plin!");

    new cost = (100 - floatround(VehicleFuel[vehid])) * 10; // 10$ per litru
    if(PlayerData[playerid][pMoney] < cost) return va_SendClientMessage(playerid, COLOR_RED, "Nu ai suficienti bani (Cost: $%d)!", cost);

    GivePlayerCash(playerid, -cost);
    VehicleFuel[vehid] = MAX_FUEL;
    
    GameTextForPlayer(playerid, "~g~Alimentat!", 3000, 3);
    va_SendClientMessage(playerid, COLOR_LIGHTPURPLE, "* Ai alimentat vehiculul pentru $%d.", cost);
    return 1;
}