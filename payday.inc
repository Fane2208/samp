#if defined _systems_payday_included
    #endinput
#endif
#define _systems_payday_included

#include <YSI_Coding\y_hooks>

// Task care verifica ora la fiecare minut (60000 ms)
task PayDayCheck[60000]() {
    new h, m, s;
    gettime(h, m, s);

    // Daca minutul este 0, inseamna ca este ora fixa
    if(m == 0) {
        TriggerPayDay();
    }
}

stock TriggerPayDay() {
    print("[SERVER] Se proceseaza PayDay-ul pentru toti jucatorii online...");

    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;

        // 1. Calcule: Salariul depinde de level
        new salary = PlayerData[i][pLevel] * 1250; 
        new needed_rp = (PlayerData[i][pLevel] + 1) * 3; // Formula RP necesar pt level up

        // 2. Actualizare date
        PlayerData[i][pRespect]++;
        PlayerData[i][pHours]++;
        GivePlayerCash(i, salary);

        // 3. Mesaj PayDay (Clean & Organized)
        SendClientMessage(i, COLOR_LIGHTPURPLE, "______________________________________________________");
        SendClientMessage(i, -1, "{BF94E4}PAYDAY: {FFFFFF}Ai primit salariul orar!");
        va_SendClientMessage(i, -1, "Salariu: {66FF66}$%d {FFFFFF}| Bonus: {66FF66}$0", salary);
        va_SendClientMessage(i, -1, "Respect Points: {BF94E4}%d/%d {FFFFFF}| Ore jucate: {BF94E4}%d", 
            PlayerData[i][pRespect], needed_rp, PlayerData[i][pHours]);
        
        // 4. Verificare Level Up
        if(PlayerData[i][pRespect] >= needed_rp) {
            PlayerData[i][pLevel]++;
            PlayerData[i][pRespect] = 0; // Resetam RP-ul
            SetPlayerScore(i, PlayerData[i][pLevel]);
            
            SendClientMessage(i, COLOR_LIGHTPURPLE, "* Felicitari! Ai avansat la level-ul %d!", PlayerData[i][pLevel]);
            PlayerPlaySound(i, 1052, 0.0, 0.0, 0.0); // Sunet de level up
        }

        SendClientMessage(i, COLOR_LIGHTPURPLE, "______________________________________________________");
        
        // Salvare imediata pentru a preveni pierderile de date la crash dupa PayDay
        SavePlayer(i);
    }
    return 1;
}

// Comanda de test pentru admini (level 7)
YCMD:forcepayday(playerid, params[], help) {
    if(PlayerData[playerid][pAdmin] < 7) return 1;
    
    TriggerPayDay();
    SendClientMessage(playerid, COLOR_RED, "[ADMIN] Ai forat un PayDay global.");
    return 1;
}