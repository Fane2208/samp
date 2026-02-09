#if defined _systems_player_main_included
    #endinput
#endif
#define _systems_player_main_included

enum E_PLAYER_DATA {
    pID,
    pName[MAX_PLAYER_NAME],
    pPassword[64],
    pSalt[17],
    pLevel,
    pMoney,
    pAdmin,
    pRespect, 
    pHours,   
    bool:pLogged,
    bool:pAdminDuty,
    Text3D:pAdminLabel
};

new PlayerData[MAX_PLAYERS][E_PLAYER_DATA];

new Iterator:Admins<MAX_PLAYERS>;

new Float:VehicleFuel[MAX_VEHICLES]; 

#define MAX_PLAYER_VEHICLES 5

enum E_VEHICLE_DATA {
    pvID,           
    pvModel,
    pvPrice,
    Float:pvKM,
    pvAge,
    pvColor1,
    pvColor2,
    pvInsurance,
    Float:pvPos[4],
    pvGameID,      
    bool:pvSpawned,
    Float:pvFuel
};

new PlayerVehicle[MAX_PLAYERS][MAX_PLAYER_VEHICLES][E_VEHICLE_DATA];

hook OnPlayerConnect(playerid) {
    GetPlayerName(playerid, PlayerData[playerid][pName], MAX_PLAYER_NAME);
    
    PlayerData[playerid][pID] = 0;
    PlayerData[playerid][pLogged] = false;
    PlayerData[playerid][pLevel] = 1;
    PlayerData[playerid][pMoney] = 500; 
    PlayerData[playerid][pAdmin] = 0;
    return 1;
}

// Hook la deconectare pentru salvare automata
// În systems/player_main.inc -> hook OnPlayerDisconnect

hook OnPlayerDisconnect(playerid, reason) {
    if(PlayerData[playerid][pLogged]) {
        for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
            if(PlayerVehicle[playerid][v][pvID] != 0) {
                if(PlayerVehicle[playerid][v][pvSpawned]) {
                    PlayerVehicle[playerid][v][pvFuel] = VehicleFuel[PlayerVehicle[playerid][v][pvGameID]];
                }
                new query[256];
                mysql_format(db_handle, query, sizeof(query), 
                    "UPDATE `player_vehicles` SET `km` = %f, `age` = %d, `fuel` = %f WHERE `id` = %d", 
                    PlayerVehicle[playerid][v][pvKM], 
                    PlayerVehicle[playerid][v][pvAge], 
                    PlayerVehicle[playerid][v][pvFuel], 
                    PlayerVehicle[playerid][v][pvID]
                );
                mysql_tquery(db_handle, query);

                if(PlayerVehicle[playerid][v][pvSpawned]) {
                    DestroyVehicle(PlayerVehicle[playerid][v][pvGameID]);
                }
            }
        }
        SavePlayer(playerid);
    }
    return 1;
}

stock SetPlayerCash(playerid, amount) {
    PlayerData[playerid][pMoney] = amount;
    ResetPlayerMoney(playerid); 
    GivePlayerMoney(playerid, amount); 
    return 1;
}

stock GivePlayerCash(playerid, amount) {
    PlayerData[playerid][pMoney] += amount;
    GivePlayerMoney(playerid, amount); 
    return 1;
}

stock GetPlayerCash(playerid) {
    return PlayerData[playerid][pMoney];
}

stock SavePlayer(playerid) {
    if(!PlayerData[playerid][pLogged]) return 0;

    new query[512];
    mysql_format(db_handle, query, sizeof(query), 
        "UPDATE `users` SET `level` = %d, `money` = %d, `admin` = %d, `respect` = %d, `hours` = %d WHERE `id` = %d",
        PlayerData[playerid][pLevel],
        PlayerData[playerid][pMoney],
        PlayerData[playerid][pAdmin],
        PlayerData[playerid][pRespect],
        PlayerData[playerid][pHours],
        PlayerData[playerid][pID]
    );
    mysql_tquery(db_handle, query);
    return 1;
}

// Task-ul de salvare la 10 minute
task AutomaticAccountSave[600000]() {
    new savedCount = 0;
    foreach(new i : Player) { 
        if(PlayerData[i][pLogged]) {
            SavePlayer(i);
            savedCount++;
        }
    }
    if(savedCount > 0) {
        printf("[SQL] Salvare automata efectuata pentru %d jucatori online.", savedCount);
    }
}