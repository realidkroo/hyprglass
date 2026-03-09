#include "PluginConfig.hpp"
#include "Globals.hpp"

#include <charconv>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#define CV(key) "plugin:hyprglass:" key

void registerConfig(HANDLE handle) {
    HyprlandAPI::addConfigValue(handle, CV("enabled"),              Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(handle, CV("preset"),               Hyprlang::STRING{"default"});
    HyprlandAPI::addConfigValue(handle, CV("live"),                 Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(handle, CV("blur_strength"),        Hyprlang::FLOAT{0.6f});
    HyprlandAPI::addConfigValue(handle, CV("blur_iterations"),      Hyprlang::INT{2});
    HyprlandAPI::addConfigValue(handle, CV("refraction_strength"),  Hyprlang::FLOAT{1.2f});
    HyprlandAPI::addConfigValue(handle, CV("chromatic_aberration"), Hyprlang::FLOAT{0.25f});
    HyprlandAPI::addConfigValue(handle, CV("fresnel_strength"),     Hyprlang::FLOAT{0.0f});
    HyprlandAPI::addConfigValue(handle, CV("specular_strength"),    Hyprlang::FLOAT{0.0f});
    HyprlandAPI::addConfigValue(handle, CV("glass_opacity"),        Hyprlang::FLOAT{1.0f});
    HyprlandAPI::addConfigValue(handle, CV("edge_thickness"),       Hyprlang::FLOAT{0.07f});
    HyprlandAPI::addConfigValue(handle, CV("tint_color"),           Hyprlang::INT{0x88aabb08});
    HyprlandAPI::addConfigValue(handle, CV("lens_distortion"),      Hyprlang::FLOAT{0.5f});
    HyprlandAPI::addConfigValue(handle, CV("brightness"),           Hyprlang::FLOAT{0.90f});
    HyprlandAPI::addConfigValue(handle, CV("contrast"),             Hyprlang::FLOAT{0.96f});
    HyprlandAPI::addConfigValue(handle, CV("saturation"),           Hyprlang::FLOAT{1.35f});
    HyprlandAPI::addConfigValue(handle, CV("vibrancy"),             Hyprlang::FLOAT{0.70f});
    HyprlandAPI::addConfigValue(handle, CV("adaptive_dim"),         Hyprlang::FLOAT{0.05f});
    HyprlandAPI::addConfigValue(handle, CV("adaptive_boost"),       Hyprlang::FLOAT{0.05f});
}

#define PTRFLOAT(key) (Hyprlang::FLOAT* const*)HyprlandAPI::getConfigValue(handle, CV(key))->getDataStaticPtr()
#define PTRINT(key)   (Hyprlang::INT*   const*)HyprlandAPI::getConfigValue(handle, CV(key))->getDataStaticPtr()
#define PTRSTR(key)   (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(handle, CV(key))->getDataStaticPtr()

void initConfigPointers(HANDLE handle, SPluginConfig& c) {
    c.enabled             = PTRINT  ("enabled");
    c.activePresetName    = PTRSTR  ("preset");
    c.live                = PTRINT  ("live");
    c.blurStrength        = PTRFLOAT("blur_strength");
    c.blurIterations      = PTRINT  ("blur_iterations");
    c.refractionStrength  = PTRFLOAT("refraction_strength");
    c.chromaticAberration = PTRFLOAT("chromatic_aberration");
    c.fresnelStrength     = PTRFLOAT("fresnel_strength");
    c.specularStrength    = PTRFLOAT("specular_strength");
    c.glassOpacity        = PTRFLOAT("glass_opacity");
    c.edgeThickness       = PTRFLOAT("edge_thickness");
    c.tintColor           = PTRINT  ("tint_color");
    c.lensDistortion      = PTRFLOAT("lens_distortion");
    c.brightness          = PTRFLOAT("brightness");
    c.contrast            = PTRFLOAT("contrast");
    c.saturation          = PTRFLOAT("saturation");
    c.vibrancy            = PTRFLOAT("vibrancy");
    c.adaptiveDim         = PTRFLOAT("adaptive_dim");
    c.adaptiveBoost       = PTRFLOAT("adaptive_boost");
}

#undef PTRFLOAT
#undef PTRINT
#undef PTRSTR

// Read current config pointers into a flat SPreset
SPreset resolveConfig(const SPluginConfig& c) {
    SPreset p;
    p.name               = c.activePresetName ? std::string(*c.activePresetName) : "default";
    p.live               = c.live                ? **c.live               : DEFAULTS.live;
    p.blurStrength       = c.blurStrength        ? **c.blurStrength       : DEFAULTS.blurStrength;
    p.blurIterations     = c.blurIterations      ? **c.blurIterations     : DEFAULTS.blurIterations;
    p.refractionStrength = c.refractionStrength  ? **c.refractionStrength : DEFAULTS.refractionStrength;
    p.chromaticAberration= c.chromaticAberration ? **c.chromaticAberration: DEFAULTS.chromaticAberration;
    p.fresnelStrength    = c.fresnelStrength     ? **c.fresnelStrength    : DEFAULTS.fresnelStrength;
    p.specularStrength   = c.specularStrength    ? **c.specularStrength   : DEFAULTS.specularStrength;
    p.glassOpacity       = c.glassOpacity        ? **c.glassOpacity       : DEFAULTS.glassOpacity;
    p.edgeThickness      = c.edgeThickness       ? **c.edgeThickness      : DEFAULTS.edgeThickness;
    p.tintColor          = c.tintColor           ? **c.tintColor          : DEFAULTS.tintColor;
    p.lensDistortion     = c.lensDistortion      ? **c.lensDistortion     : DEFAULTS.lensDistortion;
    p.brightness         = c.brightness          ? **c.brightness         : DEFAULTS.brightness;
    p.contrast           = c.contrast            ? **c.contrast           : DEFAULTS.contrast;
    p.saturation         = c.saturation          ? **c.saturation         : DEFAULTS.saturation;
    p.vibrancy           = c.vibrancy            ? **c.vibrancy           : DEFAULTS.vibrancy;
    p.adaptiveDim        = c.adaptiveDim         ? **c.adaptiveDim        : DEFAULTS.adaptiveDim;
    p.adaptiveBoost      = c.adaptiveBoost       ? **c.adaptiveBoost      : DEFAULTS.adaptiveBoost;
    return p;
}

// Per-window preset overrides — stored by preset name
// Syntax in hyprglass.conf:
//   [preset.vivid]
//   saturation = 1.8
//   glass_opacity = 0.9
static std::unordered_map<std::string, SPreset> s_pending;

static std::string s_parsingPreset;

Hyprlang::CParseResult handlePresetBlock(const char* cmd, const char* value) {
    // Not used in flat mode — placeholder for future named preset blocks
    Hyprlang::CParseResult r;
    return r;
}

void clearPresets()  { s_pending.clear(); }
void commitPresets() {
    if (g_pGlobalState) g_pGlobalState->presets = std::move(s_pending);
    s_pending.clear();
}

// Resolve a preset by name — falls back to global config values
SPreset resolvePreset(const std::string& name,
                      const std::unordered_map<std::string, SPreset>& presets,
                      const SPluginConfig& cfg) {
    // Start with global config as base
    SPreset base = resolveConfig(cfg);
    auto it = presets.find(name);
    if (it == presets.end()) return base;

    // Override base with any fields set in the named preset
    const auto& p = it->second;
    if (p.live               >= 0) base.live               = p.live;
    if (p.blurStrength       >= 0) base.blurStrength       = p.blurStrength;
    if (p.blurIterations     >= 0) base.blurIterations     = p.blurIterations;
    if (p.refractionStrength >= 0) base.refractionStrength = p.refractionStrength;
    if (p.chromaticAberration>= 0) base.chromaticAberration= p.chromaticAberration;
    if (p.fresnelStrength    >= 0) base.fresnelStrength    = p.fresnelStrength;
    if (p.specularStrength   >= 0) base.specularStrength   = p.specularStrength;
    if (p.glassOpacity       >= 0) base.glassOpacity       = p.glassOpacity;
    if (p.edgeThickness      >= 0) base.edgeThickness      = p.edgeThickness;
    if (p.tintColor          >= 0) base.tintColor          = p.tintColor;
    if (p.lensDistortion     >= 0) base.lensDistortion     = p.lensDistortion;
    if (p.brightness         >= 0) base.brightness         = p.brightness;
    if (p.contrast           >= 0) base.contrast           = p.contrast;
    if (p.saturation         >= 0) base.saturation         = p.saturation;
    if (p.vibrancy           >= 0) base.vibrancy           = p.vibrancy;
    if (p.adaptiveDim        >= 0) base.adaptiveDim        = p.adaptiveDim;
    if (p.adaptiveBoost      >= 0) base.adaptiveBoost      = p.adaptiveBoost;
    return base;
}
