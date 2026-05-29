#include "scene_game.h"
#include "../game_main.h"
#include "../../managers/InputManager.h"
#include "../../managers/AssetManager.h"
#include "../../classes/Customer.h"
#include "../../classes/Player.h"
#include "../../classes/Day.h"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>
#include <memory>



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



void SpawnCustomerAndItems(std::shared_ptr<Customer>& customerPtr, std::vector<std::shared_ptr<Item>>& belt) {
    int randomAge = GetRandomValue(15, 80);
    bool hasCard = GetRandomValue(1, 100) <= 75;
    customerPtr = std::make_shared<Customer>("Zakaznik", randomAge, hasCard);
    customerPtr->pos = Vector2{-100.0f, 200.0f};  
    customerPtr->hasCheckedCard = false;
    customerPtr->cardResponse = "";
    
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
    
    static float shiftTimer = Day::TimeLimit(); 
    static float mistakeDisplayTimer = 0.0f;
    static std::string mistakeMessage = "";
    static bool askedForCard = false;
    static bool showingDiscounts = false;
    static float discountTimer = 0.0f;
    static int discountIndex = 0;

    static std::vector<std::string> discountLines;
    static int finalDiscountedTotal = 0;

    if (resetGameSignal) {
        initialized = false;
        resetGameSignal = false;
    }

    if (!initialized) {
        leftHand = { Vector2{ 250, 300 }, false, -1, {255, 204, 153, 255}, true };
        rightHand = { Vector2{ 550, 300 }, false, -1, {255, 204, 153, 255}, false };
        
        shiftTimer = Day::TimeLimit(); 
        Day::ResetShiftStats(currentShift);        
        
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

        // GRAVITACE ITEMU
        for (size_t i = 0; i < beltItems.size(); i++) {
            bool held = ((int)i == leftHand.holdingItemIndex && leftHand.isHolding) || ((int)i == rightHand.holdingItemIndex && rightHand.isHolding);
            if (!held) {
                auto& item = beltItems[i];
                bool onSurface =
                    item->pos.y >= 510;
                if (!onSurface) {
                    item->pos.y += 6.0f;
                    if (item->pos.y > 510)
                        item->pos.y = 510;
                }
            }
        }

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
            
            if (!beltBlocked && currentCustomer->state != PAYING) {
                for (size_t i = 0; i < beltItems.size(); i++) {
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

        if (currentCustomer && currentCustomer->age < 18){
            Rectangle removeBtn = { 520, 250, 180, 40 };
            if (HandClick(leftHand, removeBtn) ||
                HandClick(rightHand, removeBtn))
            {
                for (int i = (int)beltItems.size() - 1; i >= 0; i--) {
                    auto& item = beltItems[i];
                    if (item->category == RESTRICTED_18) {
                        if (item->isScanned) {
                            totalSum -= item->basePrice;
                            if (i < receiptHistory.size()) {
                                receiptHistory.erase(
                                    receiptHistory.begin() + i
                                );
                            }
                        }
                        beltItems.erase(
                            beltItems.begin() + i
                        );
                    }
                }
                mistakeMessage = "Zakazane zbozi odebrano.";
                mistakeDisplayTimer = 2.0f;
            }
        }

        // --- SKENOVÁNÍ (Skener posunut na Y: 540) ---
        Rectangle scannerRect = { 350, 540, 100, 20 };
        for (size_t i = 0; i < beltItems.size(); i++) {
            if (!beltItems[i]->isScanned && CheckCollisionRecs(beltItems[i]->getRect(), scannerRect)) {
                beltItems[i]->isScanned = true;
                
                std::string line = beltItems[i]->name + " ... " + std::to_string(beltItems[i]->basePrice) + " Kc";
                receiptHistory.push_back(line);
                
                totalSum += beltItems[i]->basePrice;

                if (beltItems[i]->category == RESTRICTED_18 && currentCustomer->age < 18) {
                    currentShift.mistakesMade++;
                    mistakeMessage = "Chyba! Prodal jsi alkohol nezletilemu!";
                    mistakeDisplayTimer = 3.0f;
                }
                currentShift.itemsScanned++;
            }
        }

        for (size_t i = 0; i < beltItems.size(); i++) {
            if (!beltItems[i]->isScanned && beltItems[i]->pos.x > 500) {
                currentShift.mistakesMade++; 
                beltItems[i]->pos.x = -50; 
                if (currentShift.mistakesMade >= 3) {
                    currentShift.wasFired = true;
                }
                mistakeDisplayTimer = 3.0f;
            }
        }

        bool allDone = true;
        for (const auto& item : beltItems) {
            if (!item->isScanned || item->pos.x < 500) allDone = false;
        }
            
         // CLUBCARD SLEVY
        if (allDone && beltItems.size() > 0 && !leftHand.isHolding && !rightHand.isHolding && currentCustomer->state == WAITING){
            if (currentCustomer->gaveClubcard) {
                showingDiscounts = true;
                discountTimer = 0.0f;
                discountIndex = 0;
                discountLines.clear();
                finalDiscountedTotal = totalSum;
            }

            currentCustomer->state = PAYING;
        }

       // --- LOGIKA CLUB CARD ---
        if (currentCustomer->state == WAITING) {
            Rectangle askCardBtn = { 50, 150, 150, 30 }; // Definice zde
            
            if (!currentCustomer->hasCheckedCard && (HandClick(leftHand, askCardBtn) || HandClick(rightHand, askCardBtn))) {
                currentCustomer->hasCheckedCard = true;
                if (currentCustomer->hasClubcard) {
                    currentCustomer->gaveClubcard = true;
                    currentCustomer->cardResponse = "Ano, tady ji mam.";
                } else {
                    currentCustomer->cardResponse = "Bohuzel ji nemam.";
                }
            }
        }

        // POSTUPNE ODECITANI CLUBCARD SLEV
        if (showingDiscounts) {
            discountTimer += GetFrameTime();
            if (discountTimer >= 0.5f) {
                discountTimer = 0.0f;
                if (discountIndex < beltItems.size()) {
                    auto& item = beltItems[discountIndex];
                    int discount = item->basePrice - item->clubcardPrice;

                    if (discount > 0) {
                        discountLines.push_back( item->name + "  -" + std::to_string(discount) + " Kc");
                        finalDiscountedTotal -= discount;
                        totalSum = finalDiscountedTotal;
                    }
                    discountIndex++;
                }else {
                    showingDiscounts = false;
                }
            }
        }

        // 3. LOGIKA PLATBY (Zaplacení rukou)
        if (currentCustomer->state == PAYING) {
            Rectangle payBtn = { 325, 230, 150, 40 };
            if (HandClick(leftHand, payBtn) || HandClick(rightHand, payBtn)) {
                
                // Kontrola: Pokud měl kartu a nezeptal ses, je to chyba
                if (currentCustomer->hasClubcard && !currentCustomer->hasCheckedCard) {
                    currentShift.mistakesMade++;
                    mistakeMessage = "Chyba! Nezeptal jsi se na Clubcard!";
                    mistakeDisplayTimer = 3.0f;
                    if (currentShift.mistakesMade >= 3) currentShift.wasFired = true;
                }

                currentCustomer->state = WALKING_OUT;
                totalSum = 0;
                receiptHistory.clear();
                discountLines.clear(); 
                showingDiscounts = false;
                discountIndex = 0;
                discountTimer = 0.0f;
                beltItems.clear(); 
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
    DrawRectangle(300, 250, 200, 180, BLACK); // Zvětšeno na výšku 180 pro více textu
    DrawRectangleLines(300, 250, 200, 180, GREEN);
    DrawText("TESCO T-2000", 310, 255, 10, GREEN);
    
    // Výpis položek
    for (size_t i = 0; i < receiptHistory.size(); i++) {
        DrawText(receiptHistory[i].c_str(), 310, 275 + (i * 15), 12, GREEN);
    }

    // Celková suma na spodku monitoru
    DrawText(TextFormat("CELKEM: %d Kc", totalSum), 310, 400, 14, RED);

    // Tlačítko odebrání alkoholu
    if (currentCustomer &&
        currentCustomer->age < 18)
    {
        DrawRectangle(520, 250, 180, 40, RED);
        DrawText("ODEBRAT ZBOZI", 540, 263, 14, WHITE);
    }

    for (size_t i = 0; i < discountLines.size(); i++) {

    DrawText(
        discountLines[i].c_str(),
        310,
        340 + (i * 15),
        12,
        YELLOW
    );
}

    if (currentCustomer && currentCustomer->state == WAITING) {
        DrawRectangleRec({ 50, 150, 150, 30 }, currentCustomer->hasCheckedCard ? GRAY : ORANGE);
        DrawText("Dotaz na Clubcard", 55, 160, 12, BLACK);
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
        DrawRectangleRec({ 325, 230, 150, 40 }, GREEN);
        DrawText("VZIT PENIZE", 335, 243, 12, WHITE);
    }

    if (currentCustomer && !currentCustomer->cardResponse.empty()) {
        DrawText(currentCustomer->cardResponse.c_str(), currentCustomer->pos.x, currentCustomer->pos.y - 60, 16, MAROON);
    }

    // 3. Ruce hrají přes všechno ostatní
    leftHand.Draw();
    rightHand.Draw();
}