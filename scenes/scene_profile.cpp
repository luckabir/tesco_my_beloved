#include "scene_menu.h"   
#include "scene_profile.h"                  
#include "../managers/InputManager.h" 
#include "../structures/Profile.h"
#include "../structures/Settings.h"
#include "../managers/AssetManager.h"
#include "raylib.h"
#include <cmath>
#include <fstream>

static ProfileSubState subState = SUB_SEZNAM;
static std::string selectedProfileName = "";
static Profile inspectedProfile; 

static Texture2D profileTexture = { 0 };
static bool isTextureLoaded = false;

static char nameInput[16] = "\0";
static char pinInput[5] = "\0";
static int nameLetterCount = 0;
static int pinLetterCount = 0;
static bool errorWrongPin = false;

extern bool resetGameSignal;

void runProfile(GameState& currentState, InputManager& input, bool& isGamePaused)
{
    Vector2 mousePos = GetMousePosition();
    float scale = fminf((float)GetScreenWidth() / 800.0f, (float)GetScreenHeight() / 600.0f);
    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    Rectangle globalBackBtn = { 20, 20, 130, 35 };
    DrawRectangleRec(globalBackBtn, CheckCollisionPointRec(mousePos, globalBackBtn) ? LIGHTGRAY : GRAY);
    DrawTextEx(AssetManager::mainFont, "MENU", Vector2{ globalBackBtn.x + 35, globalBackBtn.y + 10 }, 14.0f, 1.0f, BLACK);
    if (CheckCollisionPointRec(mousePos, globalBackBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (isTextureLoaded) {
            UnloadTexture(profileTexture);
            isTextureLoaded = false;
        }
        currentState = STATE_MENU;
        return;
    }

    // ==========================================
    // 1. OKNO: SEZNAM PROFILŮ (Nebo detail přihlášeného)
    // ==========================================
    if (subState == SUB_SEZNAM)
    {
        if (isUserLoggedIn) 
        {
            DrawTextEx(AssetManager::mainFont, "PRIHLASENY ZAMESTNANEC", Vector2{ 260, 50 }, 22.0f, 1.0f, BLACK);

            if (!isTextureLoaded) {
                inspectedProfile = activeProfile;
                profileTexture = LoadTexture(inspectedProfile.profilePicturePath.c_str());
                isTextureLoaded = true;
            }

            if (isTextureLoaded && profileTexture.id > 0) {
                DrawTexturePro(
                    profileTexture, 
                    Rectangle{ 0, 0, (float)profileTexture.width, (float)profileTexture.height },
                    Rectangle{ 180, 130, 100, 100 }, 
                    Vector2{ 0, 0 }, 0.0f, WHITE
                );
            } else {
                DrawRectangle(180, 130, 100, 100, GRAY); 
            }

            DrawTextEx(AssetManager::mainFont, TextFormat("Jmeno            %s", activeProfile.nickname.c_str()), Vector2{ 310, 130 }, 16.0f, 1.0f, BLACK);
            DrawTextEx(AssetManager::mainFont, TextFormat("Kasa ID          %04d", activeProfile.employeeId), Vector2{ 310, 155 }, 14.0f, 1.0f, DARKGRAY);
            DrawTextEx(AssetManager::mainFont, TextFormat("Pozice           %s", activeProfile.GetRankName().c_str()), Vector2{ 310, 180 }, 14.0f, 1.0f, RED);
            DrawTextEx(AssetManager::mainFont, TextFormat("Trzba            %d Kc", activeProfile.maxScore), Vector2{ 310, 205 }, 14.0f, 1.0f, GOLD);

            Rectangle logoutBtn = { 250, 320, 300, 45 };
            bool hoverLogout = CheckCollisionPointRec(mousePos, logoutBtn);
            DrawRectangleRec(logoutBtn, hoverLogout ? RED : MAROON);
            DrawTextEx(AssetManager::mainFont, "ODCHOD Z PRACE", Vector2{ logoutBtn.x + 20, logoutBtn.y + 14 }, 14.0f, 1.0f, WHITE);

            if (hoverLogout && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
            {
                if (isTextureLoaded) {
                    UnloadTexture(profileTexture);
                    isTextureLoaded = false;
                }
                LogoutProfile(); 
                isGamePaused = false;    
                resetGameSignal = true;  
                currentState = STATE_MENU; 
                return;
            }
        }
        else 
        {
            DrawTextEx(AssetManager::mainFont, "KARTA ZAMESTNANCU", Vector2{ 220, 50 }, 22.0f, 1.0f, BLACK);

            int startY = 130;
            for (size_t i = 0; i < allProfiles.size(); i++)
            {
                Rectangle profileSlot = { 200, (float)(startY + i * 55), 320, 45 };
                Rectangle deleteSlot = { 530, (float)(startY + i * 55), 45, 45 };

                bool hoverProfile = CheckCollisionPointRec(mousePos, profileSlot);
                bool hoverDelete = CheckCollisionPointRec(mousePos, deleteSlot);

                DrawRectangleRec(profileSlot, hoverProfile ? LIGHTGRAY : GRAY); 
                DrawTextEx(AssetManager::mainFont, allProfiles[i].c_str(), Vector2{ profileSlot.x + 20, profileSlot.y + 15 }, 16.0f, 1.0f, BLACK);

                DrawRectangleRec(deleteSlot, hoverDelete ? RED : MAROON);
                DrawTextEx(AssetManager::mainFont, "X", Vector2{ deleteSlot.x + 16, deleteSlot.y + 14 }, 16.0f, 1.0f, WHITE);

                if (hoverProfile && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    selectedProfileName = allProfiles[i];
                    std::string filename = "profiles/profil_" + selectedProfileName + ".txt";
                    std::ifstream file(filename);
                    if (file.is_open()) {
                        int rankInt;
                        file >> inspectedProfile.profilePicturePath;
                        file >> inspectedProfile.nickname;
                        file >> rankInt; inspectedProfile.rank = (PlayerRank)rankInt;
                        file >> inspectedProfile.employeeId;
                        file >> inspectedProfile.pinCode;
                        file >> inspectedProfile.daysUntilPasswordChange;
                        file >> inspectedProfile.maxScore;
                        file >> inspectedProfile.save_id;
                        file >> inspectedProfile.shiftsCompleted;
                        file >> inspectedProfile.customersServed;
                        file.close();

                        if (isTextureLoaded) {
                            UnloadTexture(profileTexture);
                            isTextureLoaded = false;
                        }
                        profileTexture = LoadTexture(inspectedProfile.profilePicturePath.c_str());
                        isTextureLoaded = true;
                    }
                    pinInput[0] = '\0'; pinLetterCount = 0; 
                    errorWrongPin = false;
                    subState = SUB_DETAIL;
                }

                if (hoverDelete && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    DeleteProfile(allProfiles[i]);
                    break; 
                }
            }

            Rectangle newProfileBtn = { 250, 480, 300, 45 };
            bool hoverNew = CheckCollisionPointRec(mousePos, newProfileBtn);
            DrawRectangleRec(newProfileBtn, hoverNew ? LIME : GREEN);
            DrawTextEx(AssetManager::mainFont, "NOVY BRIGADNIK", Vector2{ newProfileBtn.x + 65, newProfileBtn.y + 14 }, 14.0f, 1.0f, WHITE);

            if (hoverNew && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                nameInput[0] = '\0'; nameLetterCount = 0;
                pinInput[0] = '\0'; pinLetterCount = 0;
                subState = SUB_EDITACE; 
                selectedProfileName = ""; 
            }
        }
    }

    // ==========================================
    // 2. OKNO: DETAIL PROFILU & PŘIHLÁŠENÍ
    // ==========================================
    else if (subState == SUB_DETAIL)
    {
        DrawTextEx(AssetManager::mainFont, "PROFIL ZAMESTNANCE", Vector2{ 280, 50 }, 22.0f, 1.0f, BLACK);

        if (isTextureLoaded && profileTexture.id > 0) {
            DrawTexturePro(
                profileTexture, 
                Rectangle{ 0, 0, (float)profileTexture.width, (float)profileTexture.height },
                Rectangle{ 180, 130, 100, 100 }, 
                Vector2{ 0, 0 }, 0.0f, WHITE
            );
        } else {
            DrawRectangle(180, 130, 100, 100, GRAY); 
            DrawTextEx(AssetManager::mainFont, "FOTO", Vector2{ 210, 170 }, 12.0f, 1.0f, WHITE);
        }

        DrawTextEx(AssetManager::mainFont, TextFormat("Jmeno            %s", inspectedProfile.nickname.c_str()), Vector2{ 310, 130 }, 16.0f, 1.0f, BLACK);
        DrawTextEx(AssetManager::mainFont, TextFormat("Kasa ID          %04d", inspectedProfile.employeeId), Vector2{ 310, 155 }, 14.0f, 1.0f, DARKGRAY);
        DrawTextEx(AssetManager::mainFont, TextFormat("Pozice           %s", inspectedProfile.GetRankName().c_str()), Vector2{ 310, 180 }, 14.0f, 1.0f, RED);
        DrawTextEx(AssetManager::mainFont, TextFormat("Trzba            %d Kc", inspectedProfile.maxScore), Vector2{ 310, 205 }, 14.0f, 1.0f, GOLD);

        DrawTextEx(AssetManager::mainFont, "ZADEJTE PIN PRO VSTUP DO KASY", Vector2{ 250, 280 }, 14.0f, 1.0f, BLACK);
        
        char maskedPin[5] = {0};
        for(int i=0; i<pinLetterCount; i++) maskedPin[i] = '*';
        DrawRectangle(350, 310, 100, 35, LIGHTGRAY);
        DrawTextEx(AssetManager::mainFont, maskedPin, Vector2{ 380, 320 }, 18.0f, 1.0f, BLACK);

        int key = GetCharPressed();
        if ((key >= '0') && (key <= '9') && (pinLetterCount < 4)) {
            pinInput[pinLetterCount] = (char)key;
            pinLetterCount++;
            pinInput[pinLetterCount] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && pinLetterCount > 0) {
            pinLetterCount--;
            pinInput[pinLetterCount] = '\0';
        }

        if (errorWrongPin) {
            DrawTextEx(AssetManager::mainFont, "NEPLATNY PIN", Vector2{ 350, 355 }, 12.0f, 1.0f, RED);
        }

        Rectangle loginBtn = { 275, 390, 250, 45 };
        if (CheckCollisionPointRec(mousePos, loginBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int enteredPin = atoi(pinInput);
            if (LoadProfile(inspectedProfile.nickname, enteredPin)) {
                if (isTextureLoaded) {
                    UnloadTexture(profileTexture);
                    isTextureLoaded = false;
                }
                subState = SUB_SEZNAM; 
                SaveSettings();
                currentState = STATE_MENU;
            } else {
                errorWrongPin = true;
                pinLetterCount = 0; pinInput[0] = '\0';
            }
        }
        DrawRectangleRec(loginBtn, GREEN);
        DrawTextEx(AssetManager::mainFont, "POTVRDIT PIN", Vector2{ loginBtn.x + 65, loginBtn.y + 14 }, 14.0f, 1.0f, WHITE);

        Rectangle backToListBtn = { 275, 450, 250, 45 };
        if (CheckCollisionPointRec(mousePos, backToListBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (isTextureLoaded) {
                UnloadTexture(profileTexture);
                isTextureLoaded = false;
            }
            subState = SUB_SEZNAM;
        }
        DrawRectangleRec(backToListBtn, GRAY);
        DrawTextEx(AssetManager::mainFont, "ZPET NA SEZNAM", Vector2{ backToListBtn.x + 60, backToListBtn.y + 14 }, 14.0f, 1.0f, WHITE);
    }

    // ==========================================
    // 3. OKNO: TVORBA / EDITACE PROFILU
    // ==========================================
    else if (subState == SUB_EDITACE)
    {
        static int activeTextBox = 0; 

        DrawTextEx(AssetManager::mainFont, "REGISTRACE NOVEHO BRIGADNIKA", Vector2{ 220, 50 }, 22.0f, 1.0f, BLACK);

        Rectangle nameBox = { 250, 180, 300, 40 };
        Rectangle pinBox = { 250, 280, 150, 40 };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, nameBox)) {
                activeTextBox = 1; 
            } else if (CheckCollisionPointRec(mousePos, pinBox)) {
                activeTextBox = 2; 
            } else {
                activeTextBox = 0; 
            }
        }

        DrawTextEx(AssetManager::mainFont, "JMENO", Vector2{ 250, 150 }, 14.0f, 1.0f, DARKGRAY);
        DrawRectangleRec(nameBox, LIGHTGRAY);
        DrawRectangleLines((int)nameBox.x, (int)nameBox.y, (int)nameBox.width, (int)nameBox.height, (activeTextBox == 1) ? BLUE : GRAY);
        DrawTextEx(AssetManager::mainFont, nameInput, Vector2{ nameBox.x + 15, nameBox.y + 12 }, 16.0f, 1.0f, BLACK);

        DrawTextEx(AssetManager::mainFont, "ZVOLTE SI 4MISTNY PIN KASY", Vector2{ 250, 250 }, 14.0f, 1.0f, DARKGRAY);
        DrawRectangleRec(pinBox, LIGHTGRAY);
        DrawRectangleLines((int)pinBox.x, (int)pinBox.y, (int)pinBox.width, (int)pinBox.height, (activeTextBox == 2) ? BLUE : GRAY);
        DrawTextEx(AssetManager::mainFont, pinInput, Vector2{ pinBox.x + 15, pinBox.y + 12 }, 16.0f, 1.0f, BLACK);

        int key = GetCharPressed();
        
        if (activeTextBox == 1) 
        {
            while (key > 0) {
                if (((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') || (key >= '0' && key <= '9')) && (nameLetterCount < 15)) {
                    nameInput[nameLetterCount] = (char)key;
                    nameLetterCount++;
                    nameInput[nameLetterCount] = '\0';
                }
                key = GetCharPressed(); 
            }
            if (IsKeyPressed(KEY_BACKSPACE) && nameLetterCount > 0) {
                nameLetterCount--;
                nameInput[nameLetterCount] = '\0';
            }
        }
        else if (activeTextBox == 2) 
        {
            while (key > 0) {
                if ((key >= '0' && key <= '9') && (pinLetterCount < 4)) {
                    pinInput[pinLetterCount] = (char)key;
                    pinLetterCount++;
                    pinInput[pinLetterCount] = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && pinLetterCount > 0) {
                pinLetterCount--;
                pinInput[pinLetterCount] = '\0';
            }
        }

        DrawTextEx(AssetManager::mainFont, "PROFILOVY OBRAZEK", Vector2{ 250, 350 }, 14.0f, 1.0f, DARKGRAY);
        Rectangle filePhotoBtn = { 250, 380, 300, 35 };
        DrawRectangleRec(filePhotoBtn, CheckCollisionPointRec(mousePos, filePhotoBtn) ? LIGHTGRAY : GRAY);
        DrawTextEx(AssetManager::mainFont, "Vybrat rucne ze souboru", Vector2{ filePhotoBtn.x + 40, filePhotoBtn.y + 10 }, 12.0f, 1.0f, WHITE);
        
        if (CheckCollisionPointRec(mousePos, filePhotoBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            activeProfile.profilePicturePath = "ASSets/profile/default_avatar.png";
        }

        Rectangle finalizeBtn = { 250, 460, 300, 45 };
        bool canFinalize = (nameLetterCount > 0 && pinLetterCount == 4);
        
        DrawRectangleRec(finalizeBtn, canFinalize ? (CheckCollisionPointRec(mousePos, finalizeBtn) ? LIME : GREEN) : GRAY);
        DrawTextEx(AssetManager::mainFont, "PODPISAT SMLOUVU", Vector2{ finalizeBtn.x + 45, finalizeBtn.y + 14 }, 14.0f, 1.0f, WHITE);

        if (canFinalize && CheckCollisionPointRec(mousePos, finalizeBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            CreateProfile(nameInput, atoi(pinInput));
            subState = SUB_SEZNAM;
            activeTextBox = 0; 
        }

        Rectangle cancelBtn = { 250, 515, 300, 35 };
        DrawRectangleRec(cancelBtn, CheckCollisionPointRec(mousePos, cancelBtn) ? RED : MAROON);
        DrawTextEx(AssetManager::mainFont, "ZRUSIT", Vector2{ cancelBtn.x + 120, cancelBtn.y + 10 }, 14.0f, 1.0f, WHITE);

        if (CheckCollisionPointRec(mousePos, cancelBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            subState = SUB_SEZNAM;
            activeTextBox = 0;
        }
    }
}