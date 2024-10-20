#include "App.h"
#include "GeometryGenerator.h"

#include <algorithm>    // std::clamp
#include <shobjidl.h>   // file dialog
#include <filesystem>   // std::path

// ���� ����
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ��� �Լ��� ���������� ȣ���ϱ� ���� ���� ������
App* g_app = nullptr;

App::App()
    : m_screenWidth(1280), m_screenHeight(720) {
    g_app = this;
}

App::~App() {
    g_app = nullptr;

    // ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(m_hWnd);
}

bool App::Initialize() {
    if (!InitWindow()) return false;
    if (!InitDirect3D()) return false;
    if (!InitGUI()) return false;

    m_OriginalMeshGroup.Initialize(m_device);

    //auto box = GeometryGenerator::CreateBox();
    //box.textureFilename = "wall.jpg";
    //m_OriginalMeshGroup.AddMesh(m_device, { box });

    // from https://f3d.app//doc/GALLERY.html
    auto zelda = GeometryGenerator::ReadFromFile("C:/Users/duram/Downloads/zelda/", "zeldaPosed001.fbx");
    m_OriginalMeshGroup.AddMesh(m_device, { zelda });
    m_OriginalMeshGroup.m_pixelConstantData.material = Material::GetMaterialPreset(MaterialPreset::Gold);

    // ���� ����
    m_ShellMeshGroup.Initialize(m_device);
    auto shell = GeometryGenerator::CreateShell(zelda, 0.01f);
    m_ShellMeshGroup.AddMesh(m_device, shell);
    m_ShellMeshGroup.m_pixelConstantData.material = Material::GetMaterialPreset(MaterialPreset::Silver);

    // ť��� �ʱ�ȭ
    // from https://www.humus.name/index.php?page=Textures&ID=124
    m_cubeMapping.Initialize(m_device, L"skybox.dds", L"diffuseMap.dds", L"specularMap.dds");
    m_OriginalMeshGroup.m_diffuseResView = m_cubeMapping.m_diffuseResView;
    m_OriginalMeshGroup.m_specularResView = m_cubeMapping.m_specularResView;

    return true;
}

int App::Run() {
    // ������ �޽��� ó�� ����
    MSG msg = {};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // ImGui frame ����
            ImGui_ImplWin32_NewFrame();
            ImGui_ImplDX11_NewFrame();
            ImGui::NewFrame();
            
            // ImGui�� ���� �� ������Ʈ
            UpdateGUI();

            // constant data ������Ʈ
            UpdateVertexConstantData();
            UpdatePixelConstantData();

            // ���� ������Ʈ
            m_OriginalMeshGroup.UpdateConstantBuffers(m_device, m_context);
            m_ShellMeshGroup.UpdateConstantBuffers(m_device, m_context);
            m_cubeMapping.UpdateConstantBuffers(m_device, m_context);

            // ������
            Render();

            // ImGui frame ����
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // swap chain ����
            m_swapChain->Present(1, 0);
        }
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return g_app->MsgProc(hWnd, msg, wParam, lParam);
}

