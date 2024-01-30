#pragma once

#include <components.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#define K_UID(...) xpe::ui::GetID({__VA_ARGS__}).c_str()

namespace xpe
{
    namespace ui
    {
        struct ENGINE_API Color final
        {
            static constexpr ImColor LAVENDER_GREY = ImColor(205, 209, 228, 255);
            static constexpr ImColor BLACK_PEARL = ImColor(8, 14, 44, 255);
            static constexpr ImColor JORDY_BLUE = ImColor(137, 196, 244, 255);
            static constexpr ImColor CORNFLOWER_BLUE = ImColor(72, 113, 247, 255);
            static constexpr ImColor ALICE_BLUE = ImColor(228, 241, 254, 255);
            static constexpr ImColor TALL_POPPY = ImColor(192, 57, 43, 255);
            static constexpr ImColor CHESNUT_ROSE = ImColor(210, 77, 87, 255);
            static constexpr ImColor LYNCH = ImColor(108, 122, 137, 255);
            static constexpr ImColor SILVER_SAND = ImColor(189, 195, 199, 255);
            static constexpr ImColor IRON = ImColor(218, 223, 225, 255);
        };

        struct ENGINE_API Style final
        {
            static constexpr ImColor k_TitleBackground = Color::BLACK_PEARL;
            static constexpr ImColor k_TitleBackgroundActive = Color::LYNCH;
            static constexpr ImColor k_TitleBackgroundCollapsed = Color::SILVER_SAND;

            static constexpr ImColor k_WindowBackground = Color::BLACK_PEARL;

            static constexpr ImColor k_FrameBackground = Color::BLACK_PEARL;
            static constexpr ImColor k_FrameBackgroundHover = Color::SILVER_SAND;
            static constexpr ImColor k_FrameBackgroundActive = Color::IRON;

            static constexpr ImColor k_Header = Color::BLACK_PEARL;
            static constexpr ImColor k_HeaderHover = Color::LYNCH;
            static constexpr ImColor k_HeaderActive = Color::SILVER_SAND;

            static constexpr ImColor k_Btn = Color::CORNFLOWER_BLUE;
            static constexpr ImColor k_BtnHover = Color::JORDY_BLUE;
            static constexpr ImColor k_BtnActive = Color::LAVENDER_GREY;

            static constexpr ImColor k_Tab = Color::JORDY_BLUE;
            static constexpr ImColor k_TabHover = Color::LAVENDER_GREY;
            static constexpr ImColor k_TabActive = Color::CORNFLOWER_BLUE;
            static constexpr ImColor k_TabUnfocused = Color::JORDY_BLUE;
            static constexpr ImColor k_TabUnfocusedActive = Color::CORNFLOWER_BLUE;

            static constexpr ImColor k_MenubarBackground = Color::BLACK_PEARL;

            static constexpr ImColor k_PopupBackground = Color::BLACK_PEARL;

            static constexpr ImColor k_ScrollbarBackground = Color::IRON;
        };

        extern ImGuiIO* IO;
        extern ImGuiID DockspaceId;
        extern ImGuiDockNodeFlags DockspaceFlags;
        extern ImFont* RegularFont;
        extern ImFont* BoldFont;
        extern bool IsFrameResized;

        ENGINE_API void Init();
        ENGINE_API void Free();

        ENGINE_API void BeginFrame();
        ENGINE_API void EndFrame();

        ENGINE_API void SetDarkTheme();

        ENGINE_API void AddRegularFont(const char* filepath, float size);

        ENGINE_API void AddBoldFont(const char* filepath, float size);

        ENGINE_API void AddIconFont(const char* filepath, float size);

        ENGINE_API void SetFont(ImFont* font);

        ENGINE_API void SetIniFilename(const char *iniFilename);

        ENGINE_API string GetID(const vector<const char*> &str);

        ENGINE_API void EnableInput();

        ENGINE_API void DisableInput();

        ENGINE_API bool Checkbox(const char *label, bool &v, const char *fmt = "%s");

