#include "GlassDecoration.hpp"
#include "GlassPassElement.hpp"
#include "Globals.hpp"
#include "WindowGeometry.hpp"

#include <algorithm>
#include <array>
#include <GLES3/gl32.h>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/desktop/rule/windowRule/WindowRuleApplicator.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprutils/math/Misc.hpp>

CGlassDecoration::CGlassDecoration(PHLWINDOW window)
    : IHyprWindowDecoration(window), m_window(window) {}

SPreset CGlassDecoration::resolveWindowPreset() const {
    try {
        const auto window = m_window.lock();
        if (window && window->m_ruleApplicator) {
            for (const auto& tag : window->m_ruleApplicator->m_tagKeeper.getTags()) {
                if (tag.starts_with(TAG_PRESET_PREFIX)) {
                    const std::string name = tag.substr(TAG_PRESET_PREFIX.size());
                    return resolvePreset(name, g_pGlobalState->presets, g_pGlobalState->config);
                }
            }
        }
    } catch (...) {}
    return {};
}

bool CGlassDecoration::hasGlassTag() const {
    try {
        const auto window = m_window.lock();
        if (window && window->m_ruleApplicator) {
            for (const auto& tag : window->m_ruleApplicator->m_tagKeeper.getTags()) {
                if (tag.starts_with(TAG_PRESET_PREFIX))
                    return true;
            }
        }
    } catch (...) {}
    return false;
}

SDecorationPositioningInfo CGlassDecoration::getPositioningInfo() {
    SDecorationPositioningInfo info;
    info.priority       = 10000;
    info.policy         = DECORATION_POSITION_ABSOLUTE;
    info.desiredExtents = {{0, 0}, {0, 0}};
    return info;
}

void CGlassDecoration::onPositioningReply(const SDecorationPositioningReply& reply) {}

void CGlassDecoration::draw(PHLMONITOR monitor, float const& alpha) {
    if (!**g_pGlobalState->config.enabled) return;
    if (!hasGlassTag()) return;


    const auto window = m_window.lock();
    if (window) {
        const auto workspace = window->m_workspace;
        if (workspace && !window->m_pinned && workspace->m_renderOffset->isBeingAnimated()) {
            m_needsResample = true;
            damageEntire();
        }
        const auto pos  = window->m_realPosition->value();
        const auto size = window->m_realSize->value();
        if (pos != m_lastPosition || size != m_lastSize) {
            m_needsResample = true;
            damageEntire();
            m_lastPosition = pos;
            m_lastSize     = size;
        }
    }

    CGlassPassElement::SGlassPassData data{this, alpha};
    g_pHyprRenderer->m_renderPass.add(makeUnique<CGlassPassElement>(data));
}

PHLWINDOW CGlassDecoration::getOwner() { return m_window.lock(); }
bool CGlassDecoration::needsResample() const noexcept { return m_needsResample; }

<<<<<<< HEAD
void CGlassDecoration::sampleBackground(CFramebuffer& sourceFramebuffer, CBox box) {
=======
void CGlassDecoration::sampleBackground(CFramebuffer& src, CBox box) {
>>>>>>> 771965e (easier documentation n ughh more preferable configuration)
    const int pad = SAMPLE_PADDING_PX;
    int pw = static_cast<int>(box.width)  + 2 * pad;
    int ph = static_cast<int>(box.height) + 2 * pad;

    if (m_sampleFramebuffer.m_size.x != pw || m_sampleFramebuffer.m_size.y != ph)
        m_sampleFramebuffer.alloc(pw, ph, src.m_drmFormat);

    int x0 = static_cast<int>(box.x) - pad, x1 = static_cast<int>(box.x + box.width)  + pad;
    int y0 = static_cast<int>(box.y) - pad, y1 = static_cast<int>(box.y + box.height) + pad;
    int fw = static_cast<int>(src.m_size.x), fh = static_cast<int>(src.m_size.y);
    int dx0 = 0, dy0 = 0, dx1 = pw, dy1 = ph;

    if (x0 < 0)  { dx0 += -x0; x0 = 0; }
    if (y0 < 0)  { dy0 += -y0; y0 = 0; }
    if (x1 > fw) { dx1 -= x1 - fw; x1 = fw; }
    if (y1 > fh) { dy1 -= y1 - fh; y1 = fh; }

    m_samplePaddingRatio = Vector2D(double(pad) / pw, double(pad) / ph);

    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.getFBID());
=======
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.getFBID());
>>>>>>> 771965e (easier documentation n ughh more preferable configuration)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_sampleFramebuffer.getFBID());
    glBlitFramebuffer(x0, y0, x1, y1, dx0, dy0, dx1, dy1, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void CGlassDecoration::blurBackground(float radius, int iterations, GLuint callerFBID, int vpW, int vpH) {
    auto& sm = g_pGlobalState->shaderManager;
    if (radius <= 0.0f || iterations <= 0 || !sm.isInitialized()) return;

    int w = static_cast<int>(m_sampleFramebuffer.m_size.x);
    int h = static_cast<int>(m_sampleFramebuffer.m_size.y);

    auto& tmp = g_pGlobalState->blurTempFramebuffer;
    if (tmp.m_size.x != w || tmp.m_size.y != h)
        tmp.alloc(w, h, m_sampleFramebuffer.m_drmFormat);

    static constexpr std::array<float, 9> PROJ = {
        2.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, -1.0f, -1.0f, 1.0f };

<<<<<<< HEAD
    const auto& blurUniforms = shaderManager.blurUniforms;

    auto shader = g_pHyprOpenGL->useShader(shaderManager.blurShader);
    shader->setUniformMatrix3fv(SHADER_PROJ, 1, GL_FALSE, FULLSCREEN_PROJECTION);
    shader->setUniformInt(SHADER_TEX, 0);
    glUniform1f(blurUniforms.radius, radius);
    glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));
    g_pHyprOpenGL->setViewport(0, 0, width, height);
