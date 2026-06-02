#include "scene_game.h"
#include "../game_main.h"
#include "../../managers/InputManager.h"
#include "../../managers/AssetManager.h"
#include "../../managers/CustomerManager.h"
#include "../../classes/Item.h"
#include "../../classes/Customer.h"
#include "../../classes/QTEEvent.h"
#include "../../classes/Player.h"
#include "../../classes/Day.h"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>
#include <memory>

#define QTE_TIME_LIMIT 20.0f

void SpawnCustomerAndItems(std::shared_ptr<Customer>& customerPtr, std::vector<std::shared_ptr<Item>>& belt) {
    customerPtr = CustomerManager::CreateCustomer();
    customerPtr->SayArrivalLine();
    int itemCount = GetRandomValue(1, 20);
    Color fallbackColors[] = { BROWN, WHITE, SKYBLUE, PINK, YELLOW, LIME, RED, PURPLE };
    for (int i = 0; i < itemCount; i++) {
        ItemTemplate recept = CustomerManager::PickItemForCustomer(*customerPtr);        
        Texture2D tex = AssetManager::GetTexture(recept.id);
        Color fColor = fallbackColors[GetRandomValue(0, 7)];
        Vector2 startPos = {-50.0f - (i * 100.0f), 510.0f};
        belt.push_back(std::make_shared<Item>( recept.name, recept.basePrice, recept.clubcardPrice, tex, fColor, startPos, recept.category));
    }
}

static bool HasRestrictedItem(const std::vector<std::shared_ptr<Item>>& belt)
{
    for (const auto& item : belt) {
        if (item->category == RESTRICTED_18) {
            return true;
        }
    }

    return false;
}

static void RebuildReceiptAndTotal(
    const std::vector<std::shared_ptr<Item>>& belt,
    std::vector<std::string>& receiptHistory,
    int& totalSum
)
{
    receiptHistory.clear();
    totalSum = 0;

    for (const auto& item : belt) {
        if (item->isScanned) {
            receiptHistory.push_back(item->name + " ... " + std::to_string(item->basePrice) + " Kc");
            totalSum += item->basePrice;
        }
    }
}


