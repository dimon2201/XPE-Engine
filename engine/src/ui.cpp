#include <ui.hpp>

#include <render_context.hpp>

#define GLFW_INCLUDE_NONE
#include <glfw3.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_dx11.h>

namespace xpe
{
    namespace ui
    {
        ImGuiIO* IO;
        ImFont* RegularFont = nullptr;
        ImFont* BoldFont = nullptr;
        bool IsFrameResized = false;
        ImGuiID DockspaceId;
        ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_None;

        static GLFWwindow* s_Window = nullptr;

        void Init()
        {
            // Setup context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            // Setup input configs
            IO = &ImGui::GetIO();
            IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking Space
            IO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Viewports
            IO->ConfigDockingWithShift = false;

            // Setup backends
            s_Window = (GLFWwindow*) MWindow::GetInstance();
#ifdef DX11
            ImGui_ImplGlfw_InitForOther(s_Window, true);
            ImGui_ImplDX11_Init((ID3D11Device*) render::context::GetDevice(), (ID3D11DeviceContext*) render::context::GetContext());
#endif
        }

        void Free()
        {
#ifdef DX11
            ImGui_ImplDX11_Shutdown();
#endif
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        void SetDarkTheme()
        {
            auto& colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_WindowBg] = Style::k_WindowBackground;

            // Headers
            colors[ImGuiCol_Header] = Style::k_Header;
            colors[ImGuiCol_HeaderHovered] = Style::k_HeaderHover;
            colors[ImGuiCol_HeaderActive] = Style::k_HeaderActive;

            // Buttons
            colors[ImGuiCol_Button] = Style::k_Btn;
            colors[ImGuiCol_ButtonHovered] = Style::k_BtnHover;
            colors[ImGuiCol_ButtonActive] = Style::k_BtnActive;

            // Frame BG
            colors[ImGuiCol_FrameBg] = Style::k_FrameBackground;
            colors[ImGuiCol_FrameBgHovered] = Style::k_FrameBackgroundHover;
            colors[ImGuiCol_FrameBgActive] = Style::k_FrameBackgroundActive;

            // Tabs
            colors[ImGuiCol_Tab] = Style::k_Tab;
            colors[ImGuiCol_TabHovered] = Style::k_TabHover;
            colors[ImGuiCol_TabActive] = Style::k_TabActive;
            colors[ImGuiCol_TabUnfocused] = Style::k_TabUnfocused;
            colors[ImGuiCol_TabUnfocusedActive] = Style::k_TabUnfocusedActive;

            // Title
            colors[ImGuiCol_TitleBg] = Style::k_TitleBackground;
            colors[ImGuiCol_TitleBgActive] = Style::k_TitleBackgroundActive;
            colors[ImGuiCol_TitleBgCollapsed] = Style::k_TitleBackgroundCollapsed;

            // Menu Bar
            colors[ImGuiCol_MenuBarBg] = Style::k_MenubarBackground;

            // Popup
            colors[ImGuiCol_PopupBg] = Style::k_PopupBackground;

            // Scrollbar
            colors[ImGuiCol_ScrollbarBg] = Style::k_ScrollbarBackground;
        }

        void BeginFrame()
        {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();
        }

        void EndFrame()
        {
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            GLFWwindow* backupWindow = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backupWindow);
        }

        void AddRegularFont(const char* filepath, float size)
        {
            RegularFont = IO->Fonts->AddFontFromFileTTF(filepath, size);
        }

        void AddBoldFont(const char* filepath, float size)
        {
            BoldFont = IO->Fonts->AddFontFromFileTTF(filepath, size);
        }

        void AddIconFont(const char* filepath, float size)
        {
            static const ImWchar iconsRange[] = { ICON_MIN_CI, ICON_MAX_CI, 0 };

            ImFontConfig iconsConfig;
            iconsConfig.MergeMode = true;
            iconsConfig.PixelSnapH = true;

            IO->Fonts->AddFontFromFileTTF(filepath, size, &iconsConfig, iconsRange);
        }

        void SetFont(ImFont *font)
        {
            IO->FontDefault = font;
        }

        void SetIniFilename(const char* iniFilename)
        {
            IO->IniFilename = iniFilename;
        }

        string GetID(const vector<const char*> &str)
        {
            stringstream ss;
            ss << "##";
            for (auto& s : str)
            {
                ss << s;
            }
            return ss.str();
        }

        static bool s_EnableInput = true;

        void EnableInput()
        {
            if (s_EnableInput)
            {
                return;
            }

            s_EnableInput = true;

            IO->ConfigFlags &= ~ImGuiConfigFlags_NoMouse;          // Enable Mouse
            IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard
        }

