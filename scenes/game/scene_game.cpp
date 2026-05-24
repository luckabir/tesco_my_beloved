#include "scene_game.h"
#include "../game_main.h"
#include "../../managers/InputManager.h"
#include "../../managers/AssetManager.h"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>
#include <memory>

#define DAY_TIME_LIMIT 120.0f // 2 minuty na den

// ==========================================
// 1. OBJEKTOVÁ ARCHITEKTURA (Třídy a Databáze)
// ==========================================


class Item {
public:
    std::string name;
    int basePrice;
    int clubcardPrice;
    Texture2D texture; 
    Color fallbackColor; 
    Vector2 pos;
    bool isScanned;
    ItemCategory category;

    Item(std::string n, int p, int cp, Texture2D tex, Color fallback, Vector2 startPos, ItemCategory cat = NORMAL) 
        : name(n), basePrice(p), clubcardPrice(cp), texture(tex), fallbackColor(fallback), pos(startPos), isScanned(false), category(cat) {}
    
    virtual ~Item() = default;

    Rectangle getRect() const { return Rectangle{ pos.x, pos.y, 120, 80 }; }
    
    void Draw() {
        if (texture.id > 0) {
            Rectangle dest = { pos.x, pos.y, 120, 80 };
            Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
            Vector2 origin = { 0, 0 };
        DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
        } else {
            DrawRectangleRec(getRect(), fallbackColor);
            DrawRectangleLines((int)pos.x, (int)pos.y, 120, 80, BLACK);
            DrawText(name.c_str(), pos.x + 5, pos.y + 12, 10, fallbackColor.r > 200 && fallbackColor.g > 200 ? BLACK : WHITE);        
        }
        
        if (isScanned) DrawText("[OK]", pos.x + 12, pos.y - 12, 10, GREEN);
    }

    virtual bool requiresSpecialAction() { return false; } 
};

// Zákazník
enum CustomerState { WALKING_IN, WAITING, PAYING, WALKING_OUT, GONE };

class Customer {
public:
    std::string name;
    int age;
    bool hasClubcard;
    bool gaveClubcard; 
    
    Vector2 pos;
    CustomerState state;

    Customer(std::string n, int a, bool hasCard) 
        : name(n), age(a), hasClubcard(hasCard), gaveClubcard(false), state(WALKING_IN) {
        pos = { -150, 150 }; 
        
        if (hasClubcard && GetRandomValue(1, 100) > 70) {
            gaveClubcard = true;
        }
    }

    void Update() {
        if (state == WALKING_IN) {
            pos.x += 3.0f;
            if (pos.x >= 350) state = WAITING;
        }
        else if (state == WALKING_OUT) {
            pos.x += 4.0f;
            if (pos.x > 900) state = GONE;
        }
    }
    
    void Draw() {
        DrawRectangle(pos.x, pos.y, 100, 350, DARKBLUE); // Tělo (protáhl jsem ho níž)
        DrawCircle(pos.x + 50, pos.y - 30, 40, BEIGE);   // Hlava
        DrawText(TextFormat("Vek: %d", age), pos.x + 10, pos.y + 20, 16, WHITE);
        
        if (gaveClubcard) {
            DrawRectangle(pos.x + 70, pos.y + 60, 30, 20, ORANGE); 
            DrawText("KARTA", pos.x + 72, pos.y + 65, 8, BLACK);
        }
    }
};

struct Hand {
    Vector2 pos;
    bool isHolding;
    int holdingItemIndex; 
    Color skinColor;
    bool isLeft;

    void Draw() {
        Vector2 elbow = isLeft ? Vector2{ 100, 700 } : Vector2{ 700, 700 };
        DrawLineEx(elbow, pos, 35.0f, skinColor); 
        DrawCircleV(pos, 25, skinColor); 
        
        DrawCircle(pos.x, pos.y - 25, 8, skinColor);
        DrawCircle(pos.x - 15, pos.y - 15, 8, skinColor);
        DrawCircle(pos.x + 15, pos.y - 15, 8, skinColor);

        DrawText(isLeft ? "L (E)" : "P (O)", pos.x - 12, pos.y - 5, 12, BLACK);
        if (isHolding) DrawCircle(pos.x, pos.y, 10, RED); 
    }
};