// ==========================================
// 2. HLAVNÍ LOGIKA SCÉNY
// ==========================================
void runGameRecieved(GameState &currentState, InputManager &input, bool &isGamePaused, GameSubState &subState) {
    SetExitKey(KEY_NULL);
    static Rectangle scannerDockRect = { 520.0f, 485.0f, 70.0f, 26.0f };
    static Rectangle handheldScannerRect = { 520.0f, 485.0f, 70.0f, 26.0f };
    static Vector2 scannerCableAnchor = { 515.0f, 500.0f };
    static bool scannerHeldByLeft = false;
    static bool scannerHeldByRight = false;
    static bool scannerFalling = false;
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
    static std::vector<std::string> pendingDiscountLines;
    static int finalDiscountedTotal = 0;
    static bool qteActive = false;
    static QTEEvent activeQTE;
    static float qteTimer = 0.0f;
    static std::string qteResultText = "";
    static float qteResultTimer = 0.0f;
    static bool qteRolledForThisCustomer = false;
    static bool ageRestrictionMistakeGiven = false;
    static float clubcardPromptCenterY = 165.0f;
    static bool shiftClosing = false;
    static bool extraCustomerPending = false;
    static bool extraCustomerSpawned = false;
    static bool clubcardScanned = false;
    static float clubcardScanEffectTimer = 0.0f;
    static Vector2 clubcardScanEffectPos = { 0.0f, 0.0f };

    if (resetGameSignal) {
        initialized = false;
        resetGameSignal = false;
        qteActive = false;
        qteTimer = 0.0f;
        qteResultText = "";
        qteResultTimer = 0.0f;
    }


    if (!initialized) {
        scannerDockRect = { 520.0f, 485.0f, 70.0f, 26.0f };
        handheldScannerRect = scannerDockRect;
        scannerCableAnchor = { 515.0f, 500.0f };
        scannerHeldByLeft = false;
        scannerHeldByRight = false;
        scannerFalling = false;
        clubcardScanned = false;
        leftHand = { Vector2{ 250, 300 }, false, -1, {255, 204, 153, 255}, true };
        rightHand = { Vector2{ 550, 300 }, false, -1, {255, 204, 153, 255}, false };
        shiftTimer = Day::TimeLimit(); 
        Day::ResetShiftStats(currentShift);        
        receiptHistory.clear();  
        totalSum = 0;            
        askedForCard = false;
        beltItems.clear();
        mistakeDisplayTimer = 0.0f;
        mistakeMessage = "";
        showingDiscounts = false;
        discountTimer = 0.0f;
        discountIndex = 0;
        discountLines.clear();
        finalDiscountedTotal = 0;
        pendingDiscountLines.clear();
        qteActive = false;
        qteTimer = 0.0f;
        qteResultText = "";
        qteResultTimer = 0.0f;
        qteRolledForThisCustomer = false;
        ageRestrictionMistakeGiven = false;
        leftHand.isHolding = false;
        leftHand.holdingItemIndex = -1;
        rightHand.isHolding = false;
        rightHand.holdingItemIndex = -1;
        SpawnCustomerAndItems(currentCustomer, beltItems);
        clubcardPromptCenterY = 165.0f + (float)GetRandomValue(-20, 20);
        qteRolledForThisCustomer = false;
        shiftClosing = false;
        extraCustomerPending = false;
        extraCustomerSpawned = false;
        clubcardScanned = false;
        initialized = true;
    }

    if (input.IsPauseTriggered() || input.IsBackTriggered()) { 
        isGamePaused = true;
        currentState = STATE_MENU;
        return;
    }

    if (!isGamePaused) {

        if (!shiftClosing) {
            shiftTimer -= GetFrameTime();

            if (shiftTimer <= 0.0f) {
                shiftTimer = 0.0f;
                shiftClosing = true;
                extraCustomerPending = GetRandomValue(1, 100) <= 25;
                extraCustomerSpawned = false;
            }
        }

        if (mistakeDisplayTimer > 0) mistakeDisplayTimer -= GetFrameTime();

        if (clubcardScanEffectTimer > 0.0f) {
            clubcardScanEffectTimer -= GetFrameTime();
            if (clubcardScanEffectTimer < 0.0f) {
                clubcardScanEffectTimer = 0.0f;
            }
        }

        if (qteResultTimer > 0.0f) {
            qteResultTimer -= GetFrameTime();
        }
 
        if (qteActive && currentCustomer) {
            qteTimer -= GetFrameTime();

            int pressedKey = KEY_NULL;

            if (IsKeyPressed(KEY_R)) pressedKey = KEY_R;
            if (IsKeyPressed(KEY_T)) pressedKey = KEY_T;
            if (IsKeyPressed(KEY_Y)) pressedKey = KEY_Z;
            if (IsKeyPressed(KEY_U)) pressedKey = KEY_U;

            bool answered = pressedKey != KEY_NULL;
            bool timeOut = qteTimer <= 0.0f;

            if (answered || timeOut) {
                bool success = answered && pressedKey == activeQTE.correctKey;
                if (success) {
                    currentCustomer->ChangePatience(activeQTE.patienceSuccess);
                    qteResultText = activeQTE.successReply;
                } else {
                    currentCustomer->ChangePatience(activeQTE.patienceFail);
                    qteResultText = activeQTE.failReply;
                }

                qteResultTimer = 2.0f;
                qteActive = false;
            }
        }


        if (currentShift.wasFired) {
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
            if (scannerHeldByLeft) {
                scannerHeldByLeft = false;
                if (CheckCollisionRecs(handheldScannerRect, scannerDockRect)) {
                    handheldScannerRect = scannerDockRect;
                    scannerFalling = false;
                } else {
                    scannerFalling = true;
                }
            }
            else if (!leftHand.isHolding && !scannerHeldByRight && CheckCollisionPointRec(leftHand.pos, handheldScannerRect)) {
                scannerHeldByLeft = true;
                scannerFalling = false;
            }
            else if (!leftHand.isHolding) {
                for (size_t i = 0; i < beltItems.size(); i++) {
                    if (beltItems[i]->deliveredToCustomer) {
                        continue;
                    }
                    if (CheckCollisionPointRec(leftHand.pos, beltItems[i]->getRect()) && (int)i != rightHand.holdingItemIndex){
                        leftHand.isHolding = true;
                        leftHand.holdingItemIndex = i;
                        break;
                    }
                }
            }
            else {
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
            if (scannerHeldByRight) {
                scannerHeldByRight = false;
                if (CheckCollisionRecs(handheldScannerRect, scannerDockRect)) {
                    handheldScannerRect = scannerDockRect;
                    scannerFalling = false;
                } else {
                    scannerFalling = true;
                }
            }
            else if (!rightHand.isHolding && !scannerHeldByLeft && CheckCollisionPointRec(rightHand.pos, handheldScannerRect)){
                scannerHeldByRight = true;
                scannerFalling = false;
            }
            else if (!rightHand.isHolding) {
                for (size_t i = 0; i < beltItems.size(); i++) {
                    if (beltItems[i]->deliveredToCustomer) {
                        continue;
                    }
                    if (CheckCollisionPointRec(rightHand.pos, beltItems[i]->getRect()) && (int)i != leftHand.holdingItemIndex){
                        rightHand.isHolding = true;
                        rightHand.holdingItemIndex = i;
                        break;
                    }
                }
            }
            else {
                rightHand.isHolding = false;
                rightHand.holdingItemIndex = -1;
            }
        }

        if (leftHand.isHolding) {
            auto& item = beltItems[leftHand.holdingItemIndex];
            item->pos = Vector2{
                leftHand.pos.x - item->drawWidth * 0.5f,
                leftHand.pos.y - item->drawHeight * 0.5f
            };
        }

        if (rightHand.isHolding) {
            auto& item = beltItems[rightHand.holdingItemIndex];
            item->pos = Vector2{
                rightHand.pos.x - item->drawWidth * 0.5f,
                rightHand.pos.y - item->drawHeight * 0.5f
            };
        }

        if (scannerHeldByLeft) {
            handheldScannerRect.x = leftHand.pos.x - handheldScannerRect.width * 0.5f;
            handheldScannerRect.y = leftHand.pos.y - handheldScannerRect.height * 0.5f;
        }
        else if (scannerHeldByRight) {
            handheldScannerRect.x = rightHand.pos.x - handheldScannerRect.width * 0.5f;
            handheldScannerRect.y = rightHand.pos.y - handheldScannerRect.height * 0.5f;
        }
        else if (scannerFalling) {
            handheldScannerRect.y += 6.0f;
            const float scannerRestY = 510.0f;
            if (handheldScannerRect.y >= scannerRestY) {
                handheldScannerRect.y = scannerRestY;
                scannerFalling = false;
            }
        }
        
        Rectangle customerAreaRect = { 500.0f, 500.0f, 300.0f, 100.0f };

        for (size_t i = 0; i < beltItems.size(); i++) {
            auto& item = beltItems[i];
            if (item->deliveredToCustomer) {
                continue;
            }
            bool held = ((int)i == leftHand.holdingItemIndex && leftHand.isHolding) || ((int)i == rightHand.holdingItemIndex && rightHand.isHolding);
            bool inCustomerArea = CheckCollisionRecs(item->getRect(), customerAreaRect);
            if (!held && item->isScanned && inCustomerArea) {
                item->customerAreaTimer += GetFrameTime();
                if (item->customerAreaTimer >= 1.0f) {
                    item->deliveredToCustomer = true;
                    item->pos = { 900.0f, 900.0f };
                }
            } else {
                item->customerAreaTimer = 0.0f;
            }
        }

        if (currentCustomer) {
            currentCustomer->Update();
            currentCustomer->UpdatePatience(GetFrameTime());           
            
            bool beltBlocked = false;
            
            for (const auto& item : beltItems) {
                if (!item->isScanned && item->pos.x >= 200.0f && item->pos.x < 300.0f) {
                    beltBlocked = true;
                }
            }
            
            if (!beltBlocked && currentCustomer->state != PAYING) {
                for (size_t i = 0; i < beltItems.size(); i++) {
                    if (!beltItems[i]->isScanned && (int)i != leftHand.holdingItemIndex && (int)i != rightHand.holdingItemIndex && beltItems[i]->pos.x < 250.0f) {
                        beltItems[i]->pos.x += 1.5f;
                    }
                }
            }
        }

         if (currentCustomer && !qteActive && !qteRolledForThisCustomer && !currentCustomer->qteSet.empty() && currentCustomer->state == WAITING){
            qteRolledForThisCustomer = true;
            if (GetRandomValue(1, 100) <= 40) {
                activeQTE = CustomerManager::PickQTEForCustomer(*currentCustomer);
                activeQTE.timeLimit = QTE_TIME_LIMIT;
                qteTimer = activeQTE.timeLimit;
                qteActive = true;
            }
        }

        if (currentCustomer && currentCustomer->age < 18 && currentCustomer->state == WAITING && HasRestrictedItem(beltItems)){
            Rectangle removeBtn = { 520, 250, 180, 40 };
            if (HandClick(leftHand, removeBtn) || HandClick(rightHand, removeBtn)){
                for (int i = (int)beltItems.size() - 1; i >= 0; i--) {
                    if (beltItems[i]->category == RESTRICTED_18) {
                        beltItems.erase(beltItems.begin() + i);
                    }
                }
                leftHand.isHolding = false;
                leftHand.holdingItemIndex = -1;
                rightHand.isHolding = false;
                rightHand.holdingItemIndex = -1;
                RebuildReceiptAndTotal(beltItems, receiptHistory, totalSum);
                mistakeMessage = "Zakazane zbozi odebrano.";
                mistakeDisplayTimer = 2.0f;
            }
        }

        Rectangle scannerRect = { 350, 540, 100, 20 };
        for (size_t i = 0; i < beltItems.size(); i++) {
            if (!beltItems[i]->isScanned && CheckCollisionRecs(beltItems[i]->getRect(), scannerRect)) {
                beltItems[i]->isScanned = true;
                AssetManager::PlayScanSound();
                std::string line = beltItems[i]->name + " ... " + std::to_string(beltItems[i]->basePrice) + " Kc";
                receiptHistory.push_back(line);
                totalSum += beltItems[i]->basePrice;
                if (beltItems[i]->category == RESTRICTED_18 && currentCustomer->age < 18 && !ageRestrictionMistakeGiven) {
                    ageRestrictionMistakeGiven = true;
                    currentShift.mistakesMade++;
                    mistakeMessage = "Chyba Prodal jsi alkohol nezletilemu";
                    mistakeDisplayTimer = 3.0f;
                    if (currentShift.mistakesMade >= 3) {
                        currentShift.wasFired = true;
                        mistakeMessage = "Mas padaka";
                    }
                }
                currentShift.itemsScanned++;
            }
        }

        for (size_t i = 0; i < beltItems.size(); i++) {
            bool held = ((int)i == leftHand.holdingItemIndex && leftHand.isHolding) || ((int)i == rightHand.holdingItemIndex && rightHand.isHolding);

            if (!held && !beltItems[i]->isScanned && !beltItems[i]->missedPenaltyGiven && beltItems[i]->pos.x > 780) {
                beltItems[i]->missedPenaltyGiven = true;
                beltItems[i]->isScanned = true;
                beltItems[i]->pos.x = 900;
                currentShift.mistakesMade++;
                mistakeMessage = "Chyba Zbozi proslo bez naskenovani";
                mistakeDisplayTimer = 3.0f;
                if (currentShift.mistakesMade >= 3) {
                    currentShift.wasFired = true;
                    mistakeMessage = "Mas padaka";
                }
            }
        }

        bool allDone = true;
        for (const auto& item : beltItems) {
            if (!item->isScanned || !item->deliveredToCustomer) {
                allDone = false;
            }
        }
            
        if (allDone && beltItems.size() > 0 && !leftHand.isHolding && !rightHand.isHolding && currentCustomer->state == WAITING) {
            showingDiscounts = false;
            discountTimer = 0.0f;
            discountIndex = 0;
            discountLines.clear();
            pendingDiscountLines.clear();
            finalDiscountedTotal = totalSum;
            if (clubcardScanned) {
                for (const auto& item : beltItems) {
                    int discount = item->basePrice - item->clubcardPrice;
                    if (discount > 0) {
                        pendingDiscountLines.push_back( item->name + "  -" + std::to_string(discount) + " Kc" );
                        finalDiscountedTotal -= discount;
                    }
                }

                totalSum = finalDiscountedTotal;
                showingDiscounts = !pendingDiscountLines.empty();
            }
            currentCustomer->state = PAYING;
        }

        if (currentCustomer && (currentCustomer->state == WAITING || currentCustomer->state == PAYING)){
            Rectangle askCardBtn = {50.0f, clubcardPromptCenterY - 15.0f, 150.0f,30.0f};
            if (!currentCustomer->gaveClubcard && !currentCustomer->hasCheckedCard && (HandClick(leftHand, askCardBtn) || HandClick(rightHand, askCardBtn))){
                currentCustomer->hasCheckedCard = true;
                if (currentCustomer->hasClubcard) {
                    currentCustomer->gaveClubcard = true;
                    currentCustomer->cardResponse = "Ano, tady ji mam.";
                } else {
                    currentCustomer->cardResponse = "Bohuzel ji nemam.";
                }
            }
            Rectangle cardRect = { 0, 0, 0, 0 };
            bool cardVisible = false;
            if (currentCustomer->gaveClubcard && !clubcardScanned && AssetManager::clubcardTexture.id > 0){
                cardRect = { currentCustomer->pos.x + 90.0f, currentCustomer->pos.y + 80.0f,45.0f,28.0f};
                cardVisible = true;
            }

            if (cardVisible && (scannerHeldByLeft || scannerHeldByRight)){
                Rectangle scanArea = handheldScannerRect;
                scanArea.x -= 12.0f;
                scanArea.y -= 12.0f;
                scanArea.width += 24.0f;
                scanArea.height += 24.0f;
                if (CheckCollisionRecs(scanArea, cardRect)) {
                    clubcardScanned = true;
                    currentCustomer->cardResponse = "Clubcard nactena.";
                    AssetManager::PlayCardSound();
                    clubcardScanEffectTimer = 0.5f;
                    clubcardScanEffectPos = {cardRect.x + cardRect.width * 0.5f,cardRect.y + cardRect.height * 0.5f};
                    if (currentCustomer->state == PAYING) {
                        pendingDiscountLines.clear();
                        discountLines.clear();
                        finalDiscountedTotal = totalSum;
                        for (const auto& item : beltItems) {
                            int discount = item->basePrice - item->clubcardPrice;
                            if (discount > 0) {
                                pendingDiscountLines.push_back(item->name + "  -" + std::to_string(discount) + " Kc");
                                finalDiscountedTotal -= discount;
                            }
                        }
                        totalSum = finalDiscountedTotal;
                        discountTimer = 0.0f;
                        discountIndex = 0;
                        showingDiscounts = !pendingDiscountLines.empty();
                    }
                }
            }
        }

        if (showingDiscounts) {
            discountTimer += GetFrameTime();
            if (discountTimer >= 0.5f) {
                discountTimer = 0.0f;
                if (discountIndex < pendingDiscountLines.size()) {
                    discountLines.push_back(pendingDiscountLines[discountIndex]);
                    discountIndex++;
                } else {
                    showingDiscounts = false;
                }
            }
        }

        if (currentCustomer->state == PAYING) {
            Rectangle payBtn = { 325, 390, 150, 40 };
            if (HandClick(leftHand, payBtn) || HandClick(rightHand, payBtn)) {
                bool clubcardMistake = false;
                if (currentCustomer->hasClubcard && !clubcardScanned) {
                    currentShift.mistakesMade++;
                    clubcardMistake = true;
                    mistakeMessage = "Chyba Nenaskenoval jsi Clubcard";
                    mistakeDisplayTimer = 3.0f;
                    if (currentShift.mistakesMade >= 3) {
                        currentShift.wasFired = true;
                        mistakeMessage = "Mas padaka";
                        mistakeDisplayTimer = 3.0f;
                    }
                }
                currentShift.moneyEarned += totalSum;
                currentShift.customersServed++;
                currentCustomer->SayExitLine();
                currentCustomer->state = WALKING_OUT;
                qteActive = false;
                qteResultText = "";
                qteResultTimer = 0.0f;
                totalSum = 0;
                receiptHistory.clear();
                discountLines.clear();
                showingDiscounts = false;
                discountIndex = 0;
                discountTimer = 0.0f;
                finalDiscountedTotal = 0;
                beltItems.clear();
                pendingDiscountLines.clear();
                if (!clubcardMistake) {
                    mistakeDisplayTimer = 0.0f;
                    mistakeMessage = "";
                }
            }
        }

        if (currentCustomer->state == GONE) {
            if (shiftClosing) {
                if (extraCustomerPending && !extraCustomerSpawned) {
                    extraCustomerPending = false;
                    extraCustomerSpawned = true;
                    SpawnCustomerAndItems(currentCustomer, beltItems);
                    clubcardScanned = false;
                    clubcardScanEffectTimer = 0.0f;
                    clubcardScanEffectPos = { 0.0f, 0.0f };
                    clubcardPromptCenterY = 165.0f + (float)GetRandomValue(-20, 20);
                    ageRestrictionMistakeGiven = false;
                    qteRolledForThisCustomer = false;
                    qteActive = false;
                    qteTimer = 0.0f;
                    qteResultText = "";
                    qteResultTimer = 0.0f;
                    mistakeMessage = "Zakaznik: Stiham to jeste, ze jo?";
                    mistakeDisplayTimer = 3.0f;
                    return;
                }
                subState = SUB_STATS;
                return;
            }

            SpawnCustomerAndItems(currentCustomer, beltItems);
            clubcardScanned = false;
            clubcardScanEffectTimer = 0.0f;
            clubcardScanEffectPos = { 0.0f, 0.0f };
            clubcardPromptCenterY = 165.0f + (float)GetRandomValue(-20, 20);
            ageRestrictionMistakeGiven = false;
            qteRolledForThisCustomer = false;
            qteActive = false;
            qteTimer = 0.0f;
            qteResultText = "";
            qteResultTimer = 0.0f;
        }
    }

    // --- VYKRESLOVÁNÍ ---
    ClearBackground(RAYWHITE); 
    if (currentCustomer) currentCustomer->Draw();

    DrawRectangle(0, 500, 300, 100, DARKGRAY);        
    DrawRectangle(295, 500, 10, 100, RED);            
    DrawRectangle(300, 500, 200, 100, GRAY);          
    DrawRectangle(350, 540, 100, 20, RED);          
    DrawRectangle(500, 500, 300, 100, LIGHTGRAY);     
    
    const int monitorX = 300;
    const int monitorY = 320;
    const int monitorW = 200;
    const int monitorH = 180;

    DrawRectangle(monitorX, monitorY, monitorW, monitorH, BLACK);
    DrawRectangleLines(monitorX, monitorY, monitorW, monitorH, GREEN);
    DrawText("TESCO T-2000", monitorX + 10, monitorY + 5, 10, RED);

    struct MonitorLine {std::string text;Color color; };
    std::vector<MonitorLine> monitorLines;

    for (const auto& line : receiptHistory) {
        monitorLines.push_back({ line, GREEN });
    }

    for (const auto& line : discountLines) {
        monitorLines.push_back({ line, YELLOW });
    }

    const int maxVisibleLines = 7;
    const int lineHeight = 14;
    const int bottomTextY = monitorY + 120;
    int visibleCount = (int)monitorLines.size();

    if (visibleCount > maxVisibleLines) {
        visibleCount = maxVisibleLines;
    }

    int firstIndex = (int)monitorLines.size() - visibleCount;
    int startY = bottomTextY - (visibleCount - 1) * lineHeight - 10;

    for (int i = 0; i < visibleCount; i++) {
        const MonitorLine& line = monitorLines[firstIndex + i];
        DrawText( line.text.c_str(), monitorX + 10, startY + i * lineHeight, 12, line.color);
    }

    DrawText(TextFormat("CELKEM %d Kc", totalSum), monitorX + 10,  monitorY + 155, 14, RED);

    DrawRectangleRec(scannerDockRect, Fade(DARKGRAY, 0.5f));
    DrawRectangleLines((int)scannerDockRect.x,(int)scannerDockRect.y,(int)scannerDockRect.width,(int)scannerDockRect.height,BLACK);
    Vector2 scannerCenter = { handheldScannerRect.x + handheldScannerRect.width * 0.5f, handheldScannerRect.y + handheldScannerRect.height * 0.5f };
    DrawLineEx(scannerCableAnchor, scannerCenter, 4.0f, DARKGRAY);
    DrawCircleV(scannerCableAnchor, 5.0f, BLACK);
    DrawRectangleRounded(handheldScannerRect, 0.25f, 4, DARKGRAY);
    DrawRectangleLines( (int)handheldScannerRect.x, (int)handheldScannerRect.y,  (int)handheldScannerRect.width, (int)handheldScannerRect.height, BLACK );
    DrawRectangle( (int)handheldScannerRect.x + 8, (int)handheldScannerRect.y + 6, 22, 12, RED );
    DrawText("SCANNER", (int)handheldScannerRect.x + 12, (int)handheldScannerRect.y + 14, 12, WHITE);

    if (currentCustomer && currentCustomer->age < 18 && currentCustomer->state == WAITING && HasRestrictedItem(beltItems)){
        DrawRectangle(520, 250, 180, 40, RED);
        DrawText("ODEBRAT ZBOZI", 540, 263, 14, WHITE);
    }

    if (currentCustomer && (currentCustomer->state == WAITING || currentCustomer->state == PAYING) && !currentCustomer->hasCheckedCard && !clubcardScanned){
        Rectangle askCardBtn = { 50.0f, clubcardPromptCenterY - 15.0f, 150.0f, 30.0f };
        DrawRectangleRec(askCardBtn, ORANGE);
        DrawText("Dotaz na Clubcard", (int)askCardBtn.x + 5, (int)askCardBtn.y + 10, 12, BLACK);
    }

    if (currentCustomer && currentCustomer->gaveClubcard && !clubcardScanned && AssetManager::clubcardTexture.id > 0) {
        Rectangle cardDest = { currentCustomer->pos.x + 90.0f, currentCustomer->pos.y + 80.0f, 45.0f, 28.0f};
        DrawTexturePro( AssetManager::clubcardTexture, Rectangle{0.0f,0.0f,(float)AssetManager::clubcardTexture.width,(float)AssetManager::clubcardTexture.height}, cardDest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    if (clubcardScanEffectTimer > 0.0f) {
        float alpha = clubcardScanEffectTimer / 0.5f;
        DrawCircleV(clubcardScanEffectPos,32.0f,Fade(RED, 0.45f * alpha));
        DrawCircleLines((int)clubcardScanEffectPos.x,(int)clubcardScanEffectPos.y,32.0f,RED);
    }

    DrawText(TextFormat("CAS DNE %.1fs", shiftTimer), 20, 20, 20, shiftTimer < 30.0f ? RED : BLACK);
    DrawText(TextFormat("CHYBY %d / 3", currentShift.mistakesMade), 650, 20, 20, currentShift.mistakesMade > 1 ? RED : BLACK);

    if (mistakeDisplayTimer > 0.0f) {
        DrawRectangle(0, 250, 800, 60, Fade(RED, 0.8f));
        DrawText(mistakeMessage.c_str(), 400 - MeasureText(mistakeMessage.c_str(), 20) / 2, 270, 20, WHITE);
    }

    for (const auto& item : beltItems) {
        if (!item->deliveredToCustomer) {
            item->Draw();
        }       
    }

    if (currentCustomer && currentCustomer->state == PAYING) {
        DrawRectangleRec({ 325, 390, 150, 40 }, RED);
        DrawText("ZAPLATIT", 336, 404, 12, WHITE);
    }

    if (currentCustomer && !currentCustomer->cardResponse.empty() && !currentCustomer->gaveClubcard){
        DrawText(currentCustomer->cardResponse.c_str(), currentCustomer->pos.x, currentCustomer->pos.y - 60, 16, MAROON);
    }

    leftHand.Draw();
    rightHand.Draw();

    if (qteActive) {
        DrawRectangle(80, 60, 640, 150, Fade(BLACK, 0.85f));
        DrawRectangleLines(80, 60, 640, 150, YELLOW);
        DrawText(activeQTE.customerLine.c_str(), 100, 80, 18, WHITE);
        std::string rText = activeQTE.optionR.empty() ? "slusna odpoved" : activeQTE.optionR;
        std::string tText = activeQTE.optionT.empty() ? "prakticka odpoved" : activeQTE.optionT;
        std::string zText = activeQTE.optionZ.empty() ? "pravidla / zakon" : activeQTE.optionZ;
        std::string uText = activeQTE.optionU.empty() ? "zrychlit" : activeQTE.optionU;
        DrawText(TextFormat("R) %s", rText.c_str()), 100, 110, 14, RAYWHITE);
        DrawText(TextFormat("T) %s", tText.c_str()), 100, 132, 14, RAYWHITE);
        DrawText(TextFormat("Z) %s", zText.c_str()), 100, 154, 14, RAYWHITE);
        DrawText(TextFormat("U) %s", uText.c_str()), 100, 176, 14, RAYWHITE);
        DrawText(TextFormat("%.1f", qteTimer), 650, 80, 22, RED);
    }

    if (qteResultTimer > 0.0f) {
        DrawRectangle(130, 215, 540, 45, Fade(RAYWHITE, 0.9f));
        DrawRectangleLines(130, 215, 540, 45, DARKGRAY);
        DrawText(qteResultText.c_str(), 145, 230, 16, MAROON);
    }
}