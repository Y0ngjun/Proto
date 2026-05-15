# Renderer Layer

Renderer 계층은 하위 수준의 그래픽스 API(OpenGL)를 추상화하고, 엔진의 시각적 표현을 위한 렌더링 파이프라인을 구축합니다.

## 📂 파일 구조
- `Renderer.h/cpp`: 드로우 콜 및 렌더링 상태 제어 (Static API)
- `Shader.h/cpp`: GLSL 셰이더 컴파일 및 유니폼 데이터 관리
- `VertexArray.h/cpp`: 정점 배열 객체(VAO) 추상화
- `Buffer.h/cpp`: 정점 버퍼(VBO) 및 인덱스 버퍼(IBO) 관리
- `Framebuffer.h/cpp`: 오프스크린 렌더링 및 오브젝트 피킹용 ID 버퍼 관리
- `MeshLoader.h/cpp`: 프리미티브 메시(Cube, Sphere, Plane, Cylinder) 생성 및 데이터 로딩
- `Camera.h`: 카메라 인터페이스 기초
- `EditorCamera.h/cpp`: 에디터 뷰포트용 자유 시점 카메라
- `SceneCamera.h/cpp`: 게임 런타임용 컴포넌트 기반 카메라

---

## 🛠 그래픽스 추상화 객체

### 1. Renderer (Static)
엔진의 실제 그리기 명령을 실행하는 중앙 통로입니다. 인스턴스화를 금지하여 유틸리티 클래스로만 사용합니다.
- **Draw Call**: `glDrawElements`를 캡슐화하여 `Submit(vao, shader)` 형태로 메시를 렌더링합니다.
- **API Agnostic**: 추후 다른 그래픽 API(Vulkan, DX12 등)로의 확장을 고려한 인터페이스를 지향합니다.

### 2. Shader
GPU에서 실행될 프로그램을 관리합니다.
- **Compilation**: 소스 코드 또는 파일 경로로부터 셰이더 프로그램을 컴파일하고 링크합니다.
- **Uniform Update**: `UploadUniformMat4`, `UploadUniformFloat3`, `UploadUniformInt` 등의 메서드를 통해 행렬, 조명 데이터, 텍스처 슬롯 정보를 GPU로 전송합니다.
- **한국어 오류 로그**: 셰이더 컴파일/링크 실패 시 `PROTO_LOG_ERROR`를 통해 한국어로 원인을 출력합니다.

### 3. VertexArray & Buffers
GPU 메모리의 기하학적 데이터를 관리합니다.
- **VertexArray (VAO)**: 버퍼 배치(Layout)와 데이터 구성을 기록합니다.
- **Buffer (VBO/IBO)**: 실제 정점 데이터(위치, 법선)와 인덱스 데이터를 보관합니다.
- **상수화**: 정점 속성의 stride, offset 등을 명명된 상수로 관리하여 가독성을 높였습니다.

### 4. Framebuffer
씬을 직접 화면에 출력하지 않고 텍스처에 렌더링합니다.
- **오프스크린 렌더링**: 씬 뷰를 ImGui 텍스처로 전달하기 위해 사용합니다.
- **ID 버퍼 (Attachment 1)**: 각 오브젝트의 런타임 ID를 별도 컬러 어태치먼트에 기록하여 오브젝트 피킹(Pixel Perfect Selection)에 활용합니다.

### 5. MeshLoader
엔진에서 즉시 사용 가능한 표준 기하 구조를 생성합니다.
- **Primitives**: `CreateCube()`, `CreateSphere()`, `CreatePlane()`, `CreateCylinder()` 등을 통해 필요한 정점 데이터를 자동으로 생성하고 VAO로 반환합니다.

---

## 🎥 카메라 시스템

엔진은 두 가지 상이한 목적의 카메라 시스템을 운용합니다.

1. **EditorCamera (Perspective)**
   - **자유 시점**: 마우스 휠(Zoom), 우클릭+WASD(Fly), Shift+드래그(Pan) 등을 통한 직관적인 조작을 지원합니다.
   - **독립성**: 씬 내부의 GameObject에 귀속되지 않고 에디터 뷰포트 자체의 상태로 존재합니다.

2. **SceneCamera (Component-based)**
   - **객체 귀속**: `CameraComponent`를 통해 GameObject에 부착됩니다.
   - **Runtime**: 실제 게임이 실행될 때 플레이어의 시점이나 연출을 담당합니다.
   - **FOV/Clip 관리**: `SetPerspective`로 수직 FOV, Near/Far 클리핑 평면을 설정합니다.

---

## 🔄 렌더링 파이프라인 (Pipeline Flow)

매 프레임마다 다음과 같은 과정을 거쳐 최종 이미지가 생성됩니다.

1.  **Clear Phase**: `Framebuffer`를 바인딩하고 지정된 배경색으로 화면을 초기화합니다.
2.  **Setup Phase**: 카메라의 View-Projection 행렬을 계산하고 셰이더의 전역 유니폼을 업데이트합니다.
3.  **Submission Phase**:
    *   씬 내의 모든 `MeshRenderer`를 순회합니다.
    *   각 객체의 `Transform` 행렬과 조명 정보를 셰이더로 전송합니다.
    *   `Renderer::Submit`을 호출하여 드로우 콜을 발생시킵니다.
4.  **ID Buffer Phase**: 에디터 모드에서 각 객체의 런타임 ID를 Framebuffer Attachment 1에 기록합니다.
5.  **Grid Phase**: 에디터 모드에서 GLSL 기반 인피니트 그리드를 투명 블렌딩으로 렌더링합니다.
6.  **Finalize**: `Framebuffer`를 언바인딩하고 렌더링된 결과 텍스처를 ImGui 뷰포트에 출력합니다.

---

## 🎨 디자인 원칙
*   **Encapsulation**: OpenGL의 직접적인 핸들(ID) 노출을 최소화하고 객체 지향적인 래퍼를 제공합니다.
*   **Separation of Concerns**: 데이터(Buffer), 로직(Shader), 흐름(Renderer)을 엄격히 분리합니다.
*   **Efficiency**: 불필요한 상태 변경(State Change)을 줄이기 위해 필요한 시점에만 바인딩을 수행합니다.
*   **한국어 로그**: 셰이더 컴파일/링크 오류 등 렌더링 관련 에러 로그는 한국어로 출력합니다.