        ENGINE_API bool InputUInt(const char *label, u32 &v, const char *fmt = "%s");
        ENGINE_API bool InputInt(const char *label, int &v, const char *fmt = "%s");
        ENGINE_API bool InputInt2(const char *label, glm::ivec2 &v, const char *fmt = "%s");
        ENGINE_API bool InputInt3(const char *label, glm::ivec3 &v, const char *fmt = "%s");
        ENGINE_API bool InputInt4(const char *label, glm::ivec4 &v, const char *fmt = "%s");
        ENGINE_API bool InputFloat(const char *label, float &v, float step, const char *fmt = "%s");
        ENGINE_API bool InputFloat2(const char *label, glm::fvec2 &v, const char *fmt = "%s");
        ENGINE_API bool InputFloat3(const char *label, glm::fvec3 &v, const char *fmt = "%s");
        ENGINE_API bool InputFloat4(const char *label, glm::fvec4 &v, const char *fmt = "%s");

        ENGINE_API bool Vec2Control(
            const string &label, glm::vec2 &values,
            const array<string, 2>& tags = { "X", "Y" },
            float resetValue = 0.0f,
            float columnWidth = 100.0f
        );
        ENGINE_API bool Vec3Control(
            const string &label, glm::vec3 &values,
            const array<string, 3>& tags = { "X", "Y", "Z" },
            float resetValue = 0.0f,
            float columnWidth = 100.0f
        );
        ENGINE_API bool Vec4Control(
            const string &label, glm::vec4 &values,
            const array<string, 4>& tags = { "X", "Y", "Z", "W" },
            float resetValue = 0.0f,
            float columnWidth = 100.0f
        );

        template<typename T, typename UIFunction>
        void ComponentControl(const string& name, cEntity& entity, UIFunction uiFunction);

        ENGINE_API void TransformControl(CTransform& transform);

        ENGINE_API bool Color3Control(
            const string &label, glm::vec3 &values,
            float resetValue = 0.0f,
            float columnWidth = 100.0f
        );
        ENGINE_API bool Color4Control(
            const string &label, glm::vec4 &values,
            float resetValue = 0.0f,
            float columnWidth = 100.0f
        );

        ENGINE_API bool ColorEdit3(const char* label, glm::vec3& values, const char* fmt = "%s");
        ENGINE_API bool ColorEdit4(const char* label, glm::vec4& values, const char* fmt = "%s");

        ENGINE_API bool ColorIntensiveEdit3(
                const string &label,
                glm::vec3& color,
                glm::vec3& intensity,
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        ENGINE_API bool InputText(string& text);

        ENGINE_API void InputFont(sFont* font, const vector<sFont*>& allFonts);

        ENGINE_API bool IconRadioButton(const char* id, const char* icon, bool& checked, const ImVec2& size = { 24, 24 }, const float cornerRadius = 2.0f);

        ENGINE_API bool IconButton(
            const char* id,
            const char* icon,
            const ImVec2& size = { 24, 24 },
            const float cornerRadius = 2.0f,
            const u32 baseColor = Color::CORNFLOWER_BLUE,
            const u32 hoverColor = Color::JORDY_BLUE,
            const u32 activeColor = Color::LAVENDER_GREY,
            const u32 textColor = Color::LAVENDER_GREY
        );

        ENGINE_API void Spacing(float width, float height);

        ENGINE_API void FullscreenMode();

        ENGINE_API void WindowMode();

        ENGINE_API bool SliderFloat(const char* label, float& value, const float min, const float max);
        ENGINE_API bool SliderInt(const char* label, int& value, const int min, const int max);

        ENGINE_API void TextCentered(const char *text, float maxWidth, int lineId, bool separator);

        ENGINE_API void Line(const string& id);

        template<typename T, typename UIFunction>
        void ComponentControl(const string& name, cEntity& entity, UIFunction uiFunction)
        {
            ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
            treeNodeFlags |= ImGuiTreeNodeFlags_Framed;
            treeNodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;
            treeNodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;
            treeNodeFlags |= ImGuiTreeNodeFlags_FramePadding;

            if (entity.HasAny<T>())
            {
                auto& component = entity.Get<T>();
                ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

                ImGui::Separator();
                bool open = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags);

                ImGui::PopStyleVar();

                ImGui::SameLine(contentRegionAvailable.x - 24);
                if (IconButton("##component_settings", "+"))
                {
                    ImGui::OpenPopup("ComponentSettings");
                }

                bool removeComponent = false;
                if (ImGui::BeginPopup("ComponentSettings"))
                {
                    if (ImGui::MenuItem("Remove"))
                    {
                        removeComponent = true;
                    }
                    ImGui::EndPopup();
                }

                if (open)
                {
                    uiFunction(component);
                    ImGui::TreePop();
                }

                if (removeComponent)
                {
                    entity.Remove<T>();
                }
            }
        }
    }
}