=======
    auto& bs = sm.blurShader;
    const auto& bu = sm.blurUniforms;
    g_pHyprOpenGL->useProgram(bs.program);
    bs.setUniformMatrix3fv(SHADER_PROJ, 1, GL_FALSE, PROJ);
    bs.setUniformInt(SHADER_TEX, 0);
    glUniform1f(bu.radius, radius);
    glBindVertexArray(bs.uniformLocations[SHADER_SHADER_VAO]);
    glViewport(0, 0, w, h);
>>>>>>> 771965e (easier documentation n ughh more preferable configuration)
    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < iterations; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, tmp.getFBID());
        m_sampleFramebuffer.getTexture()->bind();
        glUniform2f(bu.direction, 1.0f / w, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindFramebuffer(GL_FRAMEBUFFER, m_sampleFramebuffer.getFBID());
        tmp.getTexture()->bind();
        glUniform2f(bu.direction, 0.0f, 1.0f / h);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, callerFBID);
    glBindVertexArray(0);
    glViewport(0, 0, vpW, vpH);
}

void CGlassDecoration::uploadUniforms(const SPreset& p) const {
    const auto& u = g_pGlobalState->shaderManager.glassUniforms;
    glUniform1f(u.refractionStrength,  p.refractionStrength);
    glUniform1f(u.chromaticAberration, p.chromaticAberration);
    glUniform1f(u.fresnelStrength,     p.fresnelStrength);
    glUniform1f(u.specularStrength,    p.specularStrength);
    glUniform1f(u.edgeThickness,       p.edgeThickness);
    glUniform1f(u.lensDistortion,      p.lensDistortion);
    glUniform1f(u.brightness,          p.brightness);
    glUniform1f(u.contrast,            p.contrast);
    glUniform1f(u.saturation,          p.saturation);
=======
    glViewport(0, 0, vpW, vpH);
}

void CGlassDecoration::uploadUniforms(const SPreset& p) const {
    const auto& u = g_pGlobalState->shaderManager.glassUniforms;
    glUniform1f(u.refractionStrength,  p.refractionStrength);
    glUniform1f(u.chromaticAberration, p.chromaticAberration);
    glUniform1f(u.fresnelStrength,     p.fresnelStrength);
    glUniform1f(u.specularStrength,    p.specularStrength);
    glUniform1f(u.edgeThickness,       p.edgeThickness);
    glUniform1f(u.lensDistortion,      p.lensDistortion);
    glUniform1f(u.brightness,          p.brightness);
    glUniform1f(u.contrast,            p.contrast);
    glUniform1f(u.saturation,          p.saturation);
    glUniform1f(u.vibrancy,            p.vibrancy);
    glUniform1f(u.vibrancyDarkness,    0.0f);
    glUniform1f(u.adaptiveDim,         p.adaptiveDim);
    glUniform1f(u.adaptiveBoost,       p.adaptiveBoost);
    glUniform3f(u.tintColor,
        float((p.tintColor >> 24) & 0xFF) / 255.0f,
        float((p.tintColor >> 16) & 0xFF) / 255.0f,
        float((p.tintColor >>  8) & 0xFF) / 255.0f);
    glUniform1f(u.tintAlpha, float(p.tintColor & 0xFF) / 255.0f);
>>>>>>> 771965e (easier documentation n ughh more preferable configuration)
}

