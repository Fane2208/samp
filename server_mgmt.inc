#if defined _systems_server_mgmt_included
    #endinput
#endif
#define _systems_server_mgmt_included

#include <YSI_Coding\y_hooks>

// 1. Corectie Tag Mismatch: Adaugam tag-ul Timer: variabilei
static Timer:restart_timer; 
static restart_countdown = 0;

// Timer care se apeleaza in fiecare secunda a numaratorii inverse
timer RestartCountdown[1000](seconds) {
    #pragma unused seconds 
    // Am pus pragma unused pentru a elimina warning-ul 203, 
    // deoarece folosim variabila globala restart_countdown pentru logica.

    restart_countdown--;

    if(restart_countdown > 0) {
        if(restart_countdown == 30 || restart_countdown == 15 || restart_countdown <= 5) {
            new str[128];
            format(str, sizeof(str), "~r~RESTART IN ~w~%d ~r~SECUNDE...", restart_countdown);
            GameTextForAll(str, 1000, 6);
            
            foreach(new i : Player) {
                PlayerPlaySound(i, 1056, 0.0, 0.0, 0.0);
            }
            
            va_SendClientMessageToAll(COLOR_RED, "[SERVER] " C_WHITE "Serverul se va restarta in %d secunde pentru mentenanta!", restart_countdown);
        }
        
        // Reprogramam timer-ul
        restart_timer = defer RestartCountdown(restart_countdown);
    }
    else {
        SendClientMessageToAll(COLOR_RED, "[SERVER] " C_WHITE "Serverul se restarteaza ACUM. Salvare date...");
        
        new count = 0;
        foreach(new i : Player) {
            if(PlayerData[i][pLogged]) {
                SavePlayer(i);
                count++;
            }
        }
        printf("[SQL] Restart: %d conturi au fost salvate cu succes.", count);

        defer ExecuteRestart();
    }
}

timer ExecuteRestart[500]() {
    SendRconCommand("gmx");
}

YCMD:restart(playerid, params[], help) {
    if(PlayerData[playerid][pAdmin] < 7) return 1;

    new seconds_param;
    if(sscanf(params, "i", seconds_param)) return SendClientMessage(playerid, COLOR_GREY, "Sintaxa: /restart <secunde (minim 10)>");
    if(seconds_param < 10) return SendClientMessage(playerid, COLOR_RED, "Eroare: Numarul de secunde trebuie sa fie minim 10.");
    if(restart_countdown > 0) return SendClientMessage(playerid, COLOR_RED, "Eroare: Exista deja un restart in desfasurare.");

    restart_countdown = seconds_param;
    
    va_SendClientMessageToAll(COLOR_RED, "[RESTART] " C_WHITE "Adminul %s a programat un restart in %d secunde!", PlayerData[playerid][pName], seconds_param);
    GameTextForAll("~r~RESTART PROGRAMAT", 3000, 3);
    
    // Pornim numaratoarea
    restart_timer = defer RestartCountdown(restart_countdown);
    
    new action[64];
    format(action, sizeof(action), "Scheduled restart: %d seconds", seconds_param);
    LogAdminAction(playerid, playerid, action);
    return 1;
}

YCMD:stoprestart(playerid, params[], help) {
    if(PlayerData[playerid][pAdmin] < 7) return 1;
    if(restart_countdown <= 0) return SendClientMessage(playerid, COLOR_RED, "Nu exista niciun restart programat.");

    restart_countdown = 0;
    
    // Oprim timer-ul activ folosind variabila stocata
    stop restart_timer; 
    
    SendClientMessageToAll(COLOR_LIGHTPURPLE, "[RESTART] " C_WHITE "Restartul serverului a fost anulat de catre un administrator.");
    GameTextForAll("~g~RESTART ANULAT", 3000, 3);
    return 1;
}