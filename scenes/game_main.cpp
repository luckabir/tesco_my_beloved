#include "menu.h"
#include "game_main.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

// --- STRUKTURY PRO HRU ---
struct Item {
    std::string name;
    int price;
    Color color;
    Vector2 pos;
    bool isSpawned;
    bool isScanned;
    Rectangle getRect() const { return Rectangle{ pos.x, pos.y, 50, 40 }; }
};

struct Hand {
    Vector2 pos;
    bool isHolding;
    int holdingItemIndex; // -1 pokud nic nedrží
    Color color;
};

void runGame(GameState &currentState, InputManager &input, bool& isGamePaused) {
    SetExitKey(KEY_NULL);

    // --- STATIC PROMĚNNÉ PRO STAV HRY ---
    static bool initialized = false;
    
    // Ruce hráče
    static Hand leftHand;
    static Hand rightHand;
    
    // Položky v aktuálním nákupu
    static std::vector<Item> currentCart;
    static int currentItemIndex = 0; // Které zboží je zrovna na řadě
    
    // Účtenka / Monitor
    static std::vector<std::string> receiptHistory;
    static int totalSum = 0;
    static bool waitingForPayment = false;

    // --- INICIALIZACE PŘI PRVNÍM SPUŠTĚNÍ ---
    if (!initialized) {
        leftHand = { Vector2{ 250, 300 }, false, -1, GREEN };
        rightHand = { Vector2{ 550, 300 }, false, -1, ORANGE };
        
        // Vygenerujeme testovací nákup jednoho zákazníka
        currentCart = {
            { "Rohlik", 3, BROWN, { -50, 410 }, true, false },
            { "Tesco Mleko", 25, WHITE, { -150, 410 }, true, false },
            { "Vodka 40%", 179, SKYBLUE, { -250, 410 }, true, false },
            { "Parek", 49, PINK, { -350, 410 }, true, false }
        };
        
        initialized = true;
    }

    // --- MEZERNÍK / ESC - NÁVRAT DO MENU ---
    if (input.IsPauseTriggered() || input.IsBackTriggered()) { 
        isGamePaused = true;
        currentState = STATE_MENU;
        return;
    }

    // =================================================================
    // GLOBÁLNÍ HERNÍ LOGIKA (Běží, jen když NENÍ pauza)
    // =================================================================
    if (!isGamePaused) {

        // -------------------------------------------------------------
        // A. OVLÁDÁNÍ LEVÉ RUKY (WASD + E)
        // -------------------------------------------------------------
        float handSpeed = 5.0f;
        if (IsKeyDown(KEY_W)) leftHand.pos.y -= handSpeed;
        if (IsKeyDown(KEY_S)) leftHand.pos.y += handSpeed;
        if (IsKeyDown(KEY_A)) leftHand.pos.x -= handSpeed;
        if (IsKeyDown(KEY_D)) leftHand.pos.x += handSpeed;

        // Limity, aby ruce neuletěly z obrazovky
        leftHand.pos = Vector2Clamp(leftHand.pos, Vector2{0,0}, Vector2{800,600});

        // E - Uchopení / Pustění (Levá ruka)
        if (IsKeyPressed(KEY_E)) {
            if (!leftHand.isHolding) {
                // Zkusíme chytit zboží, které je aktivní a zrovna ho nedrží druhá ruka
                for (size_t i = 0; i < currentCart.size(); i++) {
                    if (CheckCollisionPointRec(leftHand.pos, currentCart[i].getRect()) && (int)i != rightHand.holdingItemIndex) {
                        leftHand.isHolding = true;
                        leftHand.holdingItemIndex = i;
                        break;
                    }
                }
            } else {
                // Pustíme zboží
                leftHand.isHolding = false;
                leftHand.holdingItemIndex = -1;
            }
        }

        // -------------------------------------------------------------
        // B. OVLÁDÁNÍ PRAVÉ RUKY (IJKL + O)
        // -------------------------------------------------------------
        if (IsKeyDown(KEY_I)) rightHand.pos.y -= handSpeed;
        if (IsKeyDown(KEY_K)) rightHand.pos.y += handSpeed;
        if (IsKeyDown(KEY_J)) rightHand.pos.x -= handSpeed;
        if (IsKeyDown(KEY_L)) rightHand.pos.x += handSpeed;

        rightHand.pos = Vector2Clamp(rightHand.pos, Vector2{0,0}, Vector2{800,600});

        // O - Uchopení / Pustění (Pravá ruka)
        if (IsKeyPressed(KEY_O)) {
            if (!rightHand.isHolding) {
                for (size_t i = 0; i < currentCart.size(); i++) {
                    if (CheckCollisionPointRec(rightHand.pos, currentCart[i].getRect()) && (int)i != leftHand.holdingItemIndex) {
                        rightHand.isHolding = true;
                        rightHand.holdingItemIndex = i;
                        break;
                    }
                }
            } else {
                rightHand.isHolding = false;
                rightHand.holdingItemIndex = -1;
            }
        }

        // -------------------------------------------------------------
        // C. AKTUALIZACE POZIC DRŽENÉHO ZBOŽÍ
        // -------------------------------------------------------------
        if (leftHand.isHolding) {
            currentCart[leftHand.holdingItemIndex].pos = Vector2{ leftHand.pos.x - 25, leftHand.pos.y - 20 };
        }
        if (rightHand.isHolding) {
            currentCart[rightHand.holdingItemIndex].pos = Vector2{ rightHand.pos.x - 25, rightHand.pos.y - 20 };
        }

        // -------------------------------------------------------------
        // D. FYZIKA PÁSU (Funguje jako IRL se senzorem)
        // -------------------------------------------------------------
        // Senzor je na pozici X = 280. Pokud tam zboží dorazí, pás se zastaví.
        bool beltBlocked = false;
        for (const auto& item : currentCart) {
            if (!leftHand.isHolding && !rightHand.isHolding && item.pos.x >= 240 && item.pos.x <= 250 && !item.isScanned) {
                beltBlocked = true;
            }
        }

        // Pokud není pás blokován zbožím na senzoru a nečekáme na placení, zboží jede
        if (!beltBlocked && !waitingForPayment) {
            for (size_t i = 0; i < currentCart.size(); i++) {
                // Posuneme zboží na pásu pouze tehdy, pokud ho zrovna nikdo nedrží
                if ((int)i != leftHand.holdingItemIndex && (int)i != rightHand.holdingItemIndex && currentCart[i].pos.x < 250) {
                    currentCart[i].pos.x += 1.5f;
                }
            }
        }

        // -------------------------------------------------------------
        // E. SKENOVÁNÍ ZBOŽÍ (Laserový skener uprostřed)
        // -------------------------------------------------------------
        Rectangle scannerRect = { 350, 440, 100, 20 };
        for (size_t i = 0; i < currentCart.size(); i++) {
            if (!currentCart[i].isScanned && CheckCollisionRecs(currentCart[i].getRect(), scannerRect)) {
                currentCart[i].isScanned = true;
                
                // Přidáme položku na monitor (max posledních 5)
                std::string entry = currentCart[i].name + " ... " + std::to_string(currentCart[i].price) + " Kc";
                receiptHistory.push_back(entry);
                if (receiptHistory.size() > 5) {
                    receiptHistory.erase(receiptHistory.begin());
                }
                
                totalSum += currentCart[i].price;
            }
        }

        // -------------------------------------------------------------
        // F. KONTROLA DOKONČENÍ NÁKUPU & PLACENÍ
        // -------------------------------------------------------------
        // Pokud je všechno zboží naskenované a odložené v zóně vpravo (X > 500)
        bool allDone = true;
        for (const auto& item : currentCart) {
            if (!item.isScanned || item.pos.x < 500) {
                allDone = false;
            }
        }
        
        if (allDone && currentCart.size() > 0) {
            waitingForPayment = true;
        }

        // Kliknutí na tlačítko "ZAPLACENO"
        if (waitingForPayment && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mPos = GetMousePosition(); // pro zjednodušení teď UI tlačítko na klik myší
            Rectangle payBtn = { 325, 230, 150, 40 };
            if (CheckCollisionPointRec(mPos, payBtn)) {
                // Reset hry pro dalšího zákazníka!
                waitingForPayment = false;
                totalSum = 0;
                receiptHistory.clear();
                currentCart = {
                    { "Chleb", 42, BROWN, { -50, 410 }, true, false },
                    { "Tesco Chips", 39, RED, { -150, 410 }, true, false },
                    { "Banany", 35, YELLOW, { -250, 410 }, true, false }
                };
            }
        }
    }

    // =================================================================
    // VYKRESLOVÁNÍ (800x600)
    // =================================================================
    ClearBackground(RAYWHITE); 

    // 1. KRESLENÍ ZÓN (Pás, Skener, Odkládací plocha)
    DrawRectangle(0, 450, 300, 40, DARKGRAY);       // Levý pás
    DrawRectangle(295, 450, 10, 40, RED);            // IRL Infračervený senzor (červené světýlko)
    DrawRectangle(300, 450, 200, 50, GRAY);          // Pokladní pult / skener zóna
    DrawRectangle(350, 450, 100, 10, RED);           // Sklo skeneru (laser)
    DrawRectangle(500, 450, 300, 50, LIGHTGRAY);     // Odkládací pult vpravo

    // Dekorace textů zón
    DrawText("PAS (ZBOZI)", 20, 470, 10, WHITE);
    DrawText("ODKLADAC", 720, 470, 10, DARKGRAY);

    // 2. MONITOR POKLADNY (Zobrazuje posledních 5 položek)
    DrawRectangle(300, 50, 200, 150, BLACK);
    DrawRectangleLines(300, 50, 200, 150, GREEN);
    DrawText("DISPLEJ TESCO T-2000", 310, 55, 10, GREEN);
    
    // Výpis historie položek
    for (size_t i = 0; i < receiptHistory.size(); i++) {
        DrawText(receiptHistory[i].c_str(), 310, 75 + (i * 15), 12, GREEN);
    }
    // Celková suma
    std::string sumStr = "CELKEM: " + std::to_string(totalSum) + " Kc";
    DrawText(sumStr.c_str(), 310, 175, 14, RED);

    // 3. VYKRESLENÍ ZBOŽÍ
    for (const auto& item : currentCart) {
        DrawRectangleRec(item.getRect(), item.color);
        DrawRectangleLines((int)item.pos.x, (int)item.pos.y, 50, 40, BLACK);
        DrawText(item.name.c_str(), item.pos.x + 5, item.pos.y + 12, 10, item.color.r > 200 && item.color.g > 200 ? BLACK : WHITE);
        
        // Pokud je naskenováno, uděláme na něm malou zelenou fajfku
        if (item.isScanned) {
            DrawText("[OK]", item.pos.x + 12, item.pos.y - 12, 10, GREEN);
        }
    }

    // 4. VYKRESLENÍ RUKOU (Zobrazeny jako kruhy s indikací stisku)
    // Levá ruka
    DrawCircleV(leftHand.pos, 20, leftHand.color);
    DrawCircleLines(leftHand.pos.x, leftHand.pos.y, 20, BLACK);
    DrawText("L (E)", leftHand.pos.x - 12, leftHand.pos.y - 5, 12, WHITE);
    if (leftHand.isHolding) DrawCircle(leftHand.pos.x, leftHand.pos.y, 6, RED);

    // Pravá ruka
    DrawCircleV(rightHand.pos, 20, rightHand.color);
    DrawCircleLines(rightHand.pos.x, rightHand.pos.y, 20, BLACK);
    DrawText("P (O)", rightHand.pos.x - 12, rightHand.pos.y - 5, 12, WHITE);
    if (rightHand.isHolding) DrawCircle(rightHand.pos.x, rightHand.pos.y, 6, RED);

    // 5. MODAL PRO PLACENÍ (Zákazník dává peníze)
    if (waitingForPayment) {
        Rectangle payBtn = { 325, 230, 150, 40 };
        DrawRectangle(250, 210, 300, 80, LIGHTGRAY);
        DrawRectangleLines(250, 210, 300, 80, BLACK);
        DrawText("Zakaznik chce zaplatit!", 285, 215, 12, BLACK);
        
        // Tlačítko schválení platby
        bool hoverPay = CheckCollisionPointRec(GetMousePosition(), payBtn);
        DrawRectangleRec(payBtn, hoverPay ? GREEN : DARKGREEN);
        DrawText("ZAPLACENO (KLIK)", 340, 243, 12, WHITE);
    }
    
    // Vrchní lišta s nápovědou
    DrawText("Levá ruka: WASD + E | Pravá ruka: IJKL + O | Menu/Pauza: Mezerník", 10, 10, 12, DARKGRAY);
}