void CGlassDecoration::applyGlassEffect(CFramebuffer& src, CFramebuffer& dst,
                                         CBox& rawBox, CBox& transformedBox,
                                         float windowAlpha, const SPreset& p) {
    auto& sm = g_pGlobalState->shaderManager;
    const auto& u = sm.glassUniforms;

    const auto transform = Math::wlTransformToHyprutils(
        Math::invertTransform(g_pHyprOpenGL->m_renderData.pMonitor->m_transform));
    Mat3x3 matrix   = g_pHyprOpenGL->m_renderData.monitorProjection.projectBox(rawBox, transform, rawBox.rot);
    Mat3x3 glMatrix = g_pHyprOpenGL->m_renderData.projection.copy().multiply(matrix);
    glMatrix.transpose();

    glBindFramebuffer(GL_FRAMEBUFFER, dst.getFBID());
    glActiveTexture(GL_TEXTURE0);
    src.getTexture()->bind();

<<<<<<< HEAD
    auto shader = g_pHyprOpenGL->useShader(shaderManager.glassShader);

    shader->setUniformMatrix3fv(SHADER_PROJ, 1, GL_FALSE, glMatrix.getMatrix());
    shader->setUniformInt(SHADER_TEX, 0);

    const auto fullSize = Vector2D(transformedBox.width, transformedBox.height);
    shader->setUniformFloat2(SHADER_FULL_SIZE,
        static_cast<float>(fullSize.x), static_cast<float>(fullSize.y));

    glUniform1f(uniforms.refractionStrength,  resolvePresetFloat(ctx, &SPresetValues::refractionStrength, &SOverridableConfig::refractionStrength));
    glUniform1f(uniforms.chromaticAberration, resolvePresetFloat(ctx, &SPresetValues::chromaticAberration, &SOverridableConfig::chromaticAberration));
    glUniform1f(uniforms.fresnelStrength,     resolvePresetFloat(ctx, &SPresetValues::fresnelStrength, &SOverridableConfig::fresnelStrength));
    glUniform1f(uniforms.specularStrength,    resolvePresetFloat(ctx, &SPresetValues::specularStrength, &SOverridableConfig::specularStrength));
    glUniform1f(uniforms.glassOpacity,        resolvePresetFloat(ctx, &SPresetValues::glassOpacity, &SOverridableConfig::glassOpacity) * windowAlpha);
    glUniform1f(uniforms.edgeThickness,       resolvePresetFloat(ctx, &SPresetValues::edgeThickness, &SOverridableConfig::edgeThickness));
    glUniform1f(uniforms.lensDistortion,      resolvePresetFloat(ctx, &SPresetValues::lensDistortion, &SOverridableConfig::lensDistortion));

    uploadThemeUniforms(ctx);

    const int64_t tintColorValue = resolvePresetInt(ctx, &SPresetValues::tintColor, &SOverridableConfig::tintColor);
    glUniform3f(uniforms.tintColor,
        static_cast<float>((tintColorValue >> 24) & 0xFF) / 255.0f,
        static_cast<float>((tintColorValue >> 16) & 0xFF) / 255.0f,
        static_cast<float>((tintColorValue >> 8) & 0xFF) / 255.0f);
    glUniform1f(uniforms.tintAlpha,
        static_cast<float>(tintColorValue & 0xFF) / 255.0f);

    glUniform2f(uniforms.uvPadding,
        static_cast<float>(m_samplePaddingRatio.x),
        static_cast<float>(m_samplePaddingRatio.y));

    const auto window = m_window.lock();
    float monitorScale = g_pHyprOpenGL->m_renderData.pMonitor->m_scale;
    float cornerRadius  = window ? window->rounding() * monitorScale : 0.0f;
    float roundingPower = window ? window->roundingPower() : 2.0f;
    shader->setUniformFloat(SHADER_RADIUS, cornerRadius);
    shader->setUniformFloat(SHADER_ROUNDING_POWER, roundingPower);

    glBindVertexArray(shader->getUniformLocation(SHADER_SHADER_VAO));
=======
    g_pHyprOpenGL->useProgram(sm.glassShader.program);
    sm.glassShader.setUniformMatrix3fv(SHADER_PROJ, 1, GL_FALSE, glMatrix.getMatrix());
    sm.glassShader.setUniformInt(SHADER_TEX, 0);
    sm.glassShader.setUniformFloat2(SHADER_FULL_SIZE,
        float(transformedBox.width), float(transformedBox.height));

    glUniform1f(u.glassOpacity, p.glassOpacity * windowAlpha);
    glUniform2f(u.uvPadding, float(m_samplePaddingRatio.x), float(m_samplePaddingRatio.y));

    const auto window   = m_window.lock();
    float monitorScale  = g_pHyprOpenGL->m_renderData.pMonitor->m_scale;
    float cornerRadius  = window ? window->rounding() * monitorScale : 0.0f;
    float roundingPower = window ? window->roundingPower() : 2.0f;
    sm.glassShader.setUniformFloat(SHADER_RADIUS, cornerRadius);
    glUniform1f(u.roundingPower, roundingPower);

    uploadUniforms(p);

    glBindVertexArray(sm.glassShader.uniformLocations[SHADER_SHADER_VAO]);
>>>>>>> 771965e (easier documentation n ughh more preferable configuration)
    g_pHyprOpenGL->scissor(rawBox);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    g_pHyprOpenGL->scissor(nullptr);
}

