#pragma once
#include <JuceHeader.h>

class OpenGLComponent : public juce::Component,
                        public juce::OpenGLRenderer{
    public:
        OpenGLComponent();
        ~OpenGLComponent();
        void paint(juce::Graphics& g);
        void resized();
        void newOpenGLContextCreated() override;
        void renderOpenGL() override;
        void openGLContextClosing() override;
    private:
        OpenGLContext openGLContext;
        struct Vertex {
            float position[2];
            float colour[4];
        };
        std::vector<Vertex> vertexBuffer;
        std::vector<unsigned int> indexBuffer;
        GLuint vbo;
        GLuint ibo;
        String vertexShader;
        String fragmentShader;
        std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;
};
