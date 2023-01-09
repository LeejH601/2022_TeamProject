#include "ImGuiManager.h"

#define NUM_FRAMES_IN_FLIGHT 3

CImGuiManager::CImGuiManager()
{
}

CImGuiManager::~CImGuiManager()
{
    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CImGuiManager::CreateSrvDescriptorHeaps(ID3D12Device* pd3dDevice)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = 1;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dSrvDescHeap));
}

void CImGuiManager::Init(HWND hWnd, ID3D12Device* pd3dDevice)
{
    CreateSrvDescriptorHeaps(pd3dDevice);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX12_Init(pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, m_pd3dSrvDescHeap.Get(),
        m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
}

void CImGuiManager::OnPreRender()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);
    //// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    //{
    //    static float f = 0.0f;
    //    static int counter = 0;

    //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    //    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    //    ImGui::Checkbox("Another Window", &show_another_window);
    //    ImGui::Checkbox("My Window", &show_my_window);
    //    
    //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //        counter++;
    //    ImGui::SameLine();
    //    ImGui::Text("counter = %d", counter);

    //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //    ImGui::End();
    //}

    //// 3. Show another simple window.
    //if (show_another_window)
    //{
    //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    //    ImGui::Text("Hello from another window!");
    //    if (ImGui::Button("Close Me"))
    //        show_another_window = false;
    //    ImGui::End();
    //}

    // Show my window.
    {
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 25, main_viewport->WorkPos.y + 25));
        ImGui::SetNextWindowSize(ImVec2(1280, 680), ImGuiCond_FirstUseEver);
        ImGui::Begin("My Window");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Start My Window!");
        ImGui::SetNextItemWidth(225.f);
        ImGui::SetCursorPosX(855.f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        ImGui::SetNextItemWidth(180.f);
        ImGui::SetCursorPosX(855.f);
        ImGui::InputInt("Animation Set Num", &Player_Animation_Number, 1, 1, 0);

        //ImGui::ColorEdit3("clear color", (float*)&clear_color, ImGuiColorEditFlags_NoInputs);
        //ImGui::ColorEdit3("clear color", (float*)&clear_color, ImGuiColorEditFlags_NoLabel);
        ImVec2 curpos = ImGui::GetCursorPos();
        ImGui::SetCursorPosY(curpos.y - 25.f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}

void CImGuiManager::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
    // Rendering
    ImGui::Render();

    pd3dCommandList->SetDescriptorHeaps(1, m_pd3dSrvDescHeap.GetAddressOf());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pd3dCommandList);
}

