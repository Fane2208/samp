#if defined _systems_admin_included
    #endinput
#endif
#define _systems_admin_included

#include <YSI_Coding\y_hooks>

// Definire culori admin
#define COLOR_ADMIN_DUTY 0xE84C3DFF // Rosu pentru duty

// Functie pentru logare actiuni admini
stock LogAdminAction(adminid, targetid, const action[]) {
    new query[400];
    mysql_format(db_handle, query, sizeof(query), 
        "INSERT INTO `admin_logs` (admin_id, admin_name, target_id, target_name, action) VALUES (%d, '%e', %d, '%e', '%e')",
        PlayerData[adminid][pID], PlayerData[adminid][pName],
        PlayerData[targetid][pID], PlayerData[targetid][pName], action);
    
    mysql_tquery(db_handle, query);
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerData[playerid][pAdminDuty] = false;
    PlayerData[playerid][pAdminLabel] = Text3D:INVALID_3DTEXT_ID;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if(Iter_Contains(Admins, playerid)) {
        Iter_Remove(Admins, playerid);
    }
    if(PlayerData[playerid][pAdminLabel] != Text3D:INVALID_3DTEXT_ID) {
        Delete3DTextLabel(PlayerData[playerid][pAdminLabel]);
        PlayerData[playerid][pAdminLabel] = Text3D:INVALID_3DTEXT_ID;
    }
    return 1;
}

// Comanda /makeadmin
YCMD:makeadmin(playerid, params[], help) {
    // Doar RCON sau Admin 7 pot folosi comanda
    if(!IsPlayerAdmin(playerid) && PlayerData[playerid][pAdmin] < 7) return 1;

    new target, level;
    if(sscanf(params, "ui", target, level)) return SendClientMessage(playerid, COLOR_GREY, "Sintaxa: /makeadmin <playerid/name> <level (0-7)>");
    if(!IsPlayerConnected(target)) return SendClientMessage(playerid, COLOR_RED, "Jucatorul nu este conectat.");
    if(level < 0 || level > 7) return SendClientMessage(playerid, COLOR_RED, "Level invalid (0-7).");

    PlayerData[target][pAdmin] = level;

    // Gestionare Iterator
    if(level > 0) Iter_Add(Admins, target);
    else {
        Iter_Remove(Admins, target);
        PlayerData[target][pAdminDuty] = false;
        if(PlayerData[target][pAdminLabel] != Text3D:INVALID_3DTEXT_ID) {
            Delete3DTextLabel(PlayerData[target][pAdminLabel]);
            PlayerData[target][pAdminLabel] = Text3D:INVALID_3DTEXT_ID;
        }
    }

    new str[128];
    format(str, sizeof(str), "[ADMIN] {FFFFFF}%s l-a numit pe %s admin level %d.", PlayerData[playerid][pName], PlayerData[target][pName], level);
    SendClientMessageToAll(COLOR_ADMIN_DUTY, str);

    // Salvare in DB si Log
    SavePlayer(target);
    format(str, sizeof(str), "Set admin level to %d", level);
    LogAdminAction(playerid, target, str);
    return 1;
}

// Comanda /aduty
YCMD:aduty(playerid, params[], help) {
    if(PlayerData[playerid][pAdmin] < 1) return 1;

    PlayerData[playerid][pAdminDuty] = !PlayerData[playerid][pAdminDuty];

    new str[128];
    if(PlayerData[playerid][pAdminDuty]) {
        SetPlayerColor(playerid, COLOR_ADMIN_DUTY);
        SetPlayerHealth(playerid, 999999.0);
        SetPlayerArmour(playerid, 999999.0);
        
        PlayerData[playerid][pAdminLabel] = Create3DTextLabel("[ADMIN]", COLOR_ADMIN_DUTY, 0.0, 0.0, 1.2, 20.0, GetPlayerVirtualWorld(playerid), true);
        Attach3DTextLabelToPlayer(PlayerData[playerid][pAdminLabel], playerid, 0.0, 0.0, 0.4);
        
        format(str, sizeof(str), "[ADMIN] {FFFFFF}%s este acum la datorie (On-Duty).", PlayerData[playerid][pName]);
        SendClientMessageToAll(COLOR_ADMIN_DUTY, str);
        
        va_SendClientMessage(playerid, COLOR_ADMIN_DUTY, "* Ti-ai activat statusul de admin %d. Esti acum invincibil (God Mode).", PlayerData[playerid][pAdmin]);
    }
    else {
        SetPlayerColor(playerid, COLOR_WHITE);
        SetPlayerHealth(playerid, 100.0); 
        
        if(PlayerData[playerid][pAdminLabel] != Text3D:INVALID_3DTEXT_ID) {
            Delete3DTextLabel(PlayerData[playerid][pAdminLabel]);
            PlayerData[playerid][pAdminLabel] = Text3D:INVALID_3DTEXT_ID;
        }

        format(str, sizeof(str), "[ADMIN] {FFFFFF}%s nu mai este la datorie (Off-Duty).", PlayerData[playerid][pName]);
        SendClientMessageToAll(COLOR_ADMIN_DUTY, str);

        SendClientMessage(playerid, COLOR_WHITE, "* Nu mai esti la datorie. God Mode dezactivat.");
    }
    return 1;
}

