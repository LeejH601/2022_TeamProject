#include "ImGuiManager.h"
#include "..\Object\Texture.h"
#include "..\Global\GameFramework.h"

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
    D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
    d3dDescriptorHeapDesc.NumDescriptors = 3;
    d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    d3dDescriptorHeapDesc.NodeMask = 0;
    pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dSrvDescHeap.GetAddressOf());

    m_d3dSrvCPUDescriptorStartHandle = m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
    m_d3dSrvGPUDescriptorStartHandle = m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();

    m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
    m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
}
void CImGuiManager::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex)
{
    m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
    m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

    int nTextures = pTexture->GetTextures();
    UINT nTextureType = pTexture->GetTextureType();
    for (int i = 0; i < nTextures; i++)
    {
        ID3D12Resource* pShaderResource = pTexture->GetResource(i);
        D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
        pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
        m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
        pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
        m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
    }
}
ID3D12Resource* CImGuiManager::GetRTTextureResource() { return m_pRTTexture->GetResource(0); }
void CImGuiManager::Init(HWND hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle)
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

    m_pTexture = std::make_unique<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
    m_pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);
    CreateShaderResourceViews(pd3dDevice, m_pTexture.get(), 1);

    m_pRTTexture = std::make_unique<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
    D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.0f, 0.0f, 1.0f } };
    m_pRTTexture->CreateTexture(pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue, RESOURCE_TEXTURE2D, 0);
    CreateShaderResourceViews(pd3dDevice, m_pRTTexture.get(), 0);

    D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
    d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
    d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

    ID3D12Resource* pd3dTextureResource = m_pRTTexture->GetResource(0);
    pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
    m_pd3dRtvCPUDescriptorHandles = d3dRtvCPUDescriptorHandle;

}
void CImGuiManager::OnPreRender()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

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
        //ImGui::ColorEdit3("clear color", (float*)&clear_color, ImGuiColorEditFlags_NoInputs);
        //ImGui::ColorEdit3("clear color", (float*)&clear_color, ImGuiColorEditFlags_NoLabel);
        ImVec2 curpos = ImGui::GetCursorPos();
        ImGui::SetCursorPosY(curpos.y - 25.f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);       

        {
            int my_image_width = 700;
            int my_image_height = 400;

            ImGui::Text("GPU handle = %p", m_pTexture->m_pd3dSrvGpuDescriptorHandles[0].ptr);
            ImGui::Text("size = %d x %d", my_image_width, my_image_height);
            ImGui::Image((ImTextureID)m_pRTTexture->m_pd3dSrvGpuDescriptorHandles[0].ptr, ImVec2((float)my_image_width, (float)my_image_height));
        }
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

