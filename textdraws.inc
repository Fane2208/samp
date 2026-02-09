#if defined _systems_textdraws_included
    #endinput
#endif
#define _systems_textdraws_included

#include <YSI_Coding\y_hooks>

// Variabile pentru TextDraw-uri
new PlayerText:LogoTD[MAX_PLAYERS];
new PlayerText:ServerTimeTD[MAX_PLAYERS];

hook OnPlayerConnect(playerid) {
    // --- TEXTDRAW LOGO (Nume, ID, Server) ---
    // Pozitionat de obicei in partea dreapta jos sau sus (langa bani)
    // Coordonate aproximative pentru dreapta-jos: 490.0, 430.0
    
    new str[128];
    format(str, sizeof(str), "%s(ID:%d) ~p~RPG.OLD-PLAY.RO", PlayerData[playerid][pName], playerid);
    
    LogoTD[playerid] = CreatePlayerTextDraw(playerid, 630.0, 430.0, str);
    PlayerTextDrawAlignment(playerid, LogoTD[playerid], 3); // Aliniere la dreapta
    PlayerTextDrawBackgroundColor(playerid, LogoTD[playerid], 255);
    PlayerTextDrawFont(playerid, LogoTD[playerid], 2);
    PlayerTextDrawLetterSize(playerid, LogoTD[playerid], 0.23, 1.2);
    PlayerTextDrawColor(playerid, LogoTD[playerid], -1);
    PlayerTextDrawSetOutline(playerid, LogoTD[playerid], 1);
    PlayerTextDrawSetProportional(playerid, LogoTD[playerid], 1);

    // --- TEXTDRAW CEAS & DATA ---
    ServerTimeTD[playerid] = CreatePlayerTextDraw(playerid, 630.0, 10.0, "00:00~n~~p~01/01/2024");
    PlayerTextDrawAlignment(playerid, ServerTimeTD[playerid], 3); // Aliniere la dreapta
    PlayerTextDrawBackgroundColor(playerid, ServerTimeTD[playerid], 255);
    PlayerTextDrawFont(playerid, ServerTimeTD[playerid], 3);
    PlayerTextDrawLetterSize(playerid, ServerTimeTD[playerid], 0.35, 1.4);
    PlayerTextDrawColor(playerid, ServerTimeTD[playerid], -1);
    PlayerTextDrawSetOutline(playerid, ServerTimeTD[playerid], 1);
    PlayerTextDrawSetProportional(playerid, ServerTimeTD[playerid], 1);

    return 1;
}

// Afisam textdraw-urile cand se spawneaza (dupa logare)
hook OnPlayerSpawn(playerid) {
    PlayerTextDrawShow(playerid, LogoTD[playerid]);
    PlayerTextDrawShow(playerid, ServerTimeTD[playerid]);
    return 1;
}

// Timer care se executa la fiecare secunda pentru a actualiza ceasul
task UpdateServerTime[1000]() {
    new h, m, s, d, mo, y, str[64];
    gettime(h, m, s);
    getdate(y, mo, d);

    format(str, sizeof(str), "%02d:%02d~n~~p~%02d/%02d/%d", h, m, d, mo, y);

    foreach(new i : Player) {
        if(PlayerData[i][pLogged]) {
            PlayerTextDrawSetString(i, ServerTimeTD[i], str);
        }
    }
}

hook OnPlayerDisconnect(playerid, reason) {
    // Distrugem textdraw-urile pentru a elibera memoria
    PlayerTextDrawDestroy(playerid, LogoTD[playerid]);
    PlayerTextDrawDestroy(playerid, ServerTimeTD[playerid]);
    return 1;
}