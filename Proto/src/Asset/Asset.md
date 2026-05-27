# Asset Layer

Asset 계층은 엔진에서 사용하는 모든 자원(메쉬, 셰이더 등)의 생명주기를 관리하고, UUID를 통해 조회할 수 있는 중앙 저장소를 제공합니다.

## 📂 파일 구조
- `Asset.h`: 에셋의 기본 인터페이스(`Asset`)와 타입(`AssetType`), 기본 에셋 ID(`DefaultAsset`) 정의
- `AssetManager.h/cpp`: UUID 기반 에셋 중앙 관리자 (추가/조회/삭제)
- `BuiltinAssetLoader.h/cpp`: 엔진 시작 시 기본 메쉬·셰이더 에셋을 생성하고 등록

---

## 🛠 핵심 컴포넌트 상세

### 1. Asset (기본 인터페이스)
모든 에셋이 공통으로 상속하는 추상 기반 클래스입니다.
- **UUID Handle**: 각 에셋은 생성 시 고유한 `UUID`를 `Handle`로 부여받아, 씬 직렬화나 컴포넌트 참조에 사용됩니다.
- **AssetType**: `None`, `Scene`, `Mesh`, `Shader`, `Material`, `Texture` 값을 가지는 열거형으로, 에셋의 종류를 식별합니다.

### 2. DefaultAsset (기본 에셋 ID 상수)
엔진 내부에서 항상 사용 가능한 기본 에셋들의 고정 UUID를 정의합니다.

| 상수 | UUID | 설명 |
|------|------|------|
| `CUBE` | 1 | 기본 큐브 메쉬 |
| `PLANE` | 2 | 기본 플레인 메쉬 |
| `SPHERE` | 3 | 기본 구형 메쉬 |
| `CYLINDER` | 4 | 기본 실린더 메쉬 |
| `SHADER` | 100 | 기본 Phong 셰이더 |

### 3. AssetManager (정적 관리자)
모든 에셋을 UUID 키로 관리하는 전역 저장소입니다. 인스턴스화를 금지하며 정적 메서드로만 접근합니다.
- **`AddAsset(asset)`**: 에셋을 등록합니다.
- **`GetAsset(uuid)`**: UUID로 에셋을 조회합니다.
- **`GetAssetAs<T>(uuid)`**: UUID로 에셋을 조회하고 `dynamic_pointer_cast`로 타입 변환하여 반환합니다.
- **`RemoveAsset(uuid)`**: 에셋을 등록 해제합니다.
- **`Clear()`**: 모든 에셋을 제거합니다.

### 4. BuiltinAssetLoader
엔진 초기화 시 호출되어 기본 에셋들을 생성하고 `AssetManager`에 등록합니다.
- **`Load()`**: `MeshLoader`를 통해 Cube/Sphere/Plane/Cylinder 메쉬를 생성하고, 기본 셰이더를 컴파일하여 `DefaultAsset` ID로 등록합니다.
- 이후 씬 내의 `CreateMeshGameObject`나 `MeshRenderer::Deserialize`에서 해당 ID로 즉시 조회하여 사용합니다.

---

## 🎨 디자인 원칙
*   **UUID 기반 참조**: 에셋은 이름이 아닌 UUID로 식별하여 이름 변경에 의한 참조 오류를 방지합니다.
*   **정적 관리**: `AssetManager`는 인스턴스 없이 전역에서 접근 가능한 단일 저장소로 동작합니다.
*   **타입 안전성**: `GetAssetAs<T>`를 통해 `dynamic_pointer_cast` 기반의 타입 안전한 에셋 조회를 제공합니다.
