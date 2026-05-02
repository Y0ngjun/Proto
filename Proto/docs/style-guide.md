# 코드 스타일 가이드

Proto 엔진의 코드 작성 규칙입니다. 간결함과 일관성을 우선으로 합니다.

## 1. 네이밍 컨벤션
| 대상 | 규칙 | 예시 |
|------|------|------|
| 클래스/구조체 | `PascalCase` | `Application`, `Transform` |
| 메서드/함수 | `PascalCase` | `Run()`, `GetDeltaTime()` |
| Private 멤버 | `m_` + `PascalCase` | `m_DeltaTime`, `m_SelectedObject` |
| Public 멤버 | `PascalCase` | `Width`, `Height` |
| 로컬 변수 | `camelCase` | `deltaTime`, `index` |
| 상수 | `UPPER_SNAKE_CASE` | `MAX_COMPONENTS`, `EPSILON` |
| 네임스페이스 | `PascalCase` | `Proto::Renderer` |

## 2. 포맷팅
- **들여쓰기**: Tab 사용 (1 tab = 4 칸)
- **중괄호 배치**: 다음 줄에 배치하는 Allman 스타일 사용
- **줄 길이**: 가급적 120자 이내 유지

## 3. 주석 규칙
- **언어**: 항상 **한국어**로 작성
- **파일 헤더**: 핵심 클래스/구조체 파일 상단에 1~2줄 요약
- **메서드/인라인**: 복잡한 비즈니스 로직이나 수학적 계산에만 간결하게 추가

## 4. 기타 코딩 지침
- **Include 순서**: 표준 라이브러리 → 외부 라이브러리 → 프로젝트 로컬 헤더 순
- **메모리 관리**: 가급적 `std::unique_ptr`, `std::shared_ptr` 등 스마트 포인터 사용
- **Const Correctness**: 불변 데이터나 참조 파라미터에는 적극적으로 `const` 사용
