#include <editor_manager.hpp>
#include <macro_icons.hpp>

#ifdef WINDOWS
#include <Windows.h>
#endif

namespace xpe
{
    namespace ui
    {
        enum class eFileDialogType
        {
            IMPORT,
            EXPORT
        };

        static string FileDialogTypeToString(eFileDialogType type)
        {
            switch (type)
            {
                case eFileDialogType::IMPORT: return "IMPORT";
                case eFileDialogType::EXPORT: return "EXPORT";
                default: return "UNKNOWN";
            }
        }

        static string OpenFileDialog(eFileDialogType type, const char* filter, const char* initialDir)
        {
            LogInfo("OpenFileDialog: type={}, filter={}, initialDir={}", FileDialogTypeToString(type), filter, initialDir);

            OPENFILENAMEA ofn;
            CHAR szFile[260] = { 0 };

            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = (HWND) MWindow::GetWin32Instance();
            ofn.lpstrFile = szFile;
            ofn.lpstrInitialDir = initialDir;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = filter;
            ofn.nFilterIndex = 1;

            if (type == eFileDialogType::IMPORT)
            {
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

                if (GetOpenFileName(&ofn) == TRUE)
                {
                    return ofn.lpstrFile;
                }
            }

            else
            {
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
                ofn.lpstrDefExt = strchr(filter, '\0') + 1;
                if (GetSaveFileName(&ofn) == TRUE)
                {
                    return ofn.lpstrFile;
                }
            }

            return "";
        }

        bool cImageWindow::Begin()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);

            if (!ImGui::Begin(Title.c_str(), &Show, WindowFlags))
            {
                End();
                return false;
            }

            if (!m_Initialized)
            {
                m_Initialized = true;
                ImGui::SetWindowPos({ Position.x, Position.y });
                ImGui::SetWindowSize({ Resolution.x, Resolution.y });
                ImVec2 frameSize = ImGui::GetContentRegionAvail();
                m_CurrentFrameSize = { frameSize.x, frameSize.y };
            }

            ImVec2 frameSize = ImGui::GetContentRegionAvail();
            if (ResizeFramebuffer && (m_CurrentFrameSize.x != frameSize.x || m_CurrentFrameSize.y != frameSize.y))
            {
                MEvent::WindowFrameResized.Notify(frameSize.x, frameSize.y);
            }
            m_CurrentFrameSize = { frameSize.x, frameSize.y };

            m_Focused = ImGui::IsWindowFocused();

            if (m_Focused)
            {
                ui::IO->WantCaptureMouse = false;
            }

//        ImguiCore::camera->moveImgui(Timer::getDeltaMillis());
//        ImguiCore::camera->onMouseCursorImgui(ImGui::GetCursorPosX(), ImGui::GetCursorPosY(), Timer::getDeltaMillis());
//
//        if (mScrollX != ImguiCore::IO->MouseWheelH)
//        {
//            mScrollX = ImguiCore::IO->MouseWheelH;
//        }
//
//        if (mScrollY != ImguiCore::IO->MouseWheel)
//        {
//            mScrollY = ImguiCore::IO->MouseWheel;
//            ImguiCore::camera->onMouseScroll(mScrollY, Timer::getDeltaMillis());
//        }

