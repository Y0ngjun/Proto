# Core Layer

Core 계층은 엔진의 기반이 되는 시스템들로 구성되어 있으며, 애플리케이션의 생명주기(Life Cycle), 윈도우 추상화, 시스템 입력 및 유틸리티를 관장합니다.

## 📂 파일 구조
- `Application.h/cpp`: 메인 루프 및 시스템 통합 관리
- `Window.h/cpp`: GLFW 기반 윈도우 및 컨텍스트 추상화
- `Input.h/cpp`: 상위 수준 입력 처리 인터페이스
- `RawInput.h/cpp`: GLFW 직접 연동 하위 수준 입력 처리
- `KeyCodes.h / MouseCodes.h`: 입력 코드 정의
- `Log.h/cpp`: 타임스탬프 기반 콘솔 로그 출력 (`PROTO_LOG_INFO/WARN/ERROR`)
- `FileDialog.h/cpp`: OS 네이티브 파일 다이얼로그 (Windows)
- `Project.h/cpp`: 프로젝트 설정(ProjectConfig) 저장/로드 및 활성 프로젝트 관리
- `UUID.h/cpp`: 64비트 고유 식별자 생성기
- `YAMLHelpers.h`: 데이터 직렬화를 위한 YAML 유틸리티

---

## 🛠 핵심 컴포넌트 상세

### 1. Application (Singleton)
엔진의 심장부로, 전체 시스템의 초기화와 메인 루프를 제어합니다.
- **게임 루프**: `Init` → `Run` (Update/Render) → `Shutdown` 흐름 관리.
- **프레임 타임**: 델타 타임(Delta Time)을 계산하여 일관된 물리/애니메이션 속도 보장.
- **씬 관리**: 현재 활성화된 Scene의 로드/저장 및 상태(Edit/Play) 전환 제어.
  - 새 씬(`NewScene`) 생성 시 기본 카메라의 뷰포트를 실시간 갱신합니다.
- **프로젝트 관리**: 프로젝트 생성/열기/저장 및 경로 관리 (`m_ActiveProjectPath`, `m_ActiveScenePath`).
- **저장 경고**: Open/New/Close 시 미저장 상태 감지 및 사용자에게 저장 여부 확인.
- **Dirty Flag**: Scene과 Project의 수정 여부를 추적하여 저장 필요성 판단.
- **UI 통합**: ImGui 프레임의 시작과 끝을 관리하며, Editor 패널(Inspector, Hierarchy)을 소유합니다.
- **단축키 연동**:
  - `Ctrl+N`: NewProject
  - `Ctrl+O`: OpenProject
  - `Ctrl+S`: SaveScene (경로 없으면 SaveSceneAs 유도)
  - `Ctrl+Shift+S`: SaveSceneAs
  - `Alt+F4`: Exit (저장 경고 포함)

### 2. Window
OS의 네이티브 윈도우를 추상화하며 그래픽 API(OpenGL)와의 연결고리 역할을 합니다.
- **추상화**: GLFW를 사용하여 크로스 플랫폼 가능성을 열어두고 윈도우 생성/파괴 관리.
- **컨텍스트**: OpenGL 컨텍스트 생성 및 VSync 제어.
- **이벤트**: 윈도우 사이즈 변경, 닫기 등의 시스템 이벤트 폴링.

### 3. Input & RawInput
사용자의 입력을 감지하고 엔진 내부에서 사용할 수 있도록 정제합니다.
- **Input (High-level)**: 엔진 전체에서 접근 가능한 정적 인터페이스 제공.
- **RawInput (Low-level)**: GLFW 윈도우 핸들에 직접 접근하여 키보드/마우스 상태를 즉각적으로 쿼리.
- **Key/Mouse Codes**: 플랫폼 독립적인 코드 정의를 통해 하위 라이브러리(GLFW) 의존성 분리.

### 4. Log
엔진 전역에서 사용하는 로그 시스템입니다.
- **매크로 인터페이스**: `PROTO_LOG_INFO`, `PROTO_LOG_WARN`, `PROTO_LOG_ERROR` 매크로로 간편 호출.
- **타임스탬프**: 각 로그에 `[HH:MM:SS]` 형식의 시각 정보를 자동 포함.
- **에디터 연동**: 콘솔 패널(ConsolePanel)에 실시간으로 로그를 전달.

### 5. Project
프로젝트 단위의 설정 관리 및 씬 파일 경로 추적을 담당합니다.
- **ProjectConfig**: 프로젝트 이름, 시작 씬 경로 등의 설정값을 담는 구조체.
- **직렬화**: `Project.proto` 파일에 YAML 포맷으로 설정 저장/복원.
- **활성 프로젝트**: `static std::shared_ptr<Project> s_ActiveProject`로 전역 접근 지원.
- **Dirty Flag**: `SetDirty` / `IsDirty`로 미저장 상태를 추적.

### 6. Utility (UUID & FileDialog)
- **UUID**: 모든 GameObject와 자원에 부여되는 64비트 고유 ID를 생성합니다. (`std::random_device` 기반)
- **FileDialog**: `CommonItemDialog` API를 사용하여 Windows 탐색기 기반의 파일 열기/저장 기능을 제공합니다.

---

## 🎨 디자인 원칙 및 특징

*   **RAII**: 모든 핵심 시스템은 스마트 포인터와 소멸자를 통해 자원 누수 없이 관리됩니다.
*   **Singleton Pattern**: `Application`은 전역적으로 접근 가능한 중앙 제어 장치로서 싱글톤으로 구현되어 있습니다.
*   **Data Driven**: `YAMLHelpers`를 통해 씬과 설정값들을 직렬화하여 데이터 중심의 엔진 구조를 지향합니다.
*   **Decoupling**: 입력 시스템과 윈도우 시스템, 로그 시스템을 분리하여 유지보수성을 높였습니다.
*   **한국어 로그**: 모든 에러/정보 로그는 한국어로 작성하여 디버깅 편의성을 확보합니다.
