# 진행 상황 (Progress)

Proto: 4계층 아키텍처 기반 3D 엔진 (Core/Scene/Renderer/Editor)
Milestone 4 (Scripting & UI) 진행 중. MVP 목표: 3D 탄막 게임.

---

## 🚨 긴급 안건
- 기능 명세 작성
  - 각 기능 점검
- 테스트 도입
- 직렬화/역직렬화 전수조사

## 📋 백로그
- public API 정의
- 스마트 포인터 전면 도입
- 중앙 집중식 데이터 수정 시스템
  - Command 패턴을 통한 데이터 변경 로직 통일
  - Undo/Redo 기능 구현 기반 구축
  - 프로퍼티 리플렉션
- 씬 그리드 고급화
- 머터리얼 캡슐화
- 입력 시스템 개선 (향후 빌드 고려)

## ✅ 완료된 마일스톤 (Milestone 0~3)
- **M0: 기초**: App/Window/GameObject/Component/Renderer/Picking/Gizmo 완비
- **M1: 직렬화**: UUID, YAML 저장/로드, 프로젝트 자동 생성 및 중심 워크플로우 구축
- **M2: Runtime**: Play Mode, SceneCamera, NativeScript(ScriptRegistry) 구현
- **M3: Physics**: Rigidbody, Collider(Box/Sphere), 충돌 감지 및 콜백 시스템 구축

---

## 📋 진행 중 및 향후 로드맵

### Milestone 4: Scripting & UI (🔄 진행 중)
- [ ] EventBus (게임 이벤트 시스템)
- [ ] UI System (Canvas, Text, Image)
- [x] ScriptRegistry (이름 기반 자동 바인딩)
- [x] 기본 메쉬 생성 단축 (Toolbar "Create Primitive" 버튼, Hierarchy 우클릭 메뉴)
- [ ] 플레이어 입력-스크립트 연동 강화

### Milestone 5: Optimization & Refinement (다음)
- [ ] 외부 물리 엔진(Jolt/PhysX) 연동 및 성능 최적화
- [x] 입력 시스템 분리 (Raw Input vs Filtered Game Input)
- [ ] Public API 정립 및 스마트 포인터 전면 도입
- [x] 에디터 고도화 (콘솔 분할 뷰 ✅, 스마트 저장/단축키 ✅)
- [ ] GetComponent 최적화 및 에셋 매니저 고도화

### Milestone 6: MVP Prototype (마지막)
- [ ] 플레이어 제어, 탄막 패턴, 적 AI, 게임 로직(점수/체력) 구현

---

## 📅 최근 업데이트 (2026-05-15)
- **환경 설정**: Git 루트(상위 디렉토리)의 `.gitignore` 통합 및 최적화
  - 하위 프로젝트의 중복 `.gitignore` 제거
  - `Dependencies` 폴더 및 내부 바이너리(.lib, .dll)가 추적되도록 예외 처리 추가
  - 빌드 결과물(`x64`, `.vs` 등) 및 로그 파일 무시 규칙 강화

## 📅 최근 업데이트 (2026-05-14)
- **리팩토링 4사이클 완료**: 엔진 전역 종합 코드 품질 개선 완료
  - **매직 넘버 상수화**: 모든 모듈에서 사용되던 숫자 리터럴을 `UPPER_SNAKE_CASE` 상수로 명명 (`GRAVITY`, `DEFAULT_NEAR_CLIP`, `GRID_SIZE` 등)
  - **C++ 표준 캐스팅**: C 스타일 캐스트 `(type)val`을 `static_cast<type>` / `dynamic_cast<type>`으로 전면 교체
  - **한국어 통일**: 모든 에러 로그(`PROTO_LOG_ERROR`), 파일 상단 요약, 인라인 주석을 한국어로 번역
  - **파일 헤더 통일**: 모든 핵심 클래스 파일 상단에 1~2줄 한국어 요약 헤더 추가
  - **가상 소멸자 추가**: 상속 계층의 컴포넌트 클래스들에 `virtual ~ClassName() = default` 명시
  - **네이티브 스크립트 역직렬화 완성**: `ScriptRegistry::BindByName`을 통한 스크립트 이름 기반 자동 복원 구현
  - **안정성 확보**: 전체 6단계 리팩토링 후 빌드 검증 완료 (경고 0, 오류 0)

## 📅 최근 업데이트 (2026-05-14) — 이전
- **리팩토링 3사이클 완료**: 엔진 전역 의존성 최적화 및 코드 스타일 가이드 적용
  - **의존성 최소화**: 모든 헤더에서 불필요한 `#include`를 제거하고 전방 선언(Forward Declaration)으로 대체하여 컴파일 효율성 극대화
  - **스타일 가이드 준수**: 전면적인 Allman 스타일 중괄호 적용 및 가독성 향상
  - **로직 정돈**: 모든 모듈에 조기 반환(Early Return) 패턴 및 `const` 사용 강화
  - **안정성 확보**: 리팩토링 후 전체 빌드 및 실행 안정성 검증 완료 (경고 0, 오류 0)

---

## 🔴 제한사항 및 빌드
- Windows 전용 (FileDialog), 문자열 기반 컴포넌트 직렬화
- **상태**: ✅ 빌드/실행 성공 (`build.ps1` -> `Proto.exe`)