            return true;
        }

        void cImageWindow::End()
        {
            ImGui::PopStyleVar(2);
            ImGui::End();
        }

        void cImageWindow::Draw()
        {
            if (Image != nullptr)
            {
                ImGui::Image(
                        Image->ViewInstance,
                        { m_CurrentFrameSize.x, m_CurrentFrameSize.y },
                        { 1, 0 },
                        { 0, 1 }
                );
            }
        }

        cTexture* MEditor::ViewportImage = nullptr;
        bool MEditor::EnableSimulation = false;
        bool MEditor::EnableWorldMode = false;
        bool MEditor::EnableMoveMode = false;
        bool MEditor::EnableRotateMode = false;
        bool MEditor::EnableScaleMode = false;
        CPerspectiveCamera* MEditor::SelectedCamera = nullptr;
        cScene* MEditor::SelectedScene = nullptr;
        cEntity MEditor::SelectedEntity;

        static cTexture* s_Logo = nullptr;

        static bool s_DockspaceInitialized = false;
        static int s_DockspaceX = 0;
        static int s_DockspaceY = 0;
        static int s_DockspaceWidth = 800;
        static int s_DockspaceHeight = 600;

        static cImageWindow* s_ViewportWindow = nullptr;
        static bool s_ViewportInitialized = false;

        static glm::mat4 s_ViewMatrix;
        static glm::mat4 s_PerspectiveMatrix;

        static ImGuizmo::MODE s_GizmoMode;

        void MEditor::Init()
        {
            Profile();

            ui::Init();
            ui::SetDarkTheme();
            MWindow::SetTheme(eWindowTheme::DARK);
            MWindow::HideTitleBar();
            InitFileBrowser();
            s_Logo = MTexture::Load("logo", "editor_res/logo.png", eTextureFormat::RGBA8);
            s_ViewportWindow = new cImageWindow("Viewport", ViewportImage);
        }

        void MEditor::Free()
        {
            Profile();

            delete s_Logo;
            delete s_ViewportWindow;
            FreeFileBrowser();
            ui::Free();
        }

        void MEditor::Update(float dt)
        {
            Profile();

//            ui::BeginFrame();
            {
//                 static bool showDemo = true;
//                 ImGui::ShowDemoWindow(&showDemo);
//                BeginDockspace();
//                {
//                    if (ImGui::BeginMenuBar())
//                    {
//                        DrawLogo();
//                        DrawWindowButtons();
//                        ImGui::EndMenuBar();
//                    }
//                    s_ViewportWindow->Image = ViewportImage;
//                    DrawViewportWindow();
//                    DrawEntityWindow();
//                    DrawComponentWindow();
////                    DrawFilesWindow();
//                }
//                EndDockspace();
            }
//            ui::EndFrame();
        }

        void MEditor::BeginDockspace()
        {
            static bool show = true;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 16 });

            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
            windowFlags |= ImGuiWindowFlags_NoTitleBar;
            windowFlags |= ImGuiWindowFlags_NoCollapse;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            windowFlags |= ImGuiWindowFlags_NoNavFocus;
            windowFlags |= ImGuiWindowFlags_NoBackground;
            windowFlags |= ImGuiWindowFlags_MenuBar;

            ImGui::Begin("Dockspace", &show, windowFlags);
        }

        void MEditor::EndDockspace()
        {
            ImGuiID dsId = ImGui::GetID("Dashboard");
            ui::DockspaceId = dsId;
            ImGui::DockSpace(dsId, { 0, 0 }, ui::DockspaceFlags);

            ImVec2 pos = ImGui::GetWindowPos();
            int width = static_cast<int>(ImGui::GetWindowWidth());
            int height = static_cast<int>(ImGui::GetWindowHeight());
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);

            if (x != s_DockspaceX || y != s_DockspaceY)
            {
                s_DockspaceX = x;
                s_DockspaceY = y;
                MWindow::SetPos(x, y);
            }

            if (width != s_DockspaceWidth || height != s_DockspaceHeight)
            {
                s_DockspaceWidth = width;
                s_DockspaceHeight = height;
                MWindow::SetSize(width, height);
            }

            ImGui::PopStyleVar(4);
            ImGui::End();
        }

        void MEditor::DrawLogo()
        {
            if (s_Logo != nullptr)
            {
                ImVec2 size = { static_cast<float>(s_Logo->Width), static_cast<float>(s_Logo->Height) };
                ImGui::Image(s_Logo->ViewInstance, size, { 0, 1 }, { 1, 0 });
            }
        }

        void MEditor::DrawWindowButtons()
        {
            ui::Spacing(ImGui::GetWindowWidth() - 400, 1);

            if (ui::IconButton("##minimize_button", "_", { 72, 36 }, 0, Style::k_MenubarBackground, Style::k_FrameBackgroundHover))
            {
                MWindow::Minimize();
            }

            ImGui::SameLine(0, 0);

            static bool fullscreen = false;
            if (ui::IconButton("##resize_button", ICON_CI_SCREEN_FULL, { 72, 36 }, 0, Style::k_MenubarBackground, Style::k_FrameBackgroundHover))
            {
                fullscreen = !fullscreen;
                if (fullscreen)
                {
                    MWindow::SetFullscreenWindowed();
                }
                else
                {
                    MWindow::SetWindowed();
                }
            }

            ImGui::SameLine(0, 0);

            if (ui::IconButton("##close_button", ICON_CI_CLOSE, { 72, 36 }, 0, Color::TALL_POPPY, Color::CHESNUT_ROSE))
            {
                MWindow::Close();
            }
        }

        void MEditor::DrawViewportWindow()
        {
            if (!s_ViewportInitialized)
            {
                s_ViewportInitialized = true;
                s_ViewportWindow->WindowFlags |= ImGuiWindowFlags_NoTitleBar;
                s_ViewportWindow->WindowFlags |= ImGuiWindowFlags_NoScrollbar;
                s_ViewportWindow->WindowFlags |= ImGuiWindowFlags_NoCollapse;
                s_ViewportWindow->WindowFlags |= ImGuiWindowFlags_DockNodeHost;
                s_ViewportWindow->Show = true;
                s_ViewportWindow->ResizeFramebuffer = true;
                ImGui::SetNextWindowDockID(ui::DockspaceId);
            }

            if (s_ViewportWindow->Begin())
            {
                DrawGizmoTab();

                s_ViewportWindow->Draw();

                if (SelectedEntity.IsValid())
                {
                    DrawGizmo();
                }

                s_ViewportWindow->End();
            }
        }

        void MEditor::DrawGizmoTab()
        {
            ui::Spacing(ImGui::GetWindowWidth(), 2);
            ui::Spacing(ImGui::GetWindowWidth() * 0.5f - 100.0f, 0);
            ImGui::SameLine();

            ui::IconRadioButton("##world_space", ICON_CI_GLOBE, EnableWorldMode);

            ImGui::SameLine(0, 4);

            ui::IconRadioButton("##translate_mode", ICON_CI_MOVE, EnableMoveMode);

            ImGui::SameLine(0, 4);

            ui::IconRadioButton("##rotate_mode", ICON_CI_CIRCLE_LARGE_OUTLINE, EnableRotateMode);

            ImGui::SameLine(0, 4);

            ui::IconRadioButton("##scale_mode", ICON_CI_ARROW_SWAP, EnableScaleMode);

            ImGui::SameLine(0, 4);

            static bool enablePlay = false;
            if (enablePlay)
            {
                ui::IconRadioButton("##pause", ICON_CI_DEBUG_PAUSE, enablePlay);
                EnableSimulation = true;
            }
            else
            {
                ui::IconRadioButton("##play", ICON_CI_PLAY, enablePlay);
                EnableSimulation = false;
            }

            ImGui::SameLine(0, 4);

            if (ui::IconButton("##compile", ICON_CI_SYNC))
            {
                // compile script code
            }

            ImGui::SameLine(0, 4);

            static bool fullscreen = false;
            if (ui::IconButton("##fullscreen", ICON_CI_SCREEN_FULL))
            {
                fullscreen = !fullscreen;
                if (fullscreen)
                {
                    ui::FullscreenMode();
                }
                else
                {
                    ui::WindowMode();
                }
            }
        }

        void MEditor::DrawGizmo()
        {
            ImGuiIO& io = *ui::IO;

            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            float width = ImGui::GetWindowWidth();
            float height = ImGui::GetWindowHeight();
            ImVec2 position = ImGui::GetWindowPos();
            ImGuizmo::SetRect(position.x, position.y, width, height);

            s_GizmoMode = EnableWorldMode ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

            if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
            {
                ui::DisableInput();
            }
            else
            {
                ui::EnableInput();
            }

            cEntity& entity = SelectedEntity;
            CPerspectiveCamera* camera = SelectedCamera;

            if (entity.HasAny<CTransform>())
            {
                CTransform& transform = entity.Get<CTransform>();

                sViewMatrix viewMatrix;
                viewMatrix.Position = SelectedCamera->Position;
                viewMatrix.Up = SelectedCamera->Up;
                viewMatrix.Front = SelectedCamera->Front;

                static const float* perspective = glm::value_ptr(MMath::UpdatePerspectiveMatrix(SelectedCamera->Projection));
                static const float* view = glm::value_ptr(MMath::UpdateViewMatrix(viewMatrix));
                static float* transformPtr = &MMath::UpdateModelMatrix(transform)[0][0];
                static float* positionPtr = &transform.Position.x;
                static float* rotationPtr = &transform.Rotation.x;
                static float* scalePtr = &transform.Scale.x;

                if (EnableMoveMode)
                {
                    ImGuizmo::Manipulate(
                            view,
                            perspective,
                            ImGuizmo::TRANSLATE,
                            s_GizmoMode,
                            transformPtr
                    );
                }

                if (EnableRotateMode)
                {
                    ImGuizmo::Manipulate(
                            view,
                            perspective,
                            ImGuizmo::ROTATE,
                            s_GizmoMode,
                            transformPtr
                    );
                }

                if (EnableScaleMode)
                {
                    ImGuizmo::Manipulate(
                            view,
                            perspective,
                            ImGuizmo::SCALE,
                            s_GizmoMode,
                            transformPtr
                    );
                }

                ImGuizmo::DecomposeMatrixToComponents(
                        transformPtr,
                        positionPtr,
                        rotationPtr,
                        scalePtr
                );
            }
        }

        void MEditor::DrawEntityWindow()
        {
            static bool open = false;
            static bool initialized = false;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

            if (!ImGui::Begin("Entities", &open, ImGuiWindowFlags_None))
            {
                ImGui::PopStyleVar(2);
                ImGui::End();
                return;
            }

            if (!initialized)
            {
                initialized = true;
                ImGui::SetWindowPos({ 0, 0 });
                ImGui::SetWindowSize({ 256, 256 });
            }

            if (SelectedScene)
            {
                auto& entities = SelectedScene->GetEntities();
                for (EntityID entityId : entities)
                {
                    if (entityId != EntityNull)
                    {
                        DrawEntity(entityId, SelectedScene);
                    }
                }

                if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                {
                    SelectedEntity = {};
                }

                // Right-click on blank space
                if (ImGui::BeginPopupContextWindow("EntityPopupMenu"))
                {
                    if (ImGui::MenuItem("Create Entity"))
                    {
                        SelectedEntity = cEntity("Untitled", SelectedScene);
                    }
                    ImGui::EndPopup();
                }
            }

            ImGui::PopStyleVar(2);
            ImGui::End();
        }

        void MEditor::DrawEntity(EntityID entityId, cScene* scene)
        {
            cEntity entity = cEntity(entityId, scene);

            auto& tag = entity.GetTag();

            ImGuiTreeNodeFlags flags = SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0;
            flags |= ImGuiTreeNodeFlags_OpenOnArrow;
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
            flags |= ImGuiTreeNodeFlags_Leaf;

            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.GetID(), flags, "%s", tag.c_str());
            if (ImGui::IsItemClicked())
            {
                SelectedEntity = cEntity(entityId, SelectedScene);
            }

            bool entityDeleted = false;
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete Entity"))
                {
                    entityDeleted = true;
                }
                ImGui::EndPopup();
            }

            if (opened)
            {
                ImGuiTreeNodeFlags childFlags;
                childFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;
                childFlags |= ImGuiTreeNodeFlags_Leaf;

                bool childOpened = ImGui::TreeNodeEx((void*)9817239, flags, "%s", tag.c_str());
                if (childOpened)
                {
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            if (entityDeleted)
            {
                if (SelectedEntity == entity)
                {
                    SelectedEntity = {};
                }
                SelectedScene->RemoveEntity(entity.GetID());
            }
        }

        void MEditor::DrawComponentWindow()
        {
            static bool open = false;
            static bool initialized = false;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

            if (!ImGui::Begin("Components", &open, ImGuiWindowFlags_None))
            {
                ImGui::PopStyleVar(2);
                ImGui::End();
                return;
            }

            if (!initialized)
            {
                initialized = true;
                ImGui::SetWindowPos({ 0, 0 });
                ImGui::SetWindowSize({ 256, 256 });
            }

            if (SelectedEntity.IsValid())
            {
                DrawComponents();
            }

            ImGui::PopStyleVar(2);
            ImGui::End();
        }

        template<typename T>
        void DrawAddComponent(cEntity& entity, const char* name)
        {
            if (!entity.HasAny<T>())
            {
                if (ImGui::MenuItem(name))
                {
                    entity.Add<T>();
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        void MEditor::DrawComponents()
        {
            ui::InputText(SelectedEntity.GetTag());

            ImGui::SameLine();

            if (ui::IconButton("##add_component", "+"))
            {
                ImGui::OpenPopup("AddComponent");
            }

            if (ImGui::BeginPopup("AddComponent"))
            {
                DrawAddComponent<CTransform>(SelectedEntity, "Transform");
                DrawAddComponent<CDirectionalLight>(SelectedEntity, "DirectionalLight");
                DrawAddComponent<CPointLight>(SelectedEntity, "PointLight");
                DrawAddComponent<CSpotLight>(SelectedEntity, "SpotLight");
                DrawAddComponent<CLabel>(SelectedEntity, "Label");
                ImGui::EndPopup();
            }

            // CTransform
            {
                ui::ComponentControl<CTransform>("Transform", SelectedEntity, [](CTransform& component)
                {
                    ui::TransformControl(component);
                });
            }

            // CDirectionalLight
            {
                ui::ComponentControl<CDirectionalLight>("DirectionalLight", SelectedEntity, [](CDirectionalLight& component)
                {
                    ui::Vec3Control("Direction", component.View.Position);
                    ui::ColorIntensiveEdit3("Color", component.Color, component.Intensity);
                });
            }

            // CPointLight
            {
                ui::ComponentControl<CPointLight>("PointLight", SelectedEntity, [](CPointLight& component)
                {
                    ui::Vec3Control("Position", component.Position);
                    ui::ColorIntensiveEdit3("PointLightColor", component.Color, component.Intensity);
                    ui::InputFloat("Constant", component.Constant, 0.1f);
                    ui::InputFloat("Linear", component.Linear, 0.1f);
                    ui::InputFloat("Quadratic", component.Quadratic, 0.1f);
                });
            }

            // CSpotLight
            {
                ui::ComponentControl<CSpotLight>("SpotLight", SelectedEntity, [](CSpotLight& component)
                {
                    ui::Vec3Control("Position", component.View.Position);
                    ui::Vec3Control("Direction", component.View.Front);
                    ui::ColorIntensiveEdit3("Color", component.Color, component.Intensity);
                    ui::InputFloat("CutOff", component.InnerCutoff, 0.1f);
                    ui::InputFloat("OuterCutoff CutOff", component.OuterCutoff, 0.1f);
                });
            }

            // CLabel
            {
                ui::ComponentControl<CLabel>("Label", SelectedEntity, [](CLabel& component)
                {
                    ImGui::SeparatorText("Text");
                    ui::InputText(component.Text);
                    ImGui::SeparatorText("Font");
//                    ui::InputFont(component.Font);
                    ImGui::SeparatorText("Transform");
                    ui::TransformControl(SelectedEntity.Get<CTransform>());
                });
            }
        }

        static cTexture* s_DirIcon = nullptr;
        static cTexture* s_FileIcon = nullptr;
        static constexpr const char* k_DragDropFile = "FILE_DRAG_DROP";

        static void ImportFile(const fs::path& dir)
        {
            std::string dirStr = dir.string();
            string importPath = OpenFileDialog(eFileDialogType::IMPORT, nullptr, dirStr.c_str());
            if (importPath.empty())
            {
                LogInfo("ImportFile: Canceled!");
                return;
            }

            fs::path importFilepath(importPath);
            auto importFileName = importFilepath.filename().string();
            fs::path fileDirpath(dir);
            fileDirpath /= importFileName;
            LogInfo("ImportFile: ", importPath);
            fs::copy(importPath, fileDirpath.string());
        }

        static void ExportFile(const fs::path& filepath)
        {
            std::string filepathStr = filepath.string();
            string exportPath = OpenFileDialog(eFileDialogType::EXPORT, nullptr, filepathStr.c_str());
            if (exportPath.empty())
            {
                LogInfo("ExportFile: Canceled!");
                return;
            }

            fs::copy(filepath, exportPath);
        }

        void MEditor::InitFileBrowser()
        {
            s_DirIcon = MTexture::Load("DirIcon", "editor_res/dir_icon.png", eTextureFormat::RGBA8);
            s_FileIcon = MTexture::Load("FileIcon", "editor_res/file_icon.png", eTextureFormat::RGBA8);
        }

        void MEditor::FreeFileBrowser()
        {
            delete s_DirIcon;
            delete s_FileIcon;
        }

        void MEditor::DrawFilesWindow()
        {
            static float filePadding = 16.0f;
            static float fileThumbnailSize = 64.0f;
            static fs::path rootPath = "";
            static fs::path currentDir = "res";
            static fs::path rightClickedDir;
            static fs::path rightClickedFilepath;
            static string rightClickedFileExt;
            static string rightClickedDirExt;
            static string oldFilename = "";
            static string newFilename = "";
            static bool renameMode = false;
            static bool open = true;

            if (!ImGui::Begin(ICON_CI_FOLDER" Files", &open))
            {
                ImGui::End();
                return;
            }

            // Back button
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
            if (ImGui::Button(ICON_CI_ARROW_CIRCLE_LEFT))
            {
                currentDir = currentDir.parent_path();
            }
            ImGui::PopStyleVar();
            ImGui::SameLine();

            // Import file
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
            if (ImGui::ButtonEx(ICON_CI_ARROW_CIRCLE_DOWN" Import", { 72, 24 }))
            {
            }
            ImGui::PopStyleVar();
            ImGui::Separator();

            // Export file
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
            if (ImGui::ButtonEx(ICON_CI_ARROW_CIRCLE_UP" Export", { 72, 24 }))
            {
            }
            ImGui::PopStyleVar();
            ImGui::Separator();

            float cellSize = fileThumbnailSize + filePadding;
            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int)(panelWidth / cellSize);
            if (columnCount < 1)
            {
                columnCount = 1;
            }

            ImGui::Columns(columnCount, nullptr, false);

            std::string currentDirStr = currentDir.string();
            LogInfo("FileBrowser: current dir {}", currentDirStr);
            if (fs::exists(currentDir))
            {
                for (auto& dir : fs::directory_iterator(currentDir))
                {
                    const auto& path = dir.path();
                    auto relativePath = fs::relative(path, rootPath);
                    const auto& fileName = relativePath.filename().string();
                    const auto& fileExtension = relativePath.extension().string();
                    bool isDirectory = dir.is_directory();

                    std::string pathStr = path.string();
                    LogInfo("FileBrowser: file {}, relative path {}", fileName, pathStr);

                    auto fileId = fileName.c_str();
                    cTexture* fileIcon = isDirectory ? s_DirIcon : s_FileIcon;

                    ImGui::PushID(fileId);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    auto iconClicked = ImGui::ImageButton(
                            fileIcon->ViewInstance,
                            { fileThumbnailSize, fileThumbnailSize },
                            { 0, 0 },
                            { 1, 1 },
                            -static_cast<int>(filePadding)
                    );

                    // send drag-drop item path
                    if (ImGui::BeginDragDropSource())
                    {
                        const wchar_t* itemPath = relativePath.c_str();
                        std::wstring itemPathWhiteStr(itemPath);
                        std::string itemPathStr = { itemPathWhiteStr.begin(), itemPathWhiteStr.end() };
                        ImGui::SetDragDropPayload(k_DragDropFile, itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                        LogInfo("FileBrowser: Sending drag-drop file {}", itemPathStr);
                        ImGui::EndDragDropSource();
                    }

                    ImGui::PopStyleColor();

                    if (isDirectory && ImGui::IsItemClicked(ButtonLeft))
                    {
                        currentDir /= path.filename();
                    }

                    if (ImGui::IsItemClicked(ButtonRight))
                    {
                        if (isDirectory)
                        {
                            rightClickedDir = path;
                            rightClickedDirExt = fileExtension;
                            rightClickedFilepath = "";
                            rightClickedFileExt = "";
                        }
                        else
                        {
                            rightClickedFilepath = path;
                            rightClickedFileExt = fileExtension;
                        }
                    }

                    if (renameMode)
                    {
                        ImGui::InputText("##file_rename", newFilename.data(), newFilename.length());
                        if (ImGui::IsKeyPressed(ImGuiKey_Enter))
                        {
                            fs::rename(rightClickedFilepath, newFilename + rightClickedFileExt);
                            renameMode = false;
                            rightClickedFilepath = "";
                        }
                    }
                    else
                    {
                        ui::TextCentered(fileName.c_str(), fileThumbnailSize * 1.5, 1, false);
                    }

                    ImGui::NextColumn();
                    ImGui::PopID();
                }
            }

            if (!rightClickedFilepath.empty())
            {
                // Draw File Popup
                if (ImGui::BeginPopupContextWindow(nullptr))
                {
                    if (ImGui::MenuItem("Rename"))
                    {
                        renameMode = true;
                        std::string oldStr = rightClickedFilepath.string();
                        oldFilename = MFile::GetFileName(oldStr.c_str());
                        newFilename = oldFilename;
                    }

                    if (ImGui::MenuItem("Import"))
                    {
                        ImportFile(rightClickedFilepath);
                        rightClickedFilepath = "";
                    }

                    if (ImGui::MenuItem("Export"))
                    {
                        ExportFile(rightClickedFilepath);
                        rightClickedFilepath = "";
                    }

                    if (ImGui::MenuItem("Delete"))
                    {
                        fs::remove(rightClickedFilepath);
                        rightClickedFilepath = "";
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::Columns(1);
            ImGui::End();
        }
    }
}