YCMD:gotoxyz(playerid, params[], help) {
    if(PlayerData[playerid][pAdmin] < 1) return 1;

    new Float:x, Float:y, Float:z;
    if(sscanf(params, "fff", x, y, z)) return SendClientMessage(playerid, COLOR_GREY, "Sintaxa: /gotoxyz <x> <y> <z>");

    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);

    if(IsPlayerInAnyVehicle(playerid)) {
        SetVehiclePos(GetPlayerVehicleID(playerid), x, y, z);
    } else {
        SetPlayerPos(playerid, x, y, z);
    }

    va_SendClientMessage(playerid, COLOR_LIGHTPURPLE, "[ADMIN] {FFFFFF}Te-ai teleportat la coordonatele: %.2f, %.2f, %.2f", x, y, z);
    return 1;
}

YCMD:set(playerid, params[], help) {
    if(PlayerData[playerid][pAdmin] < 7) return 1;

    new target, attribute[24], value;
    // Folosim "u" pentru player (id/nume), "s[24]" pentru atribut si "i" pentru valoare
    if(sscanf(params, "us[24]i", target, attribute, value)) {
        SendClientMessage(playerid, COLOR_GREY, "Sintaxa: /set <playerid/name> <attribute> <value>");
        SendClientMessage(playerid, COLOR_GREY, "Atribute: level, money, admin, respect, hours, hp, armour");
        return 1;
    }

    if(!IsPlayerConnected(target)) return SendClientMessage(playerid, COLOR_RED, "Jucatorul nu este conectat.");

    new str[128];
    str[0] = '\0'; // Initializam string-ul gol

    // --- LOGICA DE COMPARARE EXPLICITA ---

    if(strcmp(attribute, "level", true) == 0) {
        PlayerData[target][pLevel] = value;
        SetPlayerScore(target, value);
        format(str, sizeof(str), "level to %d", value);
    }
    else if(strcmp(attribute, "money", true) == 0 || strcmp(attribute, "cash", true) == 0) {
        SetPlayerCash(target, value);
        format(str, sizeof(str), "money to $%d", value);
    }
    else if(strcmp(attribute, "admin", true) == 0) {
        PlayerData[target][pAdmin] = value;
        // Update iterator admini
        if(value > 0) {
            if(!Iter_Contains(Admins, target)) Iter_Add(Admins, target);
        }
        else {
            if(Iter_Contains(Admins, target)) Iter_Remove(Admins, target);
        }
        format(str, sizeof(str), "admin level to %d", value);
    }
    else if(strcmp(attribute, "respect", true) == 0 || strcmp(attribute, "rp", true) == 0) {
        PlayerData[target][pRespect] = value;
        format(str, sizeof(str), "respect points to %d", value);
    }
    else if(strcmp(attribute, "hours", true) == 0) {
        PlayerData[target][pHours] = value;
        format(str, sizeof(str), "played hours to %d", value);
    }
    else if(strcmp(attribute, "hp", true) == 0) {
        SetPlayerHealth(target, float(value));
        format(str, sizeof(str), "HP to %d", value);
    }
    else if(strcmp(attribute, "armour", true) == 0) {
        SetPlayerArmour(target, float(value));
        format(str, sizeof(str), "armour to %d", value);
    }
    else {
        // Daca nu a intrat pe nicio ramura de mai sus
        return SendClientMessage(playerid, COLOR_RED, "Atribut invalid! Foloseste: level, money, admin, respect, hours, hp, armour");
    }

    // Mesaje confirmare si Salvare
    if(str[0] != '\0') {
        va_SendClientMessage(playerid, COLOR_LIGHTPURPLE, "[SET] {FFFFFF}I-ai setat lui %s %s.", PlayerData[target][pName], str);
        va_SendClientMessage(target, COLOR_LIGHTPURPLE, "[SET] {FFFFFF}Adminul %s ti-a setat %s.", PlayerData[playerid][pName], str);
        
        SavePlayer(target);
        LogAdminAction(playerid, target, va_return("Set %s", str));
    }
    
    return 1;
}