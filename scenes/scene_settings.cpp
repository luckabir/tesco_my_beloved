#include "scene_settings.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"
#include "../structures/Settings.h"
#include "raylib.h"
#include "rlgl.h" 
#include <cmath>

#define PL_MPEG_IMPLEMENTATION
#include "../pl_mpeg.h"

bool videoPlaying = false;
static const char* yuvShaderSource =
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D yTexture;\n"
    "uniform sampler2D uTexture;\n"
    "uniform sampler2D vTexture;\n"
    "void main() {\n"
    "    float y = texture(yTexture, fragTexCoord).r;\n"
    "    float u = texture(uTexture, fragTexCoord).r - 0.5;\n"
    "    float v = texture(vTexture, fragTexCoord).r - 0.5;\n"
    "    float r = y + 1.402 * v;\n"
    "    float g = y - 0.344136 * u - 0.714136 * v;\n"
    "    float b = y + 1.772 * u;\n"
    "    finalColor = vec4(r, g, b, 1.0);\n"
    "}\n";

void runSettings(GameState& currentState, InputManager& input)
{
    static bool isInitialized = false;
    static float tempVolume = 0.0f;
    static bool tempFullscreen = false;
    static bool isRealistic = false; 

    static float backupVolume = 0.0f;
    static bool backupFullscreen = false;

    if (!isInitialized) {
        tempVolume = gameSettings.volume;
        tempFullscreen = gameSettings.fullscreen;
        
        backupVolume = gameSettings.volume;
        backupFullscreen = gameSettings.fullscreen;
        
        isInitialized = true;
    }

    Rectangle screenModeButton = { 250, 160, 300, 45 };
    Rectangle volumeSliderBar  = { 250, 260, 300, 15 };
    
    Vector2 radioClassicCenter = { 270, 350 };
    Vector2 radioRealisticCenter = { 270, 390 };
    float radioRadius = 10.0f;

    Rectangle saveButton       = { 250, 450, 300, 45 };
    Rectangle backButton       = { 250, 510, 300, 45 };

    static plm_t* plm = nullptr;
    static Texture2D yTexture = { 0 };
    static Texture2D uTexture = { 0 };
    static Texture2D vTexture = { 0 };
    static Shader yuvShader = { 0 };
    static float timeAccumulator = 0.0f;

    Vector2 mousePos = GetMousePosition();

    float scale = fminf((float)GetScreenWidth() / 800.0f, (float)GetScreenHeight() / 600.0f);
    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    if (videoPlaying) {
        ClearBackground(BLACK);

        timeAccumulator += GetFrameTime();
        double frameDuration = 1.0 / plm_get_framerate(plm);

        while (timeAccumulator >= frameDuration) {
            timeAccumulator -= frameDuration;
            
            plm_frame_t* frame = plm_decode_video(plm);
            if (frame != nullptr) {
                UpdateTexture(yTexture, frame->y.data);
                UpdateTexture(uTexture, frame->cb.data);
                UpdateTexture(vTexture, frame->cr.data);
            }
        }

        BeginShaderMode(yuvShader);
            rlActiveTextureSlot(0); rlEnableTexture(yTexture.id);
            rlActiveTextureSlot(1); rlEnableTexture(uTexture.id);
            rlActiveTextureSlot(2); rlEnableTexture(vTexture.id);
            rlActiveTextureSlot(0); 

            DrawTexturePro(yTexture, 
                           Rectangle{ 0, 0, (float)yTexture.width, (float)yTexture.height },
                           Rectangle{ 0, 0, 800, 600 }, 
                           Vector2{ 0, 0 }, 0.0f, WHITE);
        EndShaderMode();

        if (plm_has_ended(plm)) {
            plm_destroy(plm);
            UnloadTexture(yTexture);
            UnloadTexture(uTexture);
            UnloadTexture(vTexture);
            UnloadShader(yuvShader);
            
            plm = nullptr;
            videoPlaying = false;
            
            SetMasterVolume(gameSettings.volume);
            isInitialized = false; 
            currentState = STATE_EXIT; 
        }
        
        return; 
    }


    if (CheckCollisionPointRec(mousePos, screenModeButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        tempFullscreen = !tempFullscreen;
        ToggleFullscreen();
    }

    if (CheckCollisionPointRec(mousePos, volumeSliderBar) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        tempVolume = (mousePos.x - volumeSliderBar.x) / volumeSliderBar.width;
        if (tempVolume < 0.0f) tempVolume = 0.0f;
        if (tempVolume > 1.0f) tempVolume = 1.0f;
        SetMasterVolume(tempVolume);
    }

    if (CheckCollisionPointCircle(mousePos, radioClassicCenter, radioRadius) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        isRealistic = false;
    }
    if (CheckCollisionPointCircle(mousePos, radioRealisticCenter, radioRadius) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        isRealistic = true;
    }

    if (CheckCollisionPointRec(mousePos, saveButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        gameSettings.volume = tempVolume;
        gameSettings.fullscreen = tempFullscreen;
        
        SaveSettings(); 

        if (isRealistic) {
            plm = plm_create_with_filename("ASSets/video/absolutni_realita.mpeg");

            if (plm != nullptr) {
                videoPlaying = true;
                timeAccumulator = 0.0f;
                SetMasterVolume(0.0f);
                
                int w = plm_get_width(plm);
                int h = plm_get_height(plm);
                
                yuvShader = LoadShaderFromMemory(nullptr, yuvShaderSource);
                
                int yTexLoc = GetShaderLocation(yuvShader, "yTexture");
                int uTexLoc = GetShaderLocation(yuvShader, "uTexture");
                int vTexLoc = GetShaderLocation(yuvShader, "vTexture");
                
                int yTexUnit = 0; int uTexUnit = 1; int vTexUnit = 2;
                SetShaderValue(yuvShader, yTexLoc, &yTexUnit, SHADER_UNIFORM_INT);
                SetShaderValue(yuvShader, uTexLoc, &uTexUnit, SHADER_UNIFORM_INT);
                SetShaderValue(yuvShader, vTexLoc, &vTexUnit, SHADER_UNIFORM_INT);

                Image yImg = { nullptr, w, h, 1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE };
                yTexture = LoadTextureFromImage(yImg);
                Image uImg = { nullptr, w / 2, h / 2, 1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE };
                uTexture = LoadTextureFromImage(uImg);
                Image vImg = { nullptr, w / 2, h / 2, 1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE };
                vTexture = LoadTextureFromImage(vImg);
                
                SetTextureFilter(yTexture, TEXTURE_FILTER_BILINEAR);
                SetTextureFilter(uTexture, TEXTURE_FILTER_BILINEAR);
                SetTextureFilter(vTexture, TEXTURE_FILTER_BILINEAR);
                return;
            } else {
                currentState = STATE_EXIT;
            }
        } else {
            isInitialized = false;
            currentState = STATE_MENU;
            return;
        }
    }

    bool backPressed = (CheckCollisionPointRec(mousePos, backButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
    if (input.IsBackTriggered() || backPressed)
    {
       if (IsWindowFullscreen() != backupFullscreen) {
            ToggleFullscreen();
        }

        SetMasterVolume(backupVolume);

        isInitialized = false; 
        currentState = STATE_MENU;
        return;
    }

    DrawTextEx(AssetManager::mainFont, "NASTAVENI", Vector2{ 310, 60 }, 30.0f, 2.0f, BLACK);

    DrawTextEx(AssetManager::mainFont, "REZIM OBRAZOVKY:", Vector2{ 250, 130 }, 16.0f, 1.0f, DARKGRAY);
    DrawRectangleRec(screenModeButton, CheckCollisionPointRec(mousePos, screenModeButton) ? LIGHTGRAY : GRAY);
    if (tempFullscreen) {
        DrawTextEx(AssetManager::mainFont, "CELA OBRAZOVKA", Vector2{ screenModeButton.x + 45, screenModeButton.y + 14 }, 14.0f, 1.0f, BLACK);
    } else {
        DrawTextEx(AssetManager::mainFont, "V OKNE", Vector2{ screenModeButton.x + 95, screenModeButton.y + 14 }, 14.0f, 1.0f, BLACK);
    }

    DrawTextEx(AssetManager::mainFont, "HLASITOST:", Vector2{ 250, 230 }, 16.0f, 1.0f, DARKGRAY);
    DrawRectangleRec(volumeSliderBar, LIGHTGRAY);
    DrawRectangle((int)volumeSliderBar.x, (int)volumeSliderBar.y, (int)(volumeSliderBar.width * tempVolume), (int)volumeSliderBar.height, RED);
    DrawRectangle((int)(volumeSliderBar.x + volumeSliderBar.width * tempVolume - 8), (int)volumeSliderBar.y - 5, 16, 25, DARKGRAY);

    char volText[16];
    TextCopy(volText, TextFormat("%d %%", (int)(tempVolume * 100)));
    DrawTextEx(AssetManager::mainFont, volText, Vector2{ volumeSliderBar.x + volumeSliderBar.width + 20, volumeSliderBar.y - 2 }, 16.0f, 1.0f, BLACK);

    DrawTextEx(AssetManager::mainFont, "KVALITA GRAFIKY:", Vector2{ 250, 305 }, 16.0f, 1.0f, DARKGRAY);

    bool hoverClassic = CheckCollisionPointCircle(mousePos, radioClassicCenter, radioRadius);
    DrawCircleV(radioClassicCenter, radioRadius + 2, DARKGRAY);
    DrawCircleV(radioClassicCenter, radioRadius, hoverClassic ? LIGHTGRAY : RAYWHITE);
    if (!isRealistic) {
        DrawCircleV(radioClassicCenter, radioRadius - 4, BLACK);
    }
    DrawTextEx(AssetManager::mainFont, "KLASICKA", Vector2{ radioClassicCenter.x + 20, radioClassicCenter.y - 7 }, 14.0f, 1.0f, BLACK);

    bool hoverRealistic = CheckCollisionPointCircle(mousePos, radioRealisticCenter, radioRadius);
    DrawCircleV(radioRealisticCenter, radioRadius + 2, DARKGRAY);
    DrawCircleV(radioRealisticCenter, radioRadius, hoverRealistic ? LIGHTGRAY : RAYWHITE);
    if (isRealistic) {
        DrawCircleV(radioRealisticCenter, radioRadius - 4, RED);
    }
    DrawTextEx(AssetManager::mainFont, "REALISTICKA", Vector2{ radioRealisticCenter.x + 20, radioRealisticCenter.y - 7 }, 14.0f, 1.0f, isRealistic ? RED : BLACK);

    bool hoverSave = CheckCollisionPointRec(mousePos, saveButton);
    DrawRectangleRec(saveButton, hoverSave ? LIME : GREEN);
    DrawTextEx(AssetManager::mainFont, "ULOZIT ZMENY", Vector2{ saveButton.x + 65, saveButton.y + 14 }, 14.0f, 1.0f, WHITE);

    bool hoverBack = CheckCollisionPointRec(mousePos, backButton);
    DrawRectangleRec(backButton, hoverBack ? LIGHTGRAY : GRAY);
    DrawTextEx(AssetManager::mainFont, "ZPET BEZ ULOZENI", Vector2{ backButton.x + 50, backButton.y + 14 }, 14.0f, 1.0f, BLACK);
}