void CGlassDecoration::renderPass(PHLMONITOR monitor, const float& alpha) {
    auto& sm = g_pGlobalState->shaderManager;
    sm.initializeIfNeeded();
    if (!sm.isInitialized()) return;

    const auto window = m_window.lock();
    if (!window) return;

    const auto workspace       = window->m_workspace;
    const auto workspaceOffset = workspace && !window->m_pinned
        ? workspace->m_renderOffset->value() : Vector2D();
    const auto source = g_pHyprOpenGL->m_renderData.currentFB;

    CBox windowBox = window->getWindowMainSurfaceBox()
                         .translate(workspaceOffset)
                         .translate(-monitor->m_position + window->m_floatingOffset)
                         .scale(monitor->m_scale)
                         .round();
    CBox transformBox = windowBox;
    const auto transform = Math::wlTransformToHyprutils(
        Math::invertTransform(g_pHyprOpenGL->m_renderData.pMonitor->m_transform));
    transformBox.transform(transform,
        g_pHyprOpenGL->m_renderData.pMonitor->m_transformedSize.x,
        g_pHyprOpenGL->m_renderData.pMonitor->m_transformedSize.y);

<<<<<<< HEAD
    sampleBackground(*source, transformBox);

    {
        const auto& config         = g_pGlobalState->config;
        const bool isDark          = resolveThemeIsDark();
        const std::string preset   = resolvePresetName();
        const SResolveContext ctx  = {preset, isDark, config, g_pGlobalState->customPresets};

        float blurRadius     = resolvePresetFloat(ctx, &SPresetValues::blurStrength, &SOverridableConfig::blurStrength) * 12.0f;
        int blurIterations   = std::clamp(static_cast<int>(resolvePresetInt(ctx, &SPresetValues::blurIterations, &SOverridableConfig::blurIterations)), 1, 5);
        int viewportWidth    = static_cast<int>(g_pHyprOpenGL->m_renderData.pMonitor->m_transformedSize.x);
        int viewportHeight   = static_cast<int>(g_pHyprOpenGL->m_renderData.pMonitor->m_transformedSize.y);
        blurBackground(blurRadius, blurIterations, source->getFBID(), viewportWidth, viewportHeight);
=======
    const SPreset p        = resolveWindowPreset();
    const bool    isLive   = p.live != 0;
    const bool    hasCache = m_sampleFramebuffer.m_size.x > 0;

    if (m_needsResample || !hasCache || isLive) {
        sampleBackground(*source, transformBox);
        float blurR = p.blurStrength * 12.0f;
        int   blurI = std::clamp((int)p.blurIterations, 1, 5);
        int   vpW   = int(g_pHyprOpenGL->m_renderData.pMonitor->m_transformedSize.x);
        int   vpH   = int(g_pHyprOpenGL->m_renderData.pMonitor->m_transformedSize.y);
        blurBackground(blurR, blurI, source->getFBID(), vpW, vpH);
>>>>>>> 771965e (easier documentation n ughh more preferable configuration)
    }

    applyGlassEffect(m_sampleFramebuffer, *source, windowBox, transformBox, alpha, p);
}

eDecorationType  CGlassDecoration::getDecorationType()  { return DECORATION_CUSTOM; }
eDecorationLayer CGlassDecoration::getDecorationLayer() { return DECORATION_LAYER_BOTTOM; }
uint64_t         CGlassDecoration::getDecorationFlags() { return DECORATION_NON_SOLID; }
std::string      CGlassDecoration::getDisplayName()     { return "HyprGlass"; }

void CGlassDecoration::updateWindow(PHLWINDOW window) {
<<<<<<< HEAD
=======
    m_needsResample = true;
>>>>>>> 771965e (easier documentation n ughh more preferable configuration)
    damageEntire();
}

void CGlassDecoration::damageEntire() {
    const auto window = m_window.lock();
    if (!window) return;
    const auto workspace = window->m_workspace;
    auto box = window->getWindowMainSurfaceBox();
    if (workspace && workspace->m_renderOffset->isBeingAnimated() && !window->m_pinned)
        box.translate(workspace->m_renderOffset->value());
    box.translate(window->m_floatingOffset);
    const auto monitor = window->m_monitor.lock();
    box.expand(SAMPLE_PADDING_PX / (monitor ? monitor->m_scale : 1.0f));
    g_pHyprRenderer->damageBox(box);
}