        void DisableInput()
        {
            if (!s_EnableInput)
            {
                return;
            }

            s_EnableInput = false;

            IO->ConfigFlags |= ImGuiConfigFlags_NoMouse;            // Disable Mouse
            IO->ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard; // Disable Keyboard
        }

        bool Checkbox(const char* label, bool &v, const char* fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::Checkbox(K_UID(label), &v);
        }

        bool InputUInt(const char* label, u32 &v, const char* fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputInt(K_UID(label), (int*) &v);
        }

        bool InputInt(const char* label, int &v, const char* fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputInt(K_UID(label), &v);
        }

        bool InputInt2(const char* label, glm::ivec2 &v, const char *fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputInt2(K_UID(label), glm::value_ptr(v));
        }

        bool InputInt3(const char* label, glm::ivec3 &v, const char *fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputInt3(K_UID(label), glm::value_ptr(v));
        }

        bool InputInt4(const char* label, glm::ivec4 &v, const char *fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputInt4(K_UID(label), glm::value_ptr(v));
        }

        bool InputFloat(const char* label, float &v, float step, const char *fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();

            bool updated = ImGui::InputFloat(K_UID(label), &v);
            ImGui::SameLine();

            if (ImGui::ArrowButton(K_UID(label, "_LeftArrow"), ImGuiDir_Left))
            {
                v -= step;
                updated = true;
            }
            ImGui::SameLine();

            if (ImGui::ArrowButton(K_UID(label, "_RightArrow"), ImGuiDir_Right))
            {
                v += step;
                updated = true;
            }

            return updated;
        }

        bool InputFloat2(const char* label, glm::fvec2 &v, const char *fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputFloat2(K_UID(label), glm::value_ptr(v));
        }

        bool InputFloat3(const char* label, glm::fvec3 &v, const char *fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputFloat3(K_UID(label), glm::value_ptr(v));
        }

        bool InputFloat4(const char* label, glm::fvec4 &v, const char *fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::InputFloat4(K_UID(label), glm::value_ptr(v));
        }