LRESULT App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;
    switch (msg) {
    case WM_SIZE:   // â ũ�� ���� ��
        if (m_swapChain) { // ó�� ������ �ƴ��� Ȯ��
            // ��� ���� ������Ʈ
            m_screenWidth = LOWORD(lParam);
            m_screenHeight = HIWORD(lParam);

            // ���� RTV ����
            m_renderTargetView.Reset();

            // swap chain ũ�� ����
            m_swapChain->ResizeBuffers(
                0,  // ���� ���� �� ����
                static_cast<UINT>(m_screenWidth),
                static_cast<UINT>(m_screenHeight),
                DXGI_FORMAT_UNKNOWN, // ���� ���� ����
                0);

            // ����Ʈ ����
            D3D11Utils::SetViewport(m_context, m_screenWidth, m_screenHeight);

            // RTV ����
            D3D11Utils::CreateRenderTargetView(m_device, m_swapChain, m_renderTargetView);

            // ���� depth stencil view ����
            m_depthStencilView.Reset();
             //depth stencil buffer ����
            D3D11Utils::CreateDepthBuffer(m_device, m_screenWidth, m_screenHeight, m_depthStencilView);
        }
        break;
    case WM_LBUTTONDOWN:    // ���콺 ���� ��ư down
        m_isMouseDragging = true;
        SetCapture(hwnd); // ���콺 ĸó
        GetCursorPos(&m_lastMousePosition);
        break;
    case WM_MOUSEMOVE:      // ���콺 �̵�
        // �巡�� ���� ������ ����
        if (m_isMouseDragging) {
            POINT currentMousePosition;
            GetCursorPos(&currentMousePosition);

            // ���콺 �̵��� ���
            float deltaX = currentMousePosition.x - m_lastMousePosition.x;
            float deltaY = currentMousePosition.y - m_lastMousePosition.y;

            // ���� ������Ʈ
            m_cameraAngleX = std::clamp(m_cameraAngleX + deltaY * m_rotationSpeed, -m_maxPitch, m_maxPitch);
            m_cameraAngleY += deltaX * m_rotationSpeed;

            // ������ ��ġ ������Ʈ
            m_lastMousePosition = currentMousePosition;
        }
        break;
    case WM_LBUTTONUP:  // ���콺 ���� ��ư up
        m_isMouseDragging = false;
        ReleaseCapture(); // ���콺 ĸó ����
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool App::InitWindow() {
    // DPI �ν� ����
    SetProcessDPIAware();

    // ������ Ŭ���� ����
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"DMEWindowClass";
    wc.hIconSm = nullptr;

    // ������ Ŭ���� ���
    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Window Registration Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // ������ ��Ÿ��
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_APPWINDOW;      // �۾� ǥ���ٿ� ������ ǥ��

    // ������ ũ�� ����
    // ���� �� -> ������ �Ʒ� ����
    RECT rect = { 0, 0, m_screenWidth, m_screenHeight };
    AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

    // ������ ����
    m_hWnd = CreateWindowEx(
        dwExStyle,
        wc.lpszClassName,
        L"Dynamic Mesh Effect",
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);

    if (!m_hWnd) {
        MessageBox(nullptr, L"Window Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hWnd);

    return true;
}

bool App::InitDirect3D() {
    // swap chain desc
    DXGI_SWAP_CHAIN_DESC scDesc;
    ZeroMemory(&scDesc, sizeof(scDesc));
    scDesc.BufferDesc.Width = m_screenWidth;
    scDesc.BufferDesc.Height = m_screenHeight;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.SampleDesc.Count = 1;     // not use MSAA
    scDesc.SampleDesc.Quality = 0;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = 2;
    scDesc.OutputWindow = m_hWnd;
    scDesc.Windowed = TRUE;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // feature level
    D3D_FEATURE_LEVEL featureLevels[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL featureLevel;

    // ����� ��� �� �÷��� ����
    UINT createDeviceFlags = 0;
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // device �� swap chain ����
    if (FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &scDesc,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        &featureLevel,
        m_context.GetAddressOf()))) {
        MessageBox(nullptr, L"Device and Swap Chain Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // RTV ����
    D3D11Utils::CreateRenderTargetView(m_device, m_swapChain, m_renderTargetView);

    // ����Ʈ ����
    D3D11Utils::SetViewport(m_context, m_screenWidth, m_screenHeight);
    
    // rasterizer state ����
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = FALSE;
    rastDesc.DepthClipEnable = TRUE;
    if (FAILED(m_device->CreateRasterizerState(&rastDesc, m_rasterizerStateSolid.GetAddressOf()))) {
        MessageBox(nullptr, L"Rasterizer State(Solid) Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    // wireframe�� ���� ����
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    if (FAILED(m_device->CreateRasterizerState(&rastDesc, m_rasterizerStateWire.GetAddressOf()))) {
        MessageBox(nullptr, L"Rasterizer State(Wireframe) Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    // �ʱ� �������� solid
    m_context->RSSetState(m_rasterizerStateSolid.Get());

    // depth stencil buffer ����
    D3D11Utils::CreateDepthBuffer(m_device, m_screenWidth, m_screenHeight, m_depthStencilView);

    // depth stencil state ����
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;
    if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf()))) {
        MessageBox(nullptr, L"Depth Stencil State Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

bool App::InitGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight));
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(m_hWnd)) {
        MessageBox(nullptr, L"ImGui Win32 Initialization Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get())) {
        MessageBox(nullptr, L"ImGui DX11 Initialization Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void App::UpdateVertexConstantData() {
    // model
    XMMATRIX modelMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
        XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
        XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.model, XMMatrixTranspose(modelMatrix));
    XMStoreFloat4x4(&m_ShellMeshGroup.m_vertexConstantData.model, XMMatrixTranspose(modelMatrix));

    // invTranspose
    XMMATRIX invTransposeMatrix = modelMatrix * XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    invTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, modelMatrix));
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.invTranspose, XMMatrixTranspose(invTransposeMatrix));
    XMStoreFloat4x4(&m_ShellMeshGroup.m_vertexConstantData.invTranspose, XMMatrixTranspose(invTransposeMatrix));

    // view

    // ī�޶� �ʱ� �Ӽ�
    XMVECTOR cameraPosition = XMVectorSet(0.0f, 0.0f, -1.5f, 1.0f);
    XMVECTOR cameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // ī�޶� ȸ�� ����
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_cameraAngleX, m_cameraAngleY, 0.0f);
    cameraPosition = XMVector3TransformCoord(cameraPosition, rotationMatrix);
    XMStoreFloat3(&m_OriginalMeshGroup.m_pixelConstantData.viewWorld, cameraPosition);
    XMStoreFloat3(&m_ShellMeshGroup.m_pixelConstantData.viewWorld, cameraPosition);

    // view ��� ���
    XMMATRIX viewMatrix = XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.view, XMMatrixTranspose(viewMatrix));
    XMStoreFloat4x4(&m_ShellMeshGroup.m_vertexConstantData.view, XMMatrixTranspose(viewMatrix));

    // projection
    XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(m_fovY, static_cast<float>(m_screenWidth) / m_screenHeight, m_nearZ, m_farZ);
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.projection, XMMatrixTranspose(projectionMatrix));
    XMStoreFloat4x4(&m_ShellMeshGroup.m_vertexConstantData.projection, XMMatrixTranspose(projectionMatrix));

    // ���콺 ��ġ �޾ƿ���
    XMVECTOR temp = GetMousePos3D(1.5f, viewMatrix, projectionMatrix, cameraPosition, cameraTarget);
    XMStoreFloat3(&m_ShellMeshGroup.m_shellVertexConstantData.mousePos, temp);

    // ť���
    XMStoreFloat4x4(&m_cubeMapping.m_vertexConstantData.viewProj, XMMatrixTranspose(viewMatrix * projectionMatrix));
}

