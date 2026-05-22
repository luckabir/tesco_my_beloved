#include "scene_game.h"
#include "../game_main.h"
#include "../../managers/InputManager.h"
#include "../../managers/AssetManager.h"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

#define TIME_LIMIT 40.0f

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
    int holdingItemIndex; 
    Color color;
};


void runGameRecieved(GameState &currentState, InputManager &input, bool &isGamePaused, GameSubState &subState) {
    SetExitKey(KEY_NULL);

    static bool initialized = false;
    static Hand leftHand;
    static Hand rightHand;
    static std::vector<Item> currentCart;
    static int currentItemIndex = 0;
    static std::vector<std::string> receiptHistory;
    static int totalSum = 0;
    static bool waitingForPayment = false;

    static float shiftTimer = TIME_LIMIT; 

    if (resetGameSignal) {
        initialized = false;

        currentItemIndex = 0;
        receiptHistory.clear();  
        totalSum = 0;            
        waitingForPayment = false;
        shiftTimer = TIME_LIMIT; 

        currentShift.moneyEarned = 0;
        currentShift.itemsScanned = 0;
        currentShift.mistakesMade = 0;
        currentShift.wasFired = false;

        resetGameSignal = false;
    }

    if (!initialized) {
        leftHand = { Vector2{ 250, 300 }, false, -1, GREEN };
        rightHand = { Vector2{ 550, 300 }, false, -1, ORANGE };
        
        currentCart = {
            { "Rohlik", 3, BROWN, { -50, 410 }, true, false },
            { "Tesco Mleko", 25, WHITE, { -150, 410 }, true, false },
            { "Vodka 40%", 179, SKYBLUE, { -250, 410 }, true, false },
            { "Parek", 49, PINK, { -350, 410 }, true, false }
        };
        
        currentItemIndex = 0;
        receiptHistory.clear();  
        totalSum = 0;            
        waitingForPayment = false;
        shiftTimer = TIME_LIMIT; 
        initialized = true;
    }

    // Odchod do pauzy
    if (input.IsPauseTriggered() || input.IsBackTriggered()) { 
        isGamePaused = true;
        currentState = STATE_MENU;
        return;
    }

    if (!isGamePaused) {
        // Aktualizace herního času
        shiftTimer -= GetFrameTime();

        // KONTROLA KONCE SMĚNY (Čas vypršel nebo příliš mnoho chyb)
        if (shiftTimer <= 0.0f || currentShift.mistakesMade >= 3) {
            if (currentShift.mistakesMade >= 3) {
                currentShift.wasFired = true; // Vyhazov za neschopnost
            }
            subState = SUB_STATS; // Přepnutí na statistiky dne
            return;
        }

        // --- ZDE JE TVÁ PŮVODNÍ MECHANIKA POHYBU A SKENOVÁNÍ ---
        float handSpeed = 5.0f;
        if (IsKeyDown(KEY_W)) leftHand.pos.y -= handSpeed;
        if (IsKeyDown(KEY_S)) leftHand.pos.y += handSpeed;
        if (IsKeyDown(KEY_A)) leftHand.pos.x -= handSpeed;
        if (IsKeyDown(KEY_D)) leftHand.pos.x += handSpeed;
        leftHand.pos = Vector2Clamp(leftHand.pos, Vector2{0,0}, Vector2{800,600});

        if (IsKeyPressed(KEY_E)) {
            if (!leftHand.isHolding) {
                for (size_t i = 0; i < currentCart.size(); i++) {
                    if (CheckCollisionPointRec(leftHand.pos, currentCart[i].getRect()) && (int)i != rightHand.holdingItemIndex) {
                        leftHand.isHolding = true;
                        leftHand.holdingItemIndex = i;
                        break;
                    }
                }
            } else {
                leftHand.isHolding = false;
                leftHand.holdingItemIndex = -1;
            }
        }

        if (IsKeyDown(KEY_I)) rightHand.pos.y -= handSpeed;
        if (IsKeyDown(KEY_K)) rightHand.pos.y += handSpeed;
        if (IsKeyDown(KEY_J)) rightHand.pos.x -= handSpeed;
        if (IsKeyDown(KEY_L)) rightHand.pos.x += handSpeed;
        rightHand.pos = Vector2Clamp(rightHand.pos, Vector2{0,0}, Vector2{800,600});

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

        if (leftHand.isHolding) currentCart[leftHand.holdingItemIndex].pos = Vector2{ leftHand.pos.x - 25, leftHand.pos.y - 20 };
        if (rightHand.isHolding) currentCart[rightHand.holdingItemIndex].pos = Vector2{ rightHand.pos.x - 25, rightHand.pos.y - 20 };

        bool beltBlocked = false;
        for (const auto& item : currentCart) {
            if (!leftHand.isHolding && !rightHand.isHolding && item.pos.x >= 240 && item.pos.x <= 250 && !item.isScanned) {
                beltBlocked = true;
            }
        }

        if (!beltBlocked && !waitingForPayment) {
            for (size_t i = 0; i < currentCart.size(); i++) {
                if ((int)i != leftHand.holdingItemIndex && (int)i != rightHand.holdingItemIndex && currentCart[i].pos.x < 250) {
                    currentCart[i].pos.x += 1.5f;
                }
            }
        }

        // Detekce skeneru
        Rectangle scannerRect = { 350, 440, 100, 20 };
        for (size_t i = 0; i < currentCart.size(); i++) {
            if (!currentCart[i].isScanned && CheckCollisionRecs(currentCart[i].getRect(), scannerRect)) {
                currentCart[i].isScanned = true;
                
                // ZÁPIS DO STATISTIK:
                currentShift.itemsScanned++;
                currentShift.moneyEarned += currentCart[i].price;

                std::string entry = currentCart[i].name + " ... " + std::to_string(currentCart[i].price) + " Kc";
                receiptHistory.push_back(entry);
                if (receiptHistory.size() > 5) receiptHistory.erase(receiptHistory.begin());
                totalSum += currentCart[i].price;
            }
        }

        // Příklad vyvolání chyby: Hráč hodil neskenovaný produkt rovnou na odkladač vpravo
        for (size_t i = 0; i < currentCart.size(); i++) {
            if (!currentCart[i].isScanned && currentCart[i].pos.x > 500) {
                currentShift.mistakesMade++; // CHYBA! Hodila zboží bez skenování
                currentCart[i].pos.x = -50;  // Vrať ho zpátky na pás za trest
            }
        }

        bool allDone = true;
        for (const auto& item : currentCart) {
            if (!item.isScanned || item.pos.x < 500) allDone = false;
        }
        
        if (allDone && currentCart.size() > 0 && !leftHand.isHolding && !rightHand.isHolding) {
            waitingForPayment = true;
        }

        if (waitingForPayment && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mPos = GetMousePosition(); 
            float scale = fminf((float)GetScreenWidth() / 800.0f, (float)GetScreenHeight() / 600.0f);
            mPos.x = (mPos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
            mPos.y = (mPos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;
            Rectangle payBtn = { 325, 230, 150, 40 };
            if (CheckCollisionPointRec(mPos, payBtn)) {
                waitingForPayment = false;
                totalSum = 0;
                receiptHistory.clear();
                leftHand.isHolding = false;
                leftHand.holdingItemIndex = -1;
                rightHand.isHolding = false;
                rightHand.holdingItemIndex = -1;
                currentCart = {
                    { "Chleb", 42, BROWN, { -50, 410 }, true, false },
                    { "Tesco Chips", 39, RED, { -150, 410 }, true, false },
                    { "Banany", 35, YELLOW, { -250, 410 }, true, false }
                };
                return ;
            }
        }
    }

    // --- VYKRESLOVÁNÍ (Tvoje grafika kasy) ---
    ClearBackground(RAYWHITE); 

    DrawRectangle(0, 450, 300, 40, DARKGRAY);        
    DrawRectangle(295, 450, 10, 40, RED);            
    DrawRectangle(300, 450, 200, 50, GRAY);          
    DrawRectangle(350, 450, 100, 10, RED);           
    DrawRectangle(500, 450, 300, 50, LIGHTGRAY);     

    DrawText("PAS (ZBOZI)", 20, 470, 10, WHITE);
    DrawText("ODKLADAC", 720, 470, 10, DARKGRAY);

    DrawRectangle(300, 50, 200, 150, BLACK);
    DrawRectangleLines(300, 50, 200, 150, GREEN);
    DrawText("DISPLEJ TESCO T-2000", 310, 55, 10, GREEN);
    
    for (size_t i = 0; i < receiptHistory.size(); i++) {
        DrawText(receiptHistory[i].c_str(), 310, 75 + (i * 15), 12, GREEN);
    }

    std::string sumStr = "CELKEM: " + std::to_string(totalSum) + " Kc";
    DrawText(sumStr.c_str(), 310, 175, 14, RED);

    // Vykreslení zbývajícího času a chyb směny v horní liště
    DrawText(TextFormat("CAS SMENY: %.1fs", shiftTimer), 20, 40, 16, shiftTimer < 15.0f ? RED : BLACK);
    DrawText(TextFormat("CHYBY: %d / 3", currentShift.mistakesMade), 20, 65, 16, currentShift.mistakesMade > 1 ? RED : BLACK);

    for (const auto& item : currentCart) {
        DrawRectangleRec(item.getRect(), item.color);
        DrawRectangleLines((int)item.pos.x, (int)item.pos.y, 50, 40, BLACK);
        DrawText(item.name.c_str(), item.pos.x + 5, item.pos.y + 12, 10, item.color.r > 200 && item.color.g > 200 ? BLACK : WHITE);        
        if (item.isScanned) DrawText("[OK]", item.pos.x + 12, item.pos.y - 12, 10, GREEN);
    }

    DrawCircleV(leftHand.pos, 20, leftHand.color);
    DrawCircleLines(leftHand.pos.x, leftHand.pos.y, 20, BLACK);
    DrawText("L (E)", leftHand.pos.x - 12, leftHand.pos.y - 5, 12, WHITE);
    if (leftHand.isHolding) DrawCircle(leftHand.pos.x, leftHand.pos.y, 6, RED);

    DrawCircleV(rightHand.pos, 20, rightHand.color);
    DrawCircleLines(rightHand.pos.x, rightHand.pos.y, 20, BLACK);
    DrawText("P (O)", rightHand.pos.x - 12, rightHand.pos.y - 5, 12, WHITE);
    if (rightHand.isHolding) DrawCircle(rightHand.pos.x, rightHand.pos.y, 6, RED);

    if (waitingForPayment) {
        Rectangle payBtn = { 325, 230, 150, 40 };
        DrawRectangle(250, 210, 300, 80, LIGHTGRAY);
        DrawRectangleLines(250, 210, 300, 80, BLACK);
        DrawText("Zakaznik chce zaplatit!", 285, 215, 12, BLACK);
        
        bool hoverPay = CheckCollisionPointRec(GetMousePosition(), payBtn);
        DrawRectangleRec(payBtn, hoverPay ? GREEN : DARKGREEN);
        DrawText("ZAPLACENO (KLIK)", 340, 243, 12, WHITE);
    }
}