// Pomocná funkce pro vygenerování nákupu zákazníka (abychom nepsali kód dvakrát)
void SpawnCustomerAndItems(std::shared_ptr<Customer>& customerPtr, std::vector<std::shared_ptr<Item>>& belt) {
    int randomAge = GetRandomValue(15, 80);
    bool hasCard = GetRandomValue(0, 1) == 1; // 50% šance na Clubcard
    customerPtr = std::make_shared<Customer>("Zakaznik", randomAge, hasCard);
    
    int itemCount = GetRandomValue(2, 4);
    
    Color fallbackColors[] = { BROWN, WHITE, SKYBLUE, PINK, YELLOW, LIME, RED, PURPLE };

    for (int i = 0; i < itemCount; i++) {
        ItemTemplate recept = AssetManager::GetRandomItemTemplate();
        Texture2D tex = AssetManager::GetTexture(recept.id);
        
        Color fColor = fallbackColors[GetRandomValue(0, 7)];
        
        Vector2 startPos = {-50.0f - (i * 100.0f), 510.0f};

        belt.push_back(std::make_shared<Item>(
            recept.name,
            recept.basePrice,
            recept.clubcardPrice,
            tex,
            fColor,
            startPos,
            recept.category
        ));
    }
}

// ==========================================
// 2. HLAVNÍ LOGIKA SCÉNY
// ==========================================
void runGameRecieved(GameState &currentState, InputManager &input, bool &isGamePaused, GameSubState &subState) {
    SetExitKey(KEY_NULL);

    static bool initialized = false;
    static Hand leftHand, rightHand;
    
    static std::shared_ptr<Customer> currentCustomer = nullptr;
    static std::vector<std::shared_ptr<Item>> beltItems;
    static std::vector<std::string> receiptHistory;
    static int totalSum = 0;
    
    static float shiftTimer = DAY_TIME_LIMIT; 
    static float mistakeDisplayTimer = 0.0f;
    static std::string mistakeMessage = "";
    static bool askedForCard = false;

    if (resetGameSignal) {
        initialized = false;
        resetGameSignal = false;
    }

    if (!initialized) {
        leftHand = { Vector2{ 250, 300 }, false, -1, {255, 204, 153, 255}, true };
        rightHand = { Vector2{ 550, 300 }, false, -1, {255, 204, 153, 255}, false };
        
        shiftTimer = DAY_TIME_LIMIT; 
        currentShift = {0, 0, 0, false}; 
        
        receiptHistory.clear();  
        totalSum = 0;            
        askedForCard = false;
        beltItems.clear();

        // --- SPAWN PRVNÍHO ZÁKAZNÍKA PŘES ASSET MANAGER ---
        SpawnCustomerAndItems(currentCustomer, beltItems);

        initialized = true;
    }

    if (input.IsPauseTriggered() || input.IsBackTriggered()) { 
        isGamePaused = true;
        currentState = STATE_MENU;
        return;
    }

    if (!isGamePaused) {
        shiftTimer -= GetFrameTime();
        if (mistakeDisplayTimer > 0) mistakeDisplayTimer -= GetFrameTime();

        if (shiftTimer <= 0.0f || currentShift.wasFired) {
            subState = SUB_STATS; 
            return;
        }

        // --- POHYB RUKOU ---
        float handSpeed = 6.0f;
        if (IsKeyDown(KEY_W)) leftHand.pos.y -= handSpeed;
        if (IsKeyDown(KEY_S)) leftHand.pos.y += handSpeed;
        if (IsKeyDown(KEY_A)) leftHand.pos.x -= handSpeed;
        if (IsKeyDown(KEY_D)) leftHand.pos.x += handSpeed;
        leftHand.pos = Vector2Clamp(leftHand.pos, Vector2{0,0}, Vector2{800,600});

        if (IsKeyPressed(KEY_E)) {
            if (!leftHand.isHolding) {
                for (size_t i = 0; i < beltItems.size(); i++) {
                    if (CheckCollisionPointRec(leftHand.pos, beltItems[i]->getRect()) && (int)i != rightHand.holdingItemIndex) {
                        leftHand.isHolding = true; leftHand.holdingItemIndex = i; break;
                    }
                }
            } else { leftHand.isHolding = false; leftHand.holdingItemIndex = -1; }
        }

        if (IsKeyDown(KEY_I)) rightHand.pos.y -= handSpeed;
        if (IsKeyDown(KEY_K)) rightHand.pos.y += handSpeed;
        if (IsKeyDown(KEY_J)) rightHand.pos.x -= handSpeed;
        if (IsKeyDown(KEY_L)) rightHand.pos.x += handSpeed;
        rightHand.pos = Vector2Clamp(rightHand.pos, Vector2{0,0}, Vector2{800,600});

        if (IsKeyPressed(KEY_O)) {
            if (!rightHand.isHolding) {
                for (size_t i = 0; i < beltItems.size(); i++) {
                    if (CheckCollisionPointRec(rightHand.pos, beltItems[i]->getRect()) && (int)i != leftHand.holdingItemIndex) {
                        rightHand.isHolding = true; rightHand.holdingItemIndex = i; break;
                    }
                }
            } else { rightHand.isHolding = false; rightHand.holdingItemIndex = -1; }
        }

        if (leftHand.isHolding) beltItems[leftHand.holdingItemIndex]->pos = Vector2{ leftHand.pos.x - 25, leftHand.pos.y - 20 };
        if (rightHand.isHolding) beltItems[rightHand.holdingItemIndex]->pos = Vector2{ rightHand.pos.x - 25, rightHand.pos.y - 20 };

        // --- LOGIKA ZÁKAZNÍKA A PÁSU ---
        if (currentCustomer) {
            currentCustomer->Update();
            
            // 1. Zjistíme, jestli je pás blokovaný
            bool beltBlocked = false;
            
            // Pás zastavíme, pokud jakýkoliv NENASKENOVANÝ item 
            // dosáhl "konce pásu" (např. pozice X = 250)
            for (const auto& item : beltItems) {
                if (!item->isScanned && item->pos.x >= 200.0f && item->pos.x < 300.0f) {
                    beltBlocked = true;
                }
            }
            
            // 2. Pokud není blokováno a zákazník čeká, posuneme itemy, 
            // které ještě nedojely na konec
            if (!beltBlocked && currentCustomer->state != PAYING) {
                for (size_t i = 0; i < beltItems.size(); i++) {
                    // Posouváme jen ty, které jsou za koncem pásu a nejsou drženy v ruce
                    if (!beltItems[i]->isScanned && 
                        (int)i != leftHand.holdingItemIndex && 
                        (int)i != rightHand.holdingItemIndex && 
                        beltItems[i]->pos.x < 250.0f) 
                    {
                        beltItems[i]->pos.x += 1.5f;
                    }
                }
            }
        }

        // --- SKENOVÁNÍ (Skener posunut na Y: 540) ---
        Rectangle scannerRect = { 350, 540, 100, 20 };
        for (size_t i = 0; i < beltItems.size(); i++) {
            if (!beltItems[i]->isScanned && CheckCollisionRecs(beltItems[i]->getRect(), scannerRect)) {
                beltItems[i]->isScanned = true;
                
                if (beltItems[i]->category == RESTRICTED_18 && currentCustomer->age < 18) {
                    currentShift.mistakesMade++;
                    mistakeMessage = "Chyba! Prodal jsi alkohol nezletilemu!";
                    mistakeDisplayTimer = 3.0f;
                }

                int finalPrice = (currentCustomer->gaveClubcard) ? beltItems[i]->clubcardPrice : beltItems[i]->basePrice;
                currentShift.itemsScanned++;
                currentShift.moneyEarned += finalPrice;
                totalSum += finalPrice;

                receiptHistory.push_back(beltItems[i]->name + " ... " + std::to_string(finalPrice) + " Kc");
                if (receiptHistory.size() > 5) receiptHistory.erase(receiptHistory.begin());
            }
        }

        // CHYBA: Zboží hozené na odkladač bez naskenování
        for (size_t i = 0; i < beltItems.size(); i++) {
            if (!beltItems[i]->isScanned && beltItems[i]->pos.x > 500) {
                currentShift.mistakesMade++; 
                beltItems[i]->pos.x = -50; 
                
                if (currentShift.mistakesMade == 1) mistakeMessage = "Dobre, jsi tu novacek a lidi delaji chyby.";
                else if (currentShift.mistakesMade == 2) mistakeMessage = "Snad se to uz nebude opakovat.";
                else if (currentShift.mistakesMade >= 3) {
                    mistakeMessage = "Trikrat a dost! VEN!";
                    currentShift.wasFired = true;
                }
                mistakeDisplayTimer = 3.0f;
            }
        }

        bool allDone = true;
        for (const auto& item : beltItems) {
            if (!item->isScanned || item->pos.x < 500) allDone = false;
        }
        
        if (allDone && beltItems.size() > 0 && !leftHand.isHolding && !rightHand.isHolding && currentCustomer->state == WAITING) {
            currentCustomer->state = PAYING;
        }

        Vector2 mPos = GetMousePosition();
        float scale = fminf((float)GetScreenWidth() / 800.0f, (float)GetScreenHeight() / 600.0f);
        mPos.x = (mPos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
        mPos.y = (mPos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;
        
        Rectangle askCardBtn = { 50, 150, 150, 30 };
        if (CheckCollisionPointRec(mPos, askCardBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && currentCustomer->state == WAITING) {
            askedForCard = true;
            if (currentCustomer->hasClubcard) currentCustomer->gaveClubcard = true;
        }

        if (currentCustomer->state == PAYING && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Rectangle payBtn = { 325, 230, 150, 40 };
            if (CheckCollisionPointRec(mPos, payBtn)) {
                
                if (currentCustomer->hasClubcard && !askedForCard && !currentCustomer->gaveClubcard) {
                    currentShift.mistakesMade++;
                    mistakeMessage = "Chyba! Nezeptal jsi se na Clubcard!";
                    mistakeDisplayTimer = 3.0f;
                    if (currentShift.mistakesMade >= 3) currentShift.wasFired = true;
                }

                currentCustomer->state = WALKING_OUT;
                totalSum = 0;
                receiptHistory.clear();
                beltItems.clear(); 
                askedForCard = false;
            }
        }

        // --- SPAWN DALŠÍHO ZÁKAZNÍKA ---
        if (currentCustomer->state == GONE) {
            SpawnCustomerAndItems(currentCustomer, beltItems);
        }
    }

    // --- VYKRESLOVÁNÍ ---
    ClearBackground(RAYWHITE); 

    // 1. Zákazník je vzadu za pultem
    if (currentCustomer) currentCustomer->Draw();

    // 2. Kasa a Pás 
    DrawRectangle(0, 500, 300, 100, DARKGRAY);        
    DrawRectangle(295, 500, 10, 100, RED);            
    DrawRectangle(300, 500, 200, 100, GRAY);          
    DrawRectangle(350, 540, 100, 20, RED); // Skener           
    DrawRectangle(500, 500, 300, 100, LIGHTGRAY);     
    
    DrawText("PAS", 20, 510, 10, WHITE);
    DrawText("ODKLADAC", 720, 510, 10, DARKGRAY);

    // Monitor kasy
    DrawRectangle(300, 50, 200, 150, BLACK);
    DrawRectangleLines(300, 50, 200, 150, GREEN);
    DrawText("TESCO T-2000", 310, 55, 10, GREEN);
    
    for (size_t i = 0; i < receiptHistory.size(); i++) {
        DrawText(receiptHistory[i].c_str(), 310, 75 + (i * 15), 12, GREEN);
    }
    DrawText(TextFormat("CELKEM: %d Kc", totalSum), 310, 175, 14, RED);

    if (currentCustomer && currentCustomer->state == WAITING) {
        Rectangle askCardBtn = { 50, 150, 150, 30 };
        DrawRectangleRec(askCardBtn, ORANGE);
        DrawText("Dotaz na Clubcard", 55, 160, 12, BLACK);
        if (currentCustomer->gaveClubcard) {
            DrawText("KARTA NACTENA", 50, 190, 14, GREEN);
        }
    }

    DrawText(TextFormat("CAS DNE: %.1fs", shiftTimer), 20, 20, 20, shiftTimer < 30.0f ? RED : BLACK);
    DrawText(TextFormat("CHYBY: %d / 3", currentShift.mistakesMade), 650, 20, 20, currentShift.mistakesMade > 1 ? RED : BLACK);

    if (mistakeDisplayTimer > 0.0f) {
        DrawRectangle(0, 250, 800, 60, Fade(RED, 0.8f));
        DrawText(mistakeMessage.c_str(), 400 - MeasureText(mistakeMessage.c_str(), 20) / 2, 270, 20, WHITE);
    }

    // Vykreslení zboží pomocí vestavěné metody Draw (S fallbackem na barvu)
    for (const auto& item : beltItems) {
        item->Draw();
    }

    if (currentCustomer && currentCustomer->state == PAYING) {
        Rectangle payBtn = { 325, 230, 150, 40 };
        DrawRectangle(250, 210, 300, 80, LIGHTGRAY);
        DrawRectangleLines(250, 210, 300, 80, BLACK);
        DrawText("Zakaznik plati...", 285, 215, 12, BLACK);
        DrawRectangleRec(payBtn, GREEN);
        DrawText("VZIT PENIZE (KLIK)", 335, 243, 12, WHITE);
    }

    // 3. Ruce hrají přes všechno ostatní
    leftHand.Draw();
    rightHand.Draw();
}