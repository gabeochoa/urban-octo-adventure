

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wdeprecated-copy"
#endif

#define LLGL_BUILD_STATIC_LIB
#include <LLGL/LLGL.h>

#include <glm/glm.hpp>

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

#include <fstream>
#include <iostream>

void render() {
    std::string rendererModule = "OpenGL";

    // Load render system module
    std::unique_ptr<LLGL::RenderSystem> renderer =
        LLGL::RenderSystem::Load(rendererModule);

    LLGL::RenderContextDescriptor contextDesc;
    { contextDesc.videoMode.resolution = {640, 480}; }
    LLGL::RenderContext* context = renderer->CreateRenderContext(contextDesc);

    auto& window = LLGL::CastTo<LLGL::Window>(context->GetSurface());
    window.SetTitle(L"LLGL Tutorial 01: Hello Triangle");
    window.Show();

    // Vertex data structure
    struct Vertex {
        float position[2];
        uint8_t color[4];
    };

    // Vertex data (3 vertices for our triangle)
    const float s = 0.5f;

    Vertex vertices[] = {
        {{0, s}, {255, 0, 0, 255}},    // 1st vertex: center-top, red
        {{s, -s}, {0, 255, 0, 255}},   // 2nd vertex: right-bottom, green
        {{-s, -s}, {0, 0, 255, 255}},  // 3rd vertex: left-bottom, blue
    };

    // Vertex format
    LLGL::VertexFormat vertexFormat;

    // Append 2D float vector for position attribute
    vertexFormat.AppendAttribute({"position", LLGL::Format::RG32Float});

    // Append 3D unsigned byte vector for color
    vertexFormat.AppendAttribute({"color", LLGL::Format::RGBA8UNorm});

    // Update stride in case out vertex structure is not 4-byte aligned
    vertexFormat.SetStride(sizeof(Vertex));

    // Create vertex buffer
    LLGL::BufferDescriptor vertexBufferDesc;
    {
        vertexBufferDesc.size =
            sizeof(vertices);  // Size (in bytes) of the vertex buffer
        vertexBufferDesc.bindFlags =
            LLGL::BindFlags::VertexBuffer;  // Enables the buffer to be bound to
                                            // a vertex buffer slot
        vertexBufferDesc.vertexAttribs =
            vertexFormat.attributes;  // Vertex format layout
    }
    LLGL::Buffer* vertexBuffer =
        renderer->CreateBuffer(vertexBufferDesc, vertices);

    // Create shaders
    LLGL::Shader* vertShader = nullptr;
    LLGL::Shader* fragShader = nullptr;

    LLGL::ShaderDescriptor vertShaderDesc, fragShaderDesc;

    vertShaderDesc = {LLGL::ShaderType::Vertex, "test.vert"};
    fragShaderDesc = {LLGL::ShaderType::Fragment, "test.frag"};

    // Specify vertex attributes for vertex shader
    vertShaderDesc.vertex.inputAttribs = vertexFormat.attributes;

    vertShader = renderer->CreateShader(vertShaderDesc);
    fragShader = renderer->CreateShader(fragShaderDesc);

    for (auto shader : {vertShader, fragShader}) {
        if (shader != nullptr) {
            std::string log = shader->GetReport();
            if (!log.empty()) std::cerr << log << std::endl;
        }
    }

    // Create shader program which is used as composite
    LLGL::ShaderProgramDescriptor shaderProgramDesc;
    {
        shaderProgramDesc.vertexShader = vertShader;
        shaderProgramDesc.fragmentShader = fragShader;
    }
    LLGL::ShaderProgram* shaderProgram =
        renderer->CreateShaderProgram(shaderProgramDesc);

    // Link shader program and check for errors
    if (shaderProgram->HasErrors())
        throw std::runtime_error(shaderProgram->GetReport());

    // Create graphics pipeline
    LLGL::PipelineState* pipeline = nullptr;
    std::unique_ptr<LLGL::Blob> pipelineCache;

    {
        LLGL::GraphicsPipelineDescriptor pipelineDesc;
        {
            pipelineDesc.shaderProgram = shaderProgram;
            pipelineDesc.renderPass = context->GetRenderPass();
        }

        // Create graphics PSO
        pipeline = renderer->CreatePipelineState(pipelineDesc);
    }

    // Get command queue to record and submit command buffers
    LLGL::CommandQueue* queue = renderer->GetCommandQueue();

    // Create command buffer to submit subsequent graphics commands to the GPU
    LLGL::CommandBuffer* commands = renderer->CreateCommandBuffer();


    // Enter main loop
    while (window.ProcessEvents()) {
        // Begin recording commands
        commands->Begin();
        {
            // Set viewport and scissor rectangle
            commands->SetViewport(context->GetResolution());

            // Set graphics pipeline
            commands->SetPipelineState(*pipeline);

            // Set vertex buffer
            commands->SetVertexBuffer(*vertexBuffer);

            // Set the render context as the initial render target
            commands->BeginRenderPass(*context);
            {
                // Clear color buffer
                commands->Clear(LLGL::ClearFlags::Color);

                // Draw triangle with 3 vertices
                commands->Draw(3, 0);
            }
            commands->EndRenderPass();
        }
        commands->End();
        queue->Submit(*commands);

        // Present the result on the screen
        context->Present();
    }
}

int main() {
    try {
        render();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
