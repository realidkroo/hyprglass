#include "GlassDecoration.hpp"
#include "Globals.hpp"
#include "PluginConfig.hpp"

#include <fstream>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/event/EventBus.hpp>

static void onNewWindow(PHLWINDOW window) {
    if (std::ranges::any_of(window->m_windowDecorations,
                            [](const auto& decoration) { return decoration->getDisplayName() == "HyprGlass"; }))
        return;

    auto decoration = makeUnique<CGlassDecoration>(window);
    g_pGlobalState->decorations.emplace_back(decoration);
    decoration->m_self = decoration;
    HyprlandAPI::addWindowDecoration(PHANDLE, window, std::move(decoration));
}

static void onCloseWindow(PHLWINDOW window) {
    std::erase_if(g_pGlobalState->decorations, [&window](const auto& decoration) {
        auto locked = decoration.lock();
        return !locked || locked->getOwner() == window;
    });
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH        = __hyprland_api_get_hash();
    const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

    if (HASH != CLIENT_HASH) {
        HyprlandAPI::addNotification(PHANDLE,
            std::format("[{}] Version mismatch!", PLUGIN_NAME),
            CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("Version mismatch");
    }

    g_pGlobalState = std::make_unique<SGlobalState>();

    static auto onOpen = Event::bus()->m_events.window.open.listen([&](PHLWINDOW w) { onNewWindow(w); });

    static auto onClose = Event::bus()->m_events.window.close.listen([&](PHLWINDOW w) { onCloseWindow(w); });

    static auto onPreConfigReload = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "preConfigReload",
        [&](void* /*self*/, SCallbackInfo& /*info*/, std::any /*data*/) {
            clearPresets();
        });

    static auto onConfigReloaded = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "configReloaded",
        [&](void* /*self*/, SCallbackInfo& /*info*/, std::any /*data*/) {
            // Source our config AFTER Hyprland finishes its own reload
            const char* homeDir = getenv("HOME");
            if (homeDir) {
                std::string cfgPath = std::string(homeDir) + "/.config/hypr/hyprglass.conf";
                HyprlandAPI::invokeHyprctlCommand("keyword", "source " + cfgPath);
            }
            commitPresets();
        });

    registerConfig(PHANDLE);
    initConfigPointers(PHANDLE, g_pGlobalState->config);

    // Auto-source ~/.config/hypr/hyprglass.conf — create with defaults if missing
    const char* homeDir = getenv("HOME");
    if (homeDir) {
        std::string cfgPath = std::string(homeDir) + "/.config/hypr/hyprglass.conf";
        if (!std::ifstream(cfgPath).good()) {
            std::ofstream f(cfgPath);
            if (f.is_open()) {
                f << R"(# we all know why you were here. here the default configuration for the plugin. oh yeah after saving run hyprctl reload
#
# example of windowrule
#   windowrule = tag +hyprglass_preset_default, match:class ^(kitty)$

plugin:hyprglass {
    preset = default

    live               = 1 #dis if you want a live blur on a unhovered, or unfocused window. 0 for static 1 ( default) for live
    blur_strength      = 0.6 #as it says
    blur_iterations    = 2 #how many times the blur shader is applied. more iterations = blurrier, but also more expensive. 2 is a good default
    #
    # glass shaders - just play around wit it
    #
    refraction_strength  = 1.2 #how much the glass refracts light just configure it to ur liking
    chromatic_aberration = 0.25 #how much the glass disperses light
    fresnel_strength   = 0.0 #how much the glass reflects light
    specular_strength  = 0.0 #how much the glass reflects light
    glass_opacity      = 1.0
    edge_thickness     = 0.07
    lens_distortion    = 0.5
    tint_color         = 0x88aabb08
    brightness         = 0.90
    contrast           = 0.96
    saturation         = 1.35
    vibrancy           = 0.70
    adaptive_dim       = 0.05
    adaptive_boost     = 0.05

    #to make presets just copy the plugin block and change the name and values. then you can use them in windowrules
}
)";
                f.close();
            }
        }
        HyprlandAPI::invokeHyprctlCommand("keyword", "source " + cfgPath);
    }

    // Shadows must be enabled for the glass effect to sample the correct background.
    // Force-enable if the user has disabled them.
    static auto* const PSHADOWENABLED = (Hyprlang::INT* const*)g_pConfigManager->getConfigValuePtr("decoration:shadow:enabled");
    if (PSHADOWENABLED && !**PSHADOWENABLED) {
        HyprlandAPI::invokeHyprctlCommand("keyword", "decoration:shadow:enabled true");
    }

    for (auto& window : g_pCompositor->m_windows) {
        if (window->isHidden() || !window->m_isMapped)
            continue;
        onNewWindow(window);
    }

    HyprlandAPI::reloadConfig();

    return {std::string(PLUGIN_NAME), std::string(PLUGIN_DESCRIPTION), std::string(PLUGIN_AUTHOR), std::string(PLUGIN_VERSION)};
}

APICALL EXPORT void PLUGIN_EXIT() {
    for (auto& decoration : g_pGlobalState->decorations) {
        auto locked = decoration.lock();
        if (locked) {
            auto owner = locked->getOwner();
            if (owner)
                owner->removeWindowDeco(locked.get());
        }
    }

    g_pHyprRenderer->m_renderPass.removeAllOfType("CGlassPassElement");

    g_pGlobalState->shaderManager.destroy();
    g_pGlobalState.reset();
}
