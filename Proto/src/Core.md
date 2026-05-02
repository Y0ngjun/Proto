# Core 계층

엔진의 초기화, 생명주기 관리, 윈도우 추상화 및 시스템 입력을 관장합니다.

## 핵심 클래스 및 역할

1. **Application (Singleton)**
   - 게임의 메인 루프(Init → Update/Render → Shutdown) 관리.
   - Editor와 Runtime(Play) 모드의 상태(SceneState) 제어.
   - `Run()` 내부에서 델타 타임(Delta Time)을 계산해 하위 시스템에 전달.
   - `SetUpdateCallback()`을 통해 커스텀 게임 로직 주입 가능.

2. **Window**
   - GLFW 기반의 네이티브 윈도우 생성 및 OpenGL 컨텍스트 초기화.
   - VSync(수직 동기화) 설정 및 이벤트 폴링(PollEvents) 수행.

3. **Input**
   - 키보드 및 마우스 입력을 정적/싱글톤 방식으로 처리.
   - 키 눌림 상태와 마우스 좌표계를 하위 로직(카메라 제어, Picking 등)에 제공.

4. **FileDialog**
   - Windows API 기반의 파일 열기/저장 다이얼로그(씬 저장/로드 시 사용).

## 주요 설계 특징
- **RAII 패턴**: Window 및 리소스 관리를 스마트 포인터와 소멸자에 위임.
- **중앙 제어**: 프레임버퍼 바인딩 및 씬(Scene) 업데이트의 기준점이 됨.
