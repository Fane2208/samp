#if defined _systems_player_cmds_included
    #endinput
#endif
#define _systems_player_cmds_included

#include <YSI_Visual\y_commands>

YCMD:stats(playerid, params[], help) {
    if(!PlayerData[playerid][pLogged]) return 1;
    SendClientMessage(playerid, COLOR_LIGHTPURPLE, "______________________________________________________");
    va_SendClientMessage(playerid, -1, "" C_LIGHTPURPLE "Statisticile lui " C_WHITE "%s:", PlayerData[playerid][pName]);
    va_SendClientMessage(playerid, -1, "" C_WHITE "Level: " C_LIGHTPURPLE "%d " C_WHITE "Respect Points: " C_LIGHTPURPLE "%d " C_WHITE "| Bani: " C_LIGHTPURPLE "$%d", 
        PlayerData[playerid][pLevel], PlayerData[playerid][pRespect], PlayerData[playerid][pMoney]);
    va_SendClientMessage(playerid, -1, "" C_WHITE "Admin: " C_LIGHTPURPLE "%d " C_WHITE "| Cont ID: " C_LIGHTPURPLE "#%d", 
        PlayerData[playerid][pAdmin], PlayerData[playerid][pID]);
    SendClientMessage(playerid, COLOR_LIGHTPURPLE, "______________________________________________________");
    return 1;
}
YCMD:admins(playerid, params[], help) {
    if(!PlayerData[playerid][pLogged]) return 1;
    new countOnline = 0, countDuty = 0;
    SendClientMessage(playerid, COLOR_LIGHTPURPLE, "________________ Admini Online ________________");
    foreach(new i : Admins) {
        countOnline++;
        
        new status[32];
        if(PlayerData[i][pAdminDuty]) {
            countDuty++;
            format(status, sizeof(status), "" C_RED "On-Duty");
        }
        else {
            format(status, sizeof(status), "" C_GREY "Off-Duty");
        }

        va_SendClientMessage(playerid, -1, "Admin Level %d: " C_LIGHTPURPLE "%s " C_WHITE "- %s", 
            PlayerData[i][pAdmin], 
            PlayerData[i][pName], 
            status
        );
    }
    if(countOnline == 0) {
        SendClientMessage(playerid, COLOR_GREY, "Momentan nu sunt admini online.");
    }
    SendClientMessage(playerid, COLOR_LIGHTPURPLE, "_______________________________________________");
    va_SendClientMessage(playerid, -1, "Total Admini: " C_LIGHTPURPLE "%d " C_WHITE "| La datorie (On-Duty): " C_RED "%d", 
        countOnline, countDuty);

    return 1;
}
