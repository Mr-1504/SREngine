//
// Created by Nikita on 12.06.2021.
//

#ifndef GAMEENGINE_OPENGLPOSTPROCESSING_H
#define GAMEENGINE_OPENGLPOSTPROCESSING_H

#include <Render/PostProcessing.h>

namespace Framework::Graphics {
    class OpenGLPostProcessing : public PostProcessing {
    private:
        ~OpenGLPostProcessing() override = default;
    public:
        OpenGLPostProcessing(Camera* camera) : PostProcessing(camera) { }
    public:
        void BeginGeometry() override {
            if (m_HDRFrameBufferObject) {
                m_env->BindFrameBuffer(m_HDRFrameBufferObject);
                m_env->ClearBuffers();
            }
        }
        void EndGeometry() override{
            if (m_bloom){
                this->BlurBloom();
                if (m_bloomClear)
                    m_bloomClear = false;
            }
            else if (!m_bloomClear) {
                m_env->BindFrameBuffer(m_PingPongFrameBuffers[0]);
                m_env->ClearColorBuffers(0,0,0,0);
                m_env->ClearBuffers();
                m_env->BindFrameBuffer(m_PingPongFrameBuffers[1]);
                m_env->ClearColorBuffers(0,0,0,0);
                m_env->ClearBuffers();

                m_env->BindFrameBuffer(0);

                m_bloomClear = true;
            }

            if (m_camera->IsDirectOutput())
                m_env->BindFrameBuffer(0);
            else {
                m_env->BindFrameBuffer(this->m_finalFBO);
                m_env->ClearBuffers();
            }

            m_postProcessingShader->Use();

            {
                m_postProcessingShader->SetVec3("GammaExpSat", { m_gamma, m_exposure, m_saturation });
                m_postProcessingShader->SetVec3("ColorCorrection", m_color_correction);
            }

            m_env->BindTexture(0, m_ColorBuffers[0]);
            m_postProcessingShader->SetInt("scene", 0);

            if (m_bloom) { // SEE: POSSIBLE BUGS
                m_env->BindTexture(1, m_PingPongColorBuffers[!m_horizontal]);

                m_postProcessingShader->SetInt("bloomBlur", 1);

                m_postProcessingShader->SetVec3("BloomColor", m_bloomColor);
            }

            m_env->BindTexture(2, m_skyboxColorBuffer);
            m_postProcessingShader->SetInt("skybox", 2);

            m_env->BindTexture(3, m_ColorBuffers[3]);
            m_postProcessingShader->SetInt("stencil", 3);

            m_env->BindTexture(4, m_ColorBuffers[2]);
            m_postProcessingShader->SetInt("depth", 4);
            m_env->Draw(3);

            if (!m_camera->IsDirectOutput())
                m_env->BindFrameBuffer(0);
        }

        void BeginSkybox() override {
            m_env->BindFrameBuffer(this->m_skyboxFBO);
            m_env->ClearBuffers();
        }

        void EndSkybox() override {
            m_env->BindFrameBuffer(0);
        }
    private:
        void BlurBloom() {
            if (!m_blurShader)
                return;

            m_env->BindFrameBuffer(0);

            m_horizontal = true;
            m_firstIteration = true;

            m_blurShader->Use();
            m_blurShader->SetFloat("BloomIntensity", m_bloomIntensity);

            for (unsigned char i = 0; i < m_bloomAmount; i++) {
                m_env->BindFrameBuffer(m_PingPongFrameBuffers[m_horizontal]);

                m_blurShader->SetBool("horizontal", m_horizontal);

                m_env->BindTexture(0, m_firstIteration ? m_ColorBuffers[1] : m_PingPongColorBuffers[!m_horizontal]);
                m_blurShader->SetInt("image", 0);
                m_env->Draw(3);

                m_horizontal = !m_horizontal;
                if (m_firstIteration)
                    m_firstIteration = false;
            }
        }
    public:
        bool Free() override {
            Helper::Debug::Graph("OpenGLPostProcessing::Free() : free post processing pointer...");
            delete this;
            return true;
        }
        bool Destroy() override {
            return PostProcessing::Destroy();
        }

        bool OnResize(uint32_t w, uint32_t h) override {
            if (!m_env->CreateSingleHDRFrameBO({w, h}, m_finalRBO, m_finalFBO, m_finalColorBuffer)) {
                Helper::Debug::Error("PostProcessing::ReCalcFrameBuffers() : failed to create single HDR frame buffer object!");
                return false;
            }

            if (!m_env->CreateSingleHDRFrameBO({w, h}, m_skyboxRBO, m_skyboxFBO, m_skyboxColorBuffer)) {
                Helper::Debug::Error("PostProcessing::ReCalcFrameBuffers() : failed to create single HDR frame buffer object!");
                return false;
            }

            if (!m_env->CreateHDRFrameBufferObject({w, h}, m_RBODepth, m_HDRFrameBufferObject, m_ColorBuffers)) {
                Helper::Debug::Error("PostProcessing::ReCalcFrameBuffers() : failed to create HDR frame buffer object!");
                return false;
            }

            if (!m_env->CreatePingPongFrameBufferObject({w, h}, m_PingPongFrameBuffers, m_PingPongColorBuffers)) {
                Helper::Debug::Error("PostProcessing::ReCalcFrameBuffers() : failed to create ping pong frame buffer object!");
                return false;
            }

            return true;
        }
    };
}

#endif //GAMEENGINE_OPENGLPOSTPROCESSING_H
