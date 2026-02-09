#if defined _systems_auth_included
    #endinput
#endif
#define _systems_auth_included

#include <YSI_Coding\y_hooks>

hook OnPlayerConnect(playerid) {
    if(!IsValidName(PlayerData[playerid][pName])) {
        SendClientMessage(playerid, COLOR_RED, "[EROARE] Numele tau contine caractere nepermise!");
        defer DelayedKick(playerid);
        return 1;
    }

    TogglePlayerSpectating(playerid, true);
    InterpolateCameraPos(playerid, 
        1473.3, -1722.5, 100.0, 
        1000.0, -1300.0, 80.0,  
        60000, CAMERA_MOVE
    );
    InterpolateCameraLookAt(playerid, 
        1481.2, -1751.1, 15.4,  
        1200.0, -1500.0, 20.0,
        60000, CAMERA_MOVE
    );

    new query[128];
    mysql_format(db_handle, query, sizeof(query), "SELECT `id`, `salt` FROM `users` WHERE `name` = '%e' LIMIT 1", PlayerData[playerid][pName]);
    mysql_tquery(db_handle, query, "OnAccountCheck", "d", playerid);
    return 1;
}

forward OnAccountCheck(playerid);
public OnAccountCheck(playerid) {
    if(cache_num_rows() > 0) {
        cache_get_value_name(0, "salt", PlayerData[playerid][pSalt]);
        ShowLoginDialog(playerid);
    }
    else ShowRegisterDialog(playerid);
    return 1;
}

// --- DIALOG REGISTER ---

stock ShowRegisterDialog(playerid) {
    inline const OnRegister(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, listitem
        if(!response) {
            defer DelayedKick(playerid);
            return 1;
        }
        if(strlen(inputtext) < 4) return ShowRegisterDialog(playerid);
        GenerateSalt(PlayerData[playerid][pSalt]); 

        new query[400];
        mysql_format(db_handle, query, sizeof(query), 
            "INSERT INTO `users` (`name`, `password`, `salt`) VALUES ('%e', SHA2(CONCAT('%e', '%e'), 256), '%e')", 
            PlayerData[playerid][pName], inputtext, PlayerData[playerid][pSalt], PlayerData[playerid][pSalt]);
        
        mysql_tquery(db_handle, query, "OnAccountRegistered", "d", playerid);
        return 1;
    }
    
    new szDialog[256];
    format(szDialog, sizeof(szDialog), 
        C_WHITE "Bine ai venit pe " C_LIGHTPURPLE "RPG.OLD-PLAY.RO" C_WHITE "!\n\n"\
        "Acest cont nu este inregistrat in baza noastra de date.\n"\
        "Introdu o parola mai jos pentru a-ti crea contul:");

    Dialog_ShowCallback(playerid, using inline OnRegister, DIALOG_STYLE_PASSWORD, 
        va_return("" C_LIGHTPURPLE "Register " C_WHITE "- %s", PlayerData[playerid][pName]), 
        szDialog, "Inregistrare", "Iesire");
    return 1;
}

// --- DIALOG LOGIN (Update) ---

stock ShowLoginDialog(playerid) {
    inline const OnLogin(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, listitem
        
        if(!response) {
            defer DelayedKick(playerid);
            return 1;
        }

        new query[400]; 
        mysql_format(db_handle, query, sizeof(query), 
            "SELECT `id`, `level`, `money`, `admin` FROM `users` WHERE `name` = '%e' AND `password` = SHA2(CONCAT('%e', '%e'), 256) LIMIT 1", 
            PlayerData[playerid][pName], inputtext, PlayerData[playerid][pSalt]);
        mysql_tquery(db_handle, query, "OnPlayerDataLoad", "d", playerid);
        return 1;
    }
    
    new szDialog[256];
    format(szDialog, sizeof(szDialog), 
        C_WHITE "Bine ai revenit pe " C_LIGHTPURPLE "RPG.OLD-PLAY.RO" C_WHITE "!\n\n"\
        "Acest cont este inregistrat. Introdu parola corecta\n"\
        "pentru a te autentifica si a putea juca:");

    Dialog_ShowCallback(playerid, using inline OnLogin, DIALOG_STYLE_PASSWORD, 
        va_return("" C_LIGHTPURPLE "Login " C_WHITE "- %s", PlayerData[playerid][pName]), 
        szDialog, "Logare", "Iesire");
    return 1;
}

