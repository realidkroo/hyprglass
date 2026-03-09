#pragma once

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

inline constexpr std::string_view TAG_PRESET_PREFIX = "hyprglass_preset_";

// ── All settings in one struct ────────────────────────────────────────────────
struct SPreset {
    std::string name;
    int64_t live                = -1;
    float   blurStrength        = -1;
    int64_t blurIterations      = -1;
    float   refractionStrength  = -1;
    float   chromaticAberration = -1;
    float   fresnelStrength     = -1;
    float   specularStrength    = -1;
    float   glassOpacity        = -1;
    float   edgeThickness       = -1;
    int64_t tintColor           = -1;
    float   lensDistortion      = -1;
    float   brightness          = -1;
    float   contrast            = -1;
    float   saturation          = -1;
    float   vibrancy            = -1;
    float   adaptiveDim         = -1;
    float   adaptiveBoost       = -1;
};

// Hardcoded fallback defaults
struct SPresetDefaults {
    int64_t live                = 1;
    float   blurStrength        = 0.6f;
    int64_t blurIterations      = 2;
    float   refractionStrength  = 1.2f;
    float   chromaticAberration = 0.25f;
    float   fresnelStrength     = 0.0f;
    float   specularStrength    = 0.0f;
    float   glassOpacity        = 1.0f;
    float   edgeThickness       = 0.07f;
    int64_t tintColor           = 0x88aabb08;
    float   lensDistortion      = 0.5f;
    float   brightness          = 0.90f;
    float   contrast            = 0.96f;
    float   saturation          = 1.35f;
    float   vibrancy            = 0.70f;
    float   adaptiveDim         = 0.05f;
    float   adaptiveBoost       = 0.05f;
};

inline const SPresetDefaults DEFAULTS;

// Config pointers — one per setting, all flat under plugin:hyprglass
struct SPluginConfig {
    Hyprlang::INT* const*    enabled             = nullptr;
    Hyprlang::STRING const*  activePresetName    = nullptr;
    Hyprlang::INT* const*    live                = nullptr;
    Hyprlang::FLOAT* const*  blurStrength        = nullptr;
    Hyprlang::INT* const*    blurIterations      = nullptr;
    Hyprlang::FLOAT* const*  refractionStrength  = nullptr;
    Hyprlang::FLOAT* const*  chromaticAberration = nullptr;
    Hyprlang::FLOAT* const*  fresnelStrength     = nullptr;
    Hyprlang::FLOAT* const*  specularStrength    = nullptr;
    Hyprlang::FLOAT* const*  glassOpacity        = nullptr;
    Hyprlang::FLOAT* const*  edgeThickness       = nullptr;
    Hyprlang::INT* const*    tintColor           = nullptr;
    Hyprlang::FLOAT* const*  lensDistortion      = nullptr;
    Hyprlang::FLOAT* const*  brightness          = nullptr;
    Hyprlang::FLOAT* const*  contrast            = nullptr;
    Hyprlang::FLOAT* const*  saturation          = nullptr;
    Hyprlang::FLOAT* const*  vibrancy            = nullptr;
    Hyprlang::FLOAT* const*  adaptiveDim         = nullptr;
    Hyprlang::FLOAT* const*  adaptiveBoost       = nullptr;
};

// Read the current config values into a resolved SPreset
[[nodiscard]] SPreset resolveConfig(const SPluginConfig& cfg);

void registerConfig(HANDLE handle);
void initConfigPointers(HANDLE handle, SPluginConfig& config);

// Per-preset overrides stored by name (set via window rules)
// Multiple named presets can be defined — each is a full flat block
Hyprlang::CParseResult handlePresetBlock(const char* command, const char* value);
void clearPresets();
void commitPresets();

[[nodiscard]] SPreset resolvePreset(const std::string& name,
                                    const std::unordered_map<std::string, SPreset>& presets,
                                    const SPluginConfig& cfg);
