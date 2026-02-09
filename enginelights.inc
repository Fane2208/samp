#if defined _systems_vehicles_ctrl_included
    #endinput
#endif
#define _systems_vehicles_ctrl_included

#include <YSI_Coding\y_hooks>

// Definim tastele
#define KEY_ENGINE      KEY_SUBMISSION // Tasta 2
#define KEY_LIGHTS      KEY_ACTION     // Tasta CTRL

// --- FUNCTII DE CONTROL (STOCKS) ---

stock SwitchEngine(playerid, vehid) {
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehid, engine, lights, alarm, doors, bonnet, boot, objective);
    
    if(engine <= 0) { // Pornire
        SetVehicleParamsEx(vehid, 1, lights, alarm, doors, bonnet, boot, objective);
        GameTextForPlayer(playerid, "~w~Motor ~g~PORNIT", 2000, 3);
        
        new str[128];
        format(str, sizeof(str), "* %s porneste motorul vehiculului.", PlayerData[playerid][pName]);
        ProxDetector(15.0, playerid, str, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE);
    }
    else { // Oprire
        SetVehicleParamsEx(vehid, 0, lights, alarm, doors, bonnet, boot, objective);
        GameTextForPlayer(playerid, "~w~Motor ~r~OPRIT", 2000, 3);
        
        new str[128];
        format(str, sizeof(str), "* %s opreste motorul vehiculului.", PlayerData[playerid][pName]);
        ProxDetector(15.0, playerid, str, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE);
    }
    return 1;
}

stock SwitchLights(playerid, vehid) {
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehid, engine, lights, alarm, doors, bonnet, boot, objective);
    
    if(lights <= 0) { // Aprinde
        SetVehicleParamsEx(vehid, engine, 1, alarm, doors, bonnet, boot, objective);
        
        new str[128];
        format(str, sizeof(str), "* %s aprinde farurile vehiculului.", PlayerData[playerid][pName]);
        ProxDetector(15.0, playerid, str, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE);
    }
    else { // Stinge
        SetVehicleParamsEx(vehid, engine, 0, alarm, doors, bonnet, boot, objective);
        
        new str[128];
        format(str, sizeof(str), "* %s stinge farurile vehiculului.", PlayerData[playerid][pName]);
        ProxDetector(15.0, playerid, str, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE);
    }
    return 1;
}

// --- HOOK TASTE ---

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 1;
    new vehid = GetPlayerVehicleID(playerid);

    if((newkeys & KEY_ENGINE) && !(oldkeys & KEY_ENGINE)) {
        SwitchEngine(playerid, vehid);
    }

    if((newkeys & KEY_LIGHTS) && !(oldkeys & KEY_LIGHTS)) {
        SwitchLights(playerid, vehid);
    }
    return 1;
}

// --- COMENZI ---

YCMD:engine(playerid, params[], help) {
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 1;
    SwitchEngine(playerid, GetPlayerVehicleID(playerid));
    return 1;
}

YCMD:lights(playerid, params[], help) {
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 1;
    SwitchLights(playerid, GetPlayerVehicleID(playerid));
    return 1;
}

// Comanda /lock pentru a incuia vehiculul
YCMD:lock(playerid, params[], help) {
    new vehid = -1;

    // Verificăm dacă jucătorul este în mașină sau lângă ea
    if(IsPlayerInAnyVehicle(playerid)) vehid = GetPlayerVehicleID(playerid);
    else {
        // Căutăm cea mai apropiată mașină pe o rază de 5 metri
        for(new i = 1; i < MAX_VEHICLES; i++) {
            new Float:x, Float:y, Float:z;
            GetVehiclePos(i, x, y, z);
            if(IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z)) {
                vehid = i;
                break;
            }
        }
    }

    if(vehid == -1) return SendClientMessage(playerid, COLOR_RED, "Nu esti langa niciun vehicul!");

    // Verificăm dacă vehiculul este al lui (din sloturile personale)
    new bool:isOwner = false;
    for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
        if(PlayerVehicle[playerid][v][pvSpawned] && PlayerVehicle[playerid][v][pvGameID] == vehid) {
            isOwner = true;
            break;
        }
    }

    if(!isOwner && !IsPlayerAdmin(playerid)) return SendClientMessage(playerid, COLOR_RED, "Nu ai cheile acestui vehicul!");

    // Toggle Lock
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehid, engine, lights, alarm, doors, bonnet, boot, objective);
    
    if(doors <= 0) { // Daca e descuiata, o incuiem
        SetVehicleParamsEx(vehid, engine, lights, alarm, 1, bonnet, boot, objective);
        GameTextForPlayer(playerid, "~w~Vehicul ~r~INCUIAT", 2000, 3);
    } else { // Daca e incuiata, o descuim
        SetVehicleParamsEx(vehid, engine, lights, alarm, 0, bonnet, boot, objective);
        GameTextForPlayer(playerid, "~w~Vehicul ~g~DESCUIAT", 2000, 3);
    }
    
    PlayerPlaySound(playerid, 1145, 0.0, 0.0, 0.0); // Sunet de "clic"
    return 1;
}