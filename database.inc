#if defined _systems_database_included
    #endinput
#endif
#define _systems_database_included

#include <YSI_Coding\y_hooks>

// Handle-ul global pentru baza de date
new MySQL:db_handle;

hook OnGameModeInit() {
    // Conectare directă - metoda cea mai compatibilă și stabilă
    // Parametrii: Host, User, Pass, Database
    db_handle = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB);

    // Verificăm dacă handle-ul este valid
    if(db_handle == MYSQL_INVALID_HANDLE || mysql_errno(db_handle) != 0) {
        print("\n--------------------------------------------------");
        print("[MySQL] EROARE CRITICA: Nu m-am putut conecta la baza de date!");
        print("[MySQL] Verifica systems/config.inc si porneste serverul MySQL.");
        print("--------------------------------------------------\n");
        
        // Pe VPS, e vital să nu lăsăm serverul pornit fără DB
        SendRconCommand("exit"); 
        return 1;
    }

    print("\n--------------------------------------------------");
    print(C_LIGHTPURPLE "[MySQL] Conexiune reusita!");
    print(C_WHITE "[MySQL] Baza de date a fost incarcata modular.");
    print("--------------------------------------------------\n");
    return 1;
}

hook OnGameModeExit() {
    if(db_handle != MYSQL_INVALID_HANDLE) {
        mysql_close(db_handle);
    }
    return 1;
}