void App::UpdatePixelConstantData() {
    // direction ����ȭ
    XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&m_light[m_lightType].direction));
    XMStoreFloat3(&m_light[m_lightType].direction, dir);

    m_OriginalMeshGroup.m_pixelConstantData.light = m_light[m_lightType];
    m_OriginalMeshGroup.m_pixelConstantData.lightType = m_lightType;
}

static std::pair<std::string, std::string> OpenFileDialogWithCommonItemDialog() {
    std::pair<std::string, std::string> result;

    // COM ���̺귯�� �ʱ�ȭ
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog* pFileOpen;

        // FileOpenDialog ��ü ����
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            // ��ȭ���� ǥ��
            hr = pFileOpen->Show(NULL);

            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    // ���� ��� ����
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr)) {
                        // ��ο� ���� �̸� �и�
                        std::filesystem::path path(pszFilePath);

                        result.first = path.parent_path().string() + "/";
                        result.second = path.filename().string();

                        // GeometryGenerator::ReadFromFile�� �´� ���·� ó��
                        std::replace(result.first.begin(), result.first.end(), '\\', '/');

                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return result;
}

void App::UpdateGUI() {
    // �� ��ȯ
    ImGui::SliderFloat3("Scale", &m_scale.x, 0.1f, 2.0f);
    ImGui::SliderFloat3("Rotation", &m_rotation.x, -3.14f, 3.14f);
    ImGui::SliderFloat3("Translation", &m_translation.x, -1.0f, 1.0f);

    // �ؽ�ó ���
    ImGui::Checkbox("Use Texture", &m_OriginalMeshGroup.m_pixelConstantData.useTexture);

    // wireframe ����
    ImGui::Checkbox("Wireframe", &m_drawAsWire);

    // ���� Ÿ��
    if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
        m_lightType = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
        m_lightType = 1;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
        m_lightType = 2;
    }

    // ��Ƽ���� �Ӽ�
    ImGui::SliderFloat("Shininess", &m_OriginalMeshGroup.m_pixelConstantData.material.shininess, 1.0f, 256.0f);

    // ���� �Ӽ�
    ImGui::SliderFloat3("Light Position", &m_light[m_lightType].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Light Direction", &m_light[m_lightType].direction.x, -5.0f, 5.0f);
    ImGui::SliderFloat("Light fallOffStart", &m_light[m_lightType].fallOffStart, 0.0f, 5.0f);
    ImGui::SliderFloat("Light fallOffEnd", &m_light[m_lightType].fallOffEnd, 0.0f, 10.0f);
    ImGui::SliderFloat("Light spotPower", &m_light[m_lightType].spotPower, 1.0f, 512.0f);

    // �� �ҷ�����
    if (ImGui::Button("Import Model")) {
        auto fileInfo = OpenFileDialogWithCommonItemDialog();

        if (!fileInfo.second.empty()) {
            std::cout << fileInfo.first << std::endl;
            std::cout << fileInfo.second << std::endl;
            
            m_OriginalMeshGroup.ClearMeshes();
            auto newMeshData = GeometryGenerator::ReadFromFile(fileInfo.first, fileInfo.second);
            m_OriginalMeshGroup.AddMesh(m_device, { newMeshData });
            m_OriginalMeshGroup.m_pixelConstantData.material = Material::GetMaterialPreset(MaterialPreset::Gold);
        }
    }
}

