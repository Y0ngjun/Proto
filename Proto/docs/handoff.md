# Handoff: SetDirty Compile Error 미해결

## 상황 요약
프로젝트/씬 단위의 new/open/save/save as 기능을 구현하는 작업 중, Dirty Flag를 통한 수정 상태 추적을 완성하는 과정에서 컴파일 오류가 발생했다.

## 현재 상태
- ✅ 빌드 성공 (SetDirty 호출을 일단 제거한 상태)
- ✅ `Scene.h/cpp`에 Dirty Flag 완전 구현됨
- ✅ `Project.h`에 SetDirty/IsDirty inline 메서드 선언됨
- ❌ `Project::SetDirty` 호출 시 "멤버가 아니다" 컴파일 오류 발생

## 핵심 문제
**파일 A에서는 메서드가 명확히 보이는데, 파일 B의 컴파일러는 못 본다**

### 오류 메시지
```
C2039: 'SetDirty': 'Proto::Project'의 멤버가 아닙니다.
```
위치: `src/Core/Application.cpp` line 220
```cpp
activeProject->SetDirty(false);
```

### 관련 코드 현황

**Project.h (현재 상태)**
```cpp
namespace Proto {
	class Project {
	public:
		// ... other methods ...
		// Dirty Flag 관리
		void SetDirty(bool dirty) { m_IsDirty = dirty; }
		bool IsDirty() const { return m_IsDirty; }

	private:
		ProjectConfig m_Config;
		bool m_IsDirty = false;
		static std::shared_ptr<Project> s_ActiveProject;
	};
}
```

**Application.h (현재 상태)**
```cpp
#include "Project.h"  // ← 추가됨
namespace Proto {
	class Application {
		// ...
	};
}
```

**Application.cpp (현재 상태)**
```cpp
#include "Application.h"  // 이미 Project.h를 포함
#include "Project.h"      // 명시적 포함도 있음
// ...
void Application::SaveProject() {
	// ...
	activeProject->SetDirty(false);  // ← C2039 오류 발생
}
```

## 시도한 해결책 (모두 실패)
1. ✗ Application.h에 `#include "Project.h"` 추가 → 여전히 오류
2. ✗ Application.cpp에서 Project.h include 순서 변경 → 여전히 오류
3. ✗ SetDirty를 별도 cpp에서 구현 분리 → "m_IsDirty: 선언되지 않은 식별자" 오류
4. ✗ 전방 선언 확인 → Project는 전방 선언 없음, 완전히 포함됨

## 의심 사항
1. **헤더 가드 문제**: Project.h가 여러 번 포함되어 inline 함수가 중복 정의되고 있을 가능성
2. **빌드 캐시**: Visual Studio의 IntelliSense 캐시가 스테일된 상태일 가능성
   - `.vs` 디렉터리 삭제 시도했으나 실패 (프로세스가 파일을 잠금)
3. **다중 정의**: 다른 곳에 Project 클래스가 또 정의되어 있을 가능성
4. **include 순환 참조**: Project.h ↔ Application.h 간의 간접적 순환 참조
5. **전방 선언 오버라이드**: 어디선가 Project를 전방 선언했을 가능성

## 해결 방안 아이디어

### 방안 1: Static 메서드로 변경 (제시했으나 미검증)
```cpp
// Project.h
static void SetDirtyActive(bool dirty) {
	if (s_ActiveProject) s_ActiveProject->m_IsDirty = dirty;
}

// Application.cpp에서
Project::SetDirtyActive(false);  // 인스턴스 없이 호출
```

### 방안 2: Project.cpp에 명시적 구현 분리
```cpp
// Project.h에서 선언만
void SetDirty(bool dirty);

// Project.cpp에서 구현
void Project::SetDirty(bool dirty) { m_IsDirty = dirty; }
```
단, 이전 시도 시 m_IsDirty가 "선언되지 않은 식별자"로 나왔으므로 근본 원인 불명.

### 방안 3: 빌드 시스템 레벨 디버깅
- 전처리기 output 확인 (`/E` 옵션)
- 실제 컴파일 단위에서 Project의 정의가 어떻게 보이는지 확인
- IDE 캐시 완전 초기화 (VS 종료 후 수동 삭제)

### 방안 4: 다른 접근
- Scene.cpp에서 Dirty Flag 사용하는 방식 분석 후 동일하게 적용
- 혹은 Application 내부에서만 Dirty 상태를 관리하고, Project/Scene에는 조회만 하도록 변경

## 다음 담당자를 위한 체크리스트
- [ ] Project.h 전체 파일 재확인 (inline 함수 문법, 닫히는 브레이스)
- [ ] Application.h/cpp 모든 include 목록 확인
- [ ] Project의 다른 정의가 다른 파일에 있는지 검색
- [ ] `#pragma once` 보호 재확인
- [ ] VS 완전 종료 후 .vs 폴더 강제 삭제 후 빌드
- [ ] `/E` 전처리기 옵션으로 매크로 확장 결과 확인
- [ ] Scene.cpp의 SetDirty 사용 패턴과 Project 패턴 비교

## 파일 경로 참고
- `src/Core/Project.h` - 문제의 헤더
- `src/Core/Project.cpp` - Project 구현부
- `src/Core/Application.h` - Project 포함
- `src/Core/Application.cpp` - SetDirty 호출 위치 (line ~220)
- `src/Scene/Scene.h` - 정상 작동하는 Dirty Flag 패턴 참고
- `src/Scene/Scene.cpp` - Dirty Flag 사용 위치

---

## 세부 타임라인
- **초기**: new/open/save/save as proj/scene 단위 구현 계획 수립 및 승인
- **Phase 구현**: Scene.h/cpp, Project.h, Application.h/cpp, docs 수정 완료
- **빌드 성공**: SetDirty 호출 제거 후 빌드 성공 (임시)
- **문제 재현**: SetDirty 복구 시도 → C2039 오류 (현재 상태)
