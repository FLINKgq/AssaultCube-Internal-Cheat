#include "Render.h"
#include "../hooks/hooks.h"
#include "../menu/menu.h" 
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_opengl2.h"
#include "../imgui/imgui_impl_win32.h"
#include "../globals/globals.h"
#include "../esp/esp.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include <GL/gl.h>
#include "../icon.h"

/*if you want to load photo from your own path

GLuint LoadTextureFromFile(const char* filename) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL) return 0;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    return image_texture;
}*/

GLuint LoadTextureFromMemory(const unsigned char* image_data_array, int data_size) {
    int image_width = 0;
    int image_height = 0;

    unsigned char* image_data = stbi_load_from_memory(image_data_array, data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL) return 0;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    return image_texture;
}

namespace Render {
    bool imguiInit = false;

    void OnSwapBuffers(HDC hDc) {
        if (!imguiInit) {
            //Globals::Settings::Esp::previewTexture = (void*)(intptr_t)LoadTextureFromFile("C:\\Photos\\icon.png");
            Globals::Settings::Esp::previewTexture = (void*)(intptr_t)LoadTextureFromMemory(iconBytes, sizeof(iconBytes));
            Hooks::window = FindWindowA("SDL_app", "AssaultCube");
            if (!Hooks::window) Hooks::window = FindWindowA(NULL, "AssaultCube");
            if (!Hooks::window) Hooks::window = WindowFromDC(hDc); 

            if (!Hooks::window) return;

            Hooks::oWndProc = (Hooks::tWndProc)SetWindowLongPtrA(Hooks::window, GWLP_WNDPROC, (LONG_PTR)Hooks::hkWndProc);

            ImGui::CreateContext();
            ImGui_ImplWin32_Init(Hooks::window);
            ImGui_ImplOpenGL2_Init();
            imguiInit = true;
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::GetIO().MouseDrawCursor = Menu::IsOpen();

        Menu::Draw();
        if (Globals::Settings::Esp::isEnabled) {
            ESP::Draw();
        }
        if (Globals::Settings::Esp::drawCircle) {
            ESP::DrawFovCircle();
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }


    void Shutdown() {
        if (imguiInit) {
            ImGui_ImplOpenGL2_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }
    }
}