void App::Render() {
    // RTV, depth stencil view Ŭ����
    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // ������ ���
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // RTV, depth stencil state ����
    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

    // wireframe ����
    if (m_drawAsWire) {
        m_context->RSSetState(m_rasterizerStateWire.Get());
    }
    else {
        m_context->RSSetState(m_rasterizerStateSolid.Get());
    }

    // MeshGroup ����
    m_OriginalMeshGroup.Render(m_context);

    // ���� ����
    m_ShellMeshGroup.Render(m_context);

    // ť��� ����
    m_cubeMapping.Render(m_context);
}

XMVECTOR App::GetMousePos3D(float planeDistance, XMMATRIX viewMatrix, XMMATRIX projMatrix, XMVECTOR cameraPosition, XMVECTOR cameraTarget) {
    // ���� ���콺 ��ġ�� Ŭ���̾�Ʈ ��ǥ�� ��������
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(m_hWnd, &mousePos);

    // 1. ���콺 ��ǥ�� NDC�� ��ȯ
    float ndcX = (2.0f * mousePos.x) / m_screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePos.y) / m_screenHeight;
    XMVECTOR ndcPos = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);  // z�� 1�� ���� (far plane)

    // 2. view�� projection ��ȯ�� ����� ���
    XMMATRIX invViewProj = XMMatrixInverse(nullptr, XMMatrixMultiply(viewMatrix, projMatrix));
    
    // 3. NDC ��ǥ�� ���� ��ǥ�� ��ȯ
    XMVECTOR rayEnd = XMVector3TransformCoord(ndcPos, invViewProj);

    // 4. ���� ���� ���
    XMVECTOR rayDir = XMVector3Normalize(rayEnd - cameraPosition);

    // 5. ī�޶� �ٶ󺸴� ����
    XMVECTOR viewDir = XMVector3Normalize(cameraTarget - cameraPosition);

    // 6. ������ ���
    float t = planeDistance / XMVectorGetX(XMVector3Dot(viewDir, rayDir));
    XMVECTOR intersectionPoint = XMVectorAdd(cameraPosition, XMVectorScale(rayDir, t));

    return intersectionPoint;
}