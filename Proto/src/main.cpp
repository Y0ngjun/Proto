#include <iostream>

// 1. GLAD를 무조건 GLFW보다 먼저 인클루드 해야 해!
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 2. GLM 테스트용 인클루드
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 3. ImGui 인클루드
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int main() {
    // [A] GLFW 초기화 및 윈도우 생성 테스트
    if (!glfwInit()) {
        std::cerr << "GLFW 초기화 실패!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Proto Engine Test", NULL, NULL);
    if (!window) {
        std::cerr << "윈도우 창 생성 실패!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync 켜기

    // [B] GLAD 초기화 테스트
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD 초기화 실패!" << std::endl;
        return -1;
    }

    // [C] ImGui 초기화 테스트
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 키보드 컨트롤 허용
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // 도킹 기능 활성화

    ImGui::StyleColorsDark(); // 다크 모드 테마 적용

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // [D] GLM 컴파일 테스트 (에러 안 나면 성공)
    glm::vec3 testVector = glm::vec3(1.0f, 2.0f, 3.0f);

    // [E] 메인 렌더링 루프
    while (!glfwWindowShouldClose(window)) {
        // 입력 처리
        glfwPollEvents();

        // ImGui 새 프레임 시작
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui 데모 창 띄우기 (이게 보이면 성공!)
        ImGui::ShowDemoWindow();

        // 렌더링 (배경색을 짙은 회색으로 지우기)
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ImGui 렌더링 데이터를 OpenGL로 넘기기
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 버퍼 교체 (화면에 그리기)
        glfwSwapBuffers(window);
    }

    // 종료 시 메모리 정리
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}