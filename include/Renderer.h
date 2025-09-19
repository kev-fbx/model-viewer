#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    static void Init();
    static void Shutdown();
    
    // Clear functions
    static void Clear();
    static void Clear(float r, float g, float b, float a = 1.0f);
    static void SetClearColor(float r, float g, float b, float a = 1.0f);
    
    // Viewport
    static void SetViewport(int x, int y, int width, int height);
    static void SetViewport(int width, int height);
    
    // Depth testing
    static void EnableDepthTest(bool enable = true);
    static void SetDepthFunc(GLenum func);
    
    // Face culling
    static void EnableFaceCulling(bool enable = true);
    static void SetCullFace(GLenum face);
    static void SetFrontFace(GLenum mode);
    
    // Blending
    static void EnableBlending(bool enable = true);
    static void SetBlendFunc(GLenum sfactor, GLenum dfactor);
    
    // Wireframe mode
    static void SetPolygonMode(GLenum mode);
    
    // State queries
    static bool IsDepthTestEnabled();
    static bool IsFaceCullingEnabled();
    static bool IsBlendingEnabled();
    
    // Utility functions
    static void SetLineWidth(float width);
    static void SetPointSize(float size);
    
    // ImGui support
    static void BeginImGui();
    static void EndImGui();
    
private:
    static bool s_DepthTestEnabled;
    static bool s_FaceCullingEnabled;
    static bool s_BlendingEnabled;
};