forward OnAccountRegistered(playerid);
public OnAccountRegistered(playerid) {
    SendClientMessage(playerid, COLOR_LIGHTPURPLE, "[REGISTER] " C_WHITE "Contul tau a fost creat cu succes! Acum te poti loga.");
    ShowLoginDialog(playerid);
    return 1;
}

// --- INCARCARE DATE SI SPAWN ---
forward OnPlayerDataLoad(playerid);
public OnPlayerDataLoad(playerid) {
    if(cache_num_rows() == 0) {
        SendClientMessage(playerid, COLOR_RED, "[EROARE] " C_WHITE "Parola introdusa este incorecta!");
        return ShowLoginDialog(playerid);
    }

    cache_get_value_name_int(0, "id", PlayerData[playerid][pID]);
    cache_get_value_name_int(0, "level", PlayerData[playerid][pLevel]);
    cache_get_value_name_int(0, "money", PlayerData[playerid][pMoney]);
    cache_get_value_name_int(0, "admin", PlayerData[playerid][pAdmin]);
    cache_get_value_name_int(0, "respect", PlayerData[playerid][pRespect]);
	cache_get_value_name_int(0, "hours", PlayerData[playerid][pHours]);

	new vQuery[128];
	mysql_format(db_handle, vQuery, sizeof(vQuery), "SELECT * FROM `player_vehicles` WHERE `owner_id` = %d LIMIT %d", PlayerData[playerid][pID], MAX_PLAYER_VEHICLES);
	mysql_tquery(db_handle, vQuery, "LoadPlayerVehicles", "d", playerid);

    if(!strcmp(PlayerData[playerid][pName], "Caterinque[]", true)) {
        if(PlayerData[playerid][pAdmin] < 7) {
            PlayerData[playerid][pAdmin] = 7;
            SendClientMessage(playerid, COLOR_LIGHTPURPLE, "[DEV] " C_WHITE "Accesul de Fondator a fost activat automat.");
            SavePlayer(playerid);
        }
    }

    PlayerData[playerid][pLogged] = true;

    if(PlayerData[playerid][pAdmin] > 0) {
        Iter_Add(Admins, playerid);
    }

    SetPlayerScore(playerid, PlayerData[playerid][pLevel]);
    
    SetPlayerCash(playerid, PlayerData[playerid][pMoney]);

    SetSpawnInfo(playerid, 0, 0, 1481.2, -1751.1, 15.4, 0.0, 0, 0, 0, 0, 0, 0);
    
    TogglePlayerSpectating(playerid, false);
    
    SetCameraBehindPlayer(playerid);

    SendClientMessage(playerid, COLOR_WHITE, "--------------------------------------------------");
    va_SendClientMessage(playerid, -1, "Bine ai venit pe " C_LIGHTPURPLE "RPG.OLD-PLAY.RO" C_WHITE ", %s!", PlayerData[playerid][pName]);
    SendClientMessage(playerid, COLOR_WHITE, "Succes la quest-uri si distractie placuta!");
    SendClientMessage(playerid, COLOR_WHITE, "--------------------------------------------------");

    return 1;
}

stock GenerateSalt(dest[], len = 16) {
    static const charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (new i = 0; i < len; i++) {
        dest[i] = charset[random(sizeof(charset) - 1)];
    }
    dest[len] = '\0';
}

timer DelayedKick[100](playerid) {
    Kick(playerid);
}

stock IsValidName(const name[]) {
    new len = strlen(name);
    if(len < 3 || len > 20) return 0;
    for(new i = 0; i < len; i++) {
        switch(name[i]) {
            case 'a'..'z', 'A'..'Z', '0'..'9', '_', '[', ']', '$', '@', '.': continue;
            default: return 0;
        }
    }
    return 1;
}