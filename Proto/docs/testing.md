# Proto 엔진 테스트 가이드 (Testing Guide)

Proto 엔진의 안정적인 개발과 버그 방지를 위해 구축된 유닛 테스트 환경 가이드입니다. 
본 가이드는 가벼운 C++ 테스트 프레임워크인 **doctest**를 활용한 단위 테스트 작성 및 실행 방법에 대해 설명합니다.

---

## 🚀 1. 테스트 실행 방법

프로젝트 루트 디렉터리에 포함된 PowerShell 스크립트를 실행하여 테스트를 자동으로 빌드하고 실행할 수 있습니다.

### 💻 콘솔에서 실행하기
PowerShell 콘솔을 열고 다음 명령어를 실행합니다:
```powershell
./run_tests.ps1
```

### ⚙️ 동작 과정
1. **MSBuild 탐색**: 로컬 머신에 설치된 최신 Visual Studio MSBuild를 자동으로 검색합니다.
2. **테스트 프로젝트 빌드**: `Proto.slnx` 솔루션 파일 내의 `ProtoTests` 프로젝트만 독립적으로 빌드합니다 (`x64|Debug` 구성).
3. **테스트 실행**: 빌드가 완료되면 생성된 `x64\Debug\ProtoTests.exe` 콘솔 프로그램을 실행하여 테스트 결과를 보여줍니다.

---

## 📝 2. 테스트 작성 요령 (AAA 패턴)

테스트의 일관성과 가독성을 위해 **AAA 패턴 (Arrange - Act - Assert)**을 엄격하게 준수하여 작성해야 합니다.

### 🔍 AAA 패턴 설명
*   **Arrange (준비)**: 테스트를 수행하는 데 필요한 객체 생성, 데이터 설정 및 의존성 준비 단계.
*   **Act (실행)**: 테스트 대상 메서드/함수를 호출하여 실제 연산을 수행하는 단계.
*   **Assert (검증)**: 실행 결과가 예상한 값과 일치하는지 비교하고 검증하는 단계.

### 💡 예제 코드 (`tests/SceneTests.cpp`)
```cpp
#include <doctest/doctest.h>
#include "../src/Scene/Scene.h"
#include "../src/Scene/GameObject.h"
#include "../src/Scene/Components/Transform.h"

TEST_SUITE("Scene System")
{
    TEST_CASE("GameObject Creation and Transform Check")
    {
        // 1. Arrange (준비)
        Proto::Scene scene;

        // 2. Act (실행)
        Proto::GameObject* go = scene.CreateGameObject("TestObject");

        // 3. Assert (검증)
        REQUIRE(go != nullptr); // 조건 불충족 시 테스트 즉시 중단
        CHECK(go->GetName() == "TestObject"); // 실패해도 다음 단계를 계속 진행하여 확인

        auto transform = go->GetComponent<Proto::Transform>();
        REQUIRE(transform != nullptr); 
        CHECK(transform->Translation == glm::vec3(0.0f));
    }
}
```

---

## 🛠️ 3. 테스트 프로젝트 구조 설명

테스트 환경은 메인 엔진 소스 코드 빌드와 꼬이지 않도록 다음과 같이 분리되어 설계되었습니다:

```
Proto/
├── src/                      # 메인 엔진 소스 코드
│   └── main.cpp              # 엔진 실행 진입점 (테스트 빌드 시 제외!)
├── tests/                    # 테스트 전용 디렉터리
│   ├── main.cpp              # doctest 실행 진입점 (DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
│   └── SceneTests.cpp        # 씬 및 컴포넌트 유닛 테스트
├── Proto.slnx                # 새 Visual Studio 솔루션 파일 (ProtoTests 등록됨)
├── Proto.vcxproj             # 메인 엔진 프로젝트
├── ProtoTests.vcxproj        # 테스트 프로젝트
└── run_tests.ps1             # 빌드 및 실행 자동화 스크립트
```

### ⚠️ 중요 주의 사항 (소스 파일 추가 시)
1. **메인 함수 중복 방지**: 
   `ProtoTests.vcxproj`는 `src/main.cpp`를 컴파일 리스트에서 제외하고 대신 `tests/main.cpp`를 진입점으로 사용합니다. 이를 통해 메인 함수 이중 정의(`LNK2005` 오류)를 원천 차단합니다.
2. **새 소스 파일(.cpp) 추가 시**: 
   엔진 핵심 로직에 새로운 소스 파일(`.cpp`)을 추가할 경우, 메인 프로젝트(`Proto.vcxproj`)와 테스트 프로젝트(`ProtoTests.vcxproj`) **두 군데 모두에 파일을 등록**해 주어야 테스트 프로젝트에서 링커 오류가 발생하지 않습니다.
   * *Tip*: Visual Studio 솔루션 탐색기에서 소스 파일을 복사하여 두 프로젝트에 각각 추가하면 편리합니다.
