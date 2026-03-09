#pragma once

#include "PluginConfig.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>


struct SThemeDefaults {
    float brightness;
    float contrast;
    float saturation;
    float vibrancy;
    float vibrancyDarkness;
    float adaptiveDim;
    float adaptiveBoost;
};

inline constexpr SThemeDefaults DARK_THEME_DEFAULTS  = {0.88f, 0.95f, 1.2f, 0.6f, 0.1f, 0.1f, 0.0f};
inline constexpr SThemeDefaults LIGHT_THEME_DEFAULTS = {1.05f, 0.95f, 1.2f, 0.5f, 0.0f, 0.0f, 0.2f};


namespace GlobalDefaults {
    inline constexpr float   BLUR_STRENGTH        = 0.4f;
    inline constexpr int64_t BLUR_ITERATIONS      = 2;
    inline constexpr float   REFRACTION_STRENGTH  = 1.2f;
    inline constexpr float   CHROMATIC_ABERRATION = 0.3f;
    inline constexpr float   FRESNEL_STRENGTH     = 0.0f;
    inline constexpr float   SPECULAR_STRENGTH    = 0.0f;
    inline constexpr float   GLASS_OPACITY        = 0.75f;
    inline constexpr float   EDGE_THICKNESS       = 0.07f;
    inline constexpr int64_t TINT_COLOR           = 0x88aabb11;
    inline constexpr float   LENS_DISTORTION      = 0.6f;
} // namespace GlobalDefaults

// ── Built-in presets ─────────────────────────────────────────────────────────
// To add a new built-in preset: define a make*() function and register it
// in getAll().

namespace BuiltInPresets {

inline SCustomPreset makeHighContrast() {
    SCustomPreset p;
    p.name = "high_contrast";

    p.shared.blurStrength        = 1.2f;
    p.shared.blurIterations      = 2;
    p.shared.lensDistortion      = 0.5f;
    p.shared.refractionStrength  = 1.2f;
    p.shared.chromaticAberration = 0.25f;
    p.shared.fresnelStrength     = 0.3f;
    p.shared.specularStrength    = 0.8f;
    p.shared.glassOpacity        = 1.0f;
    p.shared.edgeThickness       = 0.06f;

    p.dark.brightness         = 0.82f;
    p.dark.contrast           = 1.14f;
    p.dark.saturation         = 0.92f;
    p.dark.vibrancy           = 0.5f;
    p.dark.vibrancyDarkness   = 0.2f;
    p.dark.adaptiveDim        = 0.25f;
    p.dark.tintColor          = 0x02142aa9;

    p.light.brightness         = 1.0f;
    p.light.contrast           = 0.92f;
    p.light.saturation         = 0.8f;
    p.light.vibrancy           = 0.12f;
    p.light.vibrancyDarkness   = 5.0f;
    p.light.adaptiveBoost      = 0.15f;
    p.light.tintColor          = 0xc2cddb33;

    return p;
}

inline SCustomPreset makeSubtle() {
    SCustomPreset p;
    p.name = "subtle";

    p.shared.blurStrength        = 1.0f;
    p.shared.refractionStrength  = 0.3f;
    p.shared.chromaticAberration = 0.2f;
    p.shared.fresnelStrength     = 0.3f;
    p.shared.specularStrength    = 0.4f;

    return p;
}

inline SCustomPreset makeClear() {
    SCustomPreset p;
    p.name = "clear";

    p.shared.blurStrength        = 0.0f;
    p.shared.refractionStrength  = 0.3f;
    p.shared.chromaticAberration = 0.2f;
    p.shared.fresnelStrength     = 0.3f;
    p.shared.specularStrength    = 0.4f;

    return p;
}

inline SCustomPreset makeGlass() {
    SCustomPreset p;
    p.name = "glass";

    p.shared.blurStrength        = 1.0f;
    p.shared.blurIterations      = 2;
    p.shared.lensDistortion      = 0.3f;
    p.shared.refractionStrength  = 8.0f;
    p.shared.chromaticAberration = 0.5f;
    p.shared.fresnelStrength     = 0.4f;
    p.shared.specularStrength    = 0.8f;
    p.shared.glassOpacity        = 1.0f;
    p.shared.edgeThickness       = 0.06f;
    p.shared.tintColor           = 0xffffff00;

    p.dark.adaptiveDim           = 0.3f;
    p.light.adaptiveBoost        = 0.3f;

    return p;
}

inline SCustomPreset makeApple() {
    SCustomPreset p;
    p.name = "apple";

    // Clear, minimal blur — vivid colors bleed through
    p.shared.blurStrength        = 0.4f;
    p.shared.blurIterations      = 2;
    p.shared.refractionStrength  = 1.4f;
    p.shared.chromaticAberration = 0.25f;
    p.shared.fresnelStrength     = 0.0f;
    p.shared.specularStrength    = 0.0f;
    p.shared.glassOpacity        = 0.72f;
    p.shared.edgeThickness       = 0.07f;
    p.shared.lensDistortion      = 0.7f;
    p.shared.tintColor           = 0x88aabb08; // very subtle tint

    p.dark.brightness         = 0.90f;
    p.dark.contrast           = 0.96f;
    p.dark.saturation         = 1.35f;
    p.dark.vibrancy           = 0.70f;
    p.dark.vibrancyDarkness   = 0.1f;
    p.dark.adaptiveDim        = 0.05f;
    p.dark.adaptiveBoost      = 0.05f;

    p.light.brightness         = 1.05f;
    p.light.contrast           = 0.96f;
    p.light.saturation         = 1.25f;
    p.light.vibrancy           = 0.55f;
    p.light.adaptiveBoost      = 0.15f;

    return p;
}

inline std::unordered_map<std::string, SCustomPreset> getAll() {
    std::unordered_map<std::string, SCustomPreset> presets;

    auto add = [&](SCustomPreset p) { presets[p.name] = std::move(p); };

    add(makeHighContrast());
    add(makeSubtle());
    add(makeClear());
    add(makeGlass());
    add(makeApple());

    return presets;
}

} // namespace BuiltInPresets