        bool Vec2Control(
                const string& label, glm::vec2& values,
                const array<string, 2>& tags,
                float resetValue, float columnWidth
        )
        {
            bool press = false;
            glm::vec2 oldValue = values;

            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label.c_str());
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[0].c_str(), buttonSize))
            {
                values.x = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##X", label.c_str()), &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[1].c_str(), buttonSize))
            {
                values.y = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##Y", label.c_str()), &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();

            ImGui::Columns(1);

            ImGui::PopID();

            return press || (oldValue != values);
        }

        bool Vec3Control(
                const string& label, glm::vec3& values,
                const array<string, 3>& tags,
                float resetValue, float columnWidth
        )
        {
            bool press = false;
            glm::vec3 oldValue = values;

            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label.c_str());
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[0].c_str(), buttonSize))
            {
                values.x = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##X", label.c_str()), &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[1].c_str(), buttonSize))
            {
                values.y = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##Y", label.c_str()), &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[2].c_str(), buttonSize))
            {
                values.z = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##Z", label.c_str()), &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();

            ImGui::Columns(1);

            ImGui::PopID();

            return press || (oldValue != values);
        }

        bool Vec4Control(
                const string& label, glm::vec4& values,
                const array<string, 4>& tags,
                float resetValue, float columnWidth
        )
        {
            bool press = false;
            glm::vec4 oldValue = values;

            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label.c_str());
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[0].c_str(), buttonSize))
            {
                values.x = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##X", label.c_str()), &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[1].c_str(), buttonSize))
            {
                values.y = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##Y", label.c_str()), &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[2].c_str(), buttonSize))
            {
                values.z = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##Z", label.c_str()), &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.7f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(tags[3].c_str(), buttonSize))
            {
                values.w = resetValue;
                press = true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat(K_UID("##W", label.c_str()), &values.w, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();

            ImGui::Columns(1);

            ImGui::PopID();

            return press || (oldValue != values);
        }

        void TransformControl(CTransform& transform)
        {
            Vec3Control("Position", transform.Position);
            glm::vec3 rotation = glm::degrees(transform.Rotation);
            Vec3Control("Rotation", rotation);
            transform.Rotation = glm::radians(rotation);
            Vec3Control("Scale", transform.Scale, { "X", "Y", "Z" }, 1.0f);
        }

        bool Color3Control(
                const string &label, glm::vec3 &values,
                float resetValue, float columnWidth
        )
        {
            return Vec3Control(label, values, { "R", "G", "B" }, resetValue, columnWidth);
        }

        bool Color4Control(
                const string &label, glm::vec4 &values,
                float resetValue, float columnWidth
        )
        {
            return Vec4Control(label, values, { "R", "G", "B", "A" }, resetValue, columnWidth);
        }

        bool ColorEdit3(const char* label, glm::vec3 &values, const char* fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::ColorEdit3(K_UID(label), glm::value_ptr(values));
        }

        bool ColorEdit4(const char* label, glm::vec4 &values, const char* fmt)
        {
            ImGui::Text(fmt, label);
            ImGui::SameLine();
            return ImGui::ColorEdit4(K_UID(label), glm::value_ptr(values));
        }

        bool ColorIntensiveEdit3(
            const string &label,
            glm::vec3& color,
            glm::vec3& intensity,
            float resetValue,
            float columnWidth
        )
        {
            ImGui::SeparatorText("Color");
            bool colorChanged = Color3Control("", color, resetValue, columnWidth);
            ImGui::PushID(K_UID("##light_intensity_", label.c_str()));
            bool intensityChanged = ImGui::SliderFloat3("Intensity", glm::value_ptr(intensity), 0, 100);
            ImGui::PopID();
            return colorChanged || intensityChanged;
        }

        bool InputText(string& text)
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), text.c_str(), sizeof(buffer));

            if (ImGui::InputText("##tag", buffer, sizeof(buffer)))
            {
                text = string(buffer);
                return true;
            }

            return false;
        }

        void InputFont(sFont* font, const vector<sFont*>& allFonts)
        {}

        bool IconRadioButton(const char* id, const char* icon, bool& checked, const ImVec2& size, const float cornerRadius)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cornerRadius);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImU32) Color::LAVENDER_GREY);
            ImGui::PushID(id);

            if (checked)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImU32) Color::LAVENDER_GREY);
                ImGui::PushStyleColor(ImGuiCol_Text, (ImU32) Color::BLACK_PEARL);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImU32) Color::ALICE_BLUE);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImU32) Color::CORNFLOWER_BLUE);
                ImGui::PushStyleColor(ImGuiCol_Text, (ImU32) Color::LAVENDER_GREY);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImU32) Color::JORDY_BLUE);
            }

            bool pressed = ImGui::Button(icon, size);
            if (pressed)
            {
                checked = !checked;
            }

            ImGui::PopID();
            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();

            return pressed;
        }

        bool IconButton(
            const char* id,
            const char* icon,
            const ImVec2& size,
            const float cornerRadius,
            const u32 baseColor,
            const u32 hoverColor,
            const u32 activeColor,
            const u32 textColor
        )
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cornerRadius);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
            ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::PushID(id);

            bool pressed = ImGui::Button(icon, size);

            ImGui::PopID();
            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();

            return pressed;
        }

        void Spacing(float width, float height)
        {
            ImGui::Dummy({ width, height });
        }

        void WindowMode()
        {
            float width = static_cast<float>(MWindow::GetWidth());
            float height = static_cast<float>(MWindow::GetHeight());
            ImGui::SetWindowPos({0, 0 });
            ImGui::SetWindowSize({ width, height });
        }

        void FullscreenMode()
        {
            float width = static_cast<float>(MWindow::GetMonitorWidth());
            float height = static_cast<float>(MWindow::GetMonitorHeight());
            ImGui::SetWindowPos({0, 0 });
            ImGui::SetWindowSize({ width, height });
        }

        bool SliderFloat(const char *label, float &value, const float min, const float max)
        {
            ImGui::PushID(K_UID(label));
            bool result = ImGui::SliderFloat(label, &value, min, max);
            ImGui::PopID();
            return result;
        }

        bool SliderInt(const char *label, int &value, const int min, const int max)
        {
            ImGui::PushID(K_UID(label));
            bool result = ImGui::SliderInt(label, &value, min, max);
            ImGui::PopID();
            return result;
        }

        void TextCentered(const char *text, float maxWidth, int lineId, bool separator)
        {
            ImGui::Spacing();
            ImGui::SameLine((maxWidth / 2) - (ImGui::CalcTextSize(text).x / 2));
            ImGui::Text(text);
            ImGui::Spacing();
            if (separator)
            {
                Line(string(std::to_string(lineId)));
            }
        }

        void Line(const string& id)
        {
            string newId = "line__" + id;
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
            ImGui::BeginChild(newId.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 1), false);
            ImGui::Separator();
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
    }
}