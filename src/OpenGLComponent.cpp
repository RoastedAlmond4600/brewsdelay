#include "OpenGLComponent.h"
OpenGLComponent::OpenGLComponent() {
    setOpaque(true);
    openGLContext.setRenderer(this);
    openGLContext.setContinuousRepainting(true);
    openGLContext.attachTo(*this);
}
OpenGLComponent::~OpenGLComponent() {
    openGLContext.detach();
}
void OpenGLComponent::paint(juce::Graphics& g) {

}
void OpenGLComponent::resized() {

}
void OpenGLComponent::newOpenGLContextCreated() {
    vertexShader = R"(
        #version 330 core
        in vec4 aPos;
        in vec4 sourceColour; 
        out vec4 fragColour;

        void main() {
            gl_Position = aPos;
            fragColour = sourceColour;
        }
            )";
    fragmentShader = R"(
        #version 330 core
        int vec4 fragColour;
        void main() {
            gl_FragColor = fragColour;
        }

    )";
    //Gen Buffers
    openGLContext.extensions.glGenBuffers(1, &vbo);
    openGLContext.extensions.glGenBuffers(1, &ibo);

    //Buffer Declarations
    vertexBuffer = {
        {
            {-0.5f, 0.5f},
            {1.f, 0.0f, 0.f, 1.f} //Red
        },
        {
            {0.5f, 0.5f},
            {1.f, 0.5f, 0.f, 1.f} //Orange
        },
        {
            {0.5f, -0.5f},
            {1.f, 1.f, 0.f, 1.f} //Yellow
        },
        {
            {-0.5f, -0.5f},
            {0.f, 0.f, 1.f, 1.f} //Blue
        }
    };
    indexBuffer = {
        0,1,2,
        3,4,5
    };
    //Bind Buffers
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
    openGLContext.extensions.glBufferData(juce::gl::GL_ARRAY_BUFFER, sizeof(Vertex) * vertexBuffer.size(), vertexBuffer.data(), juce::gl::GL_STATIC_DRAW);
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);
    openGLContext.extensions.glBufferData(juce::gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexBuffer.size(), indexBuffer.data(), juce::gl::GL_STATIC_DRAW);

    shaderProgram.reset(new OpenGLShaderProgram(openGLContext));
    if (shaderProgram->addVertexShader(vertexShader) && shaderProgram->addFragmentShader(fragmentShader) && shaderProgram->link()) {
        shaderProgram->use();
        std::printf("Rock and rolling\n");
    }
    else {
        jassertfalse;
    }
}
void OpenGLComponent::renderOpenGL() {
    juce::OpenGLHelpers::clear(juce::Colours::black);
    shaderProgram->use();
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);
    openGLContext.extensions.glVertexAttribPointer(0, 2, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    openGLContext.extensions.glEnableVertexAttribArray(0);
    openGLContext.extensions.glVertexAttribPointer(1, 4, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 2));
    openGLContext.extensions.glEnableVertexAttribArray(1);
    juce::gl::glDrawElements(juce::gl::GL_TRIANGLES, indexBuffer.size(), juce::gl::GL_UNSIGNED_INT, nullptr);
}
void OpenGLComponent::openGLContextClosing() {
    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);

}

