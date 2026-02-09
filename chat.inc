#if defined _systems_chat_included
    #endinput
#endif
#define _systems_chat_included

#include <YSI_Coding\y_hooks>

// --- FUNCȚII UTILS (Definite la început pentru a evita warning 208) ---

stock Float:GetPlayerDistanceToPoint(playerid, Float:x, Float:y, Float:z) {
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    return VectorSize(px-x, py-y, pz-z);
}

stock Float:GetPlayerDistanceToPlayer(playerid, targetid) {
    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    return GetPlayerDistanceToPoint(playerid, x, y, z);
}

stock ProxDetector(Float:radi, playerid, const string[], col1, col2, col3, col4, col5) {
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;
        if(IsPlayerInRangeOfPoint(i, radi, x, y, z)) {
            new Float:dist = GetPlayerDistanceToPlayer(playerid, i);
            if(dist < radi/4) SendClientMessage(i, col1, string);
            else if(dist < radi/3) SendClientMessage(i, col2, string);
            else if(dist < radi/2) SendClientMessage(i, col3, string);
            else if(dist < radi) SendClientMessage(i, col4, string);
            else SendClientMessage(i, col5, string);
        }
    }
    return 1;
}

// --- CHAT LOCAL (PROXIMITY) ---

hook OnPlayerText(playerid, text[]) {
    if(!PlayerData[playerid][pLogged]) return 0;

    new str[256], Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    format(str, sizeof(str), "%s spune: %s", PlayerData[playerid][pName], text);

    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;
        if(IsPlayerInRangeOfPoint(i, 20.0, x, y, z)) {
            new Float:dist = GetPlayerDistanceToPlayer(playerid, i);
            if(dist < 10.0) SendClientMessage(i, COLOR_WHITE, str);
            else SendClientMessage(i, COLOR_GREY, str);
        }
    }
    return 0; 
}

// --- COMENZI ---

YCMD:me(playerid, params[], help) {
    if(isnull(params)) return SendClientMessage(playerid, COLOR_GREY, "Sintaxa: /me <actiune>");
    
    new str[256];
    format(str, sizeof(str), "* %s %s", PlayerData[playerid][pName], params);
    ProxDetector(20.0, playerid, str, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE, COLOR_LIGHTPURPLE);
    return 1;
}

YCMD:a(playerid, params[], help) {
    if(PlayerData[playerid][pAdmin] < 1) return 1;
    if(isnull(params)) return SendClientMessage(playerid, COLOR_GREY, "Sintaxa: /a <mesaj>");

    new str[256];
    format(str, sizeof(str), "{FF0000}[Admin Chat] %s: " C_WHITE "%s", PlayerData[playerid][pName], params);

    foreach(new i : Admins) {
        SendClientMessage(i, -1, str);
    }
    return 1;
}