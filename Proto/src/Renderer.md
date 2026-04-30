# Renderer 계층

OpenGL 기반 렌더링 추상화와 저수준 그래픽 리소스를 관리합니다.

## 파일 구조

```
src/Renderer/
├── Renderer.h/cpp         (렌더링 명령 인터페이스)
├── Shader.h/cpp           (GLSL 셰이더 프로그램)
├── VertexArray.h/cpp      (VAO 추상화)
├── Buffer.h/cpp           (VBO, IBO 추상화)
├── Framebuffer.h/cpp      (오프스크린 렌더링)
├── Camera.h               (카메라 기본 구조)
├── SceneCamera.h          (씬 카메라 구현)
└── EditorCamera.h/cpp     (에디터 카메라)
```

## 주요 클래스

### Renderer (Static)

**책임**: 렌더링 명령 수집 및 발행

| 메서드 | 설명 |
|--------|------|
| `SetClearColor(color)` | 클리어 색상 설정 (RGBA) |
| `Clear()` | 컬러 및 깊이 버퍼 클리어 |
| `Submit(vao, shader)` | 드로우 콜 발행 |

### Shader

**책임**: GLSL 셰이더 프로그램 관리

| 메서드 | 설명 |
|--------|------|
| `Bind()` | 셰이더 활성화 |
| `Unbind()` | 셰이더 비활성화 |
| `UploadUniformMat4(name, mat)` | 4x4 행렬 유니폼 전송 |
| `UploadUniformFloat3(name, vec)` | 3차 벡터 유니폼 전송 |
| `UploadUniformFloat(name, val)` | 실수 유니폼 전송 |
| `UploadUniformInt(name, val)` | 정수 유니폼 전송 |

**내부**:
- 정점 셰이더, 프래그먼트 셰이더 컴파일
- 프로그램 링크
- m_RendererID - OpenGL 프로그램 ID

### VertexArray

**책임**: VAO (Vertex Array Object) 관리

| 메서드 | 설명 |
|--------|------|
| `Bind()` | VAO 활성화 |
| `Unbind()` | VAO 비활성화 |
| `AddVertexBuffer(vbo)` | VertexBuffer 연결 |
| `SetIndexBuffer(ibo)` | IndexBuffer 연결 |
| `GetVertexBuffers()` | VBO 벡터 반환 |
| `GetIndexBuffer()` | IBO 반환 |

### Buffer 추상화

**VertexBuffer** - 정점 데이터 (VBO)

```
생성: VertexBuffer(data, size)
용도: 위치, 법선, UV 등 정점 속성
```

**IndexBuffer** - 인덱스 데이터 (IBO)

```
생성: IndexBuffer(indices, count)
용도: 드로우 순서 정의
```

### Framebuffer

**책임**: 오프스크린 렌더링 (FBO)

```
Bind()               - FBO 활성화
Unbind()             - 화면으로 복귀
Resize(width, height)- 크기 변경
GetColorAttachment() - 컬러 텍스처
```

**용도**:
- 에디터 뷰포트 렌더링
- 게임 런타임 렌더링
- 포스트 프로세싱 (향후)

### Camera

**책각**: 뷰/투영 행렬 관리

```
GetViewMatrix()       - 뷰 행렬
GetProjectionMatrix() - 투영 행렬
```

**파생 클래스**:
- **SceneCamera** - 씬 내 게임 카메라
- **EditorCamera** - 에디터 자유 카메라

## 렌더링 파이프라인

```
Renderer::Submit(vao, shader)
├── shader.Bind()
├── vao.Bind()
├── shader.UploadUniform*()  (행렬, 색상 등)
├── glDrawElements()          (OpenGL 드로우 콜)
└── vao.Unbind()
```

## 데이터 구조

```
Scene
└── GameObject
    └── MeshRenderer
        ├── VertexArray
        │   ├── VertexBuffer (위치, 법선, UV)
        │   └── IndexBuffer (드로우 순서)
        └── Shader
            └── Uniform (행렬, 색상)
```

## 설계 특징

1. **OpenGL 추상화** - 저수준 API 캡슐화
2. **Resource Sharing** - std::shared_ptr로 메시/셰이더 공유
3. **Framebuffer 분리** - 에디터/게임 뷰포트 독립
4. **Uniform 전송** - 유니폼 API 중앙화

## 확장 포인트

- Shader 전처리 - 매크로, 버전 관리
- Framebuffer 확장 - 깊이, 법선 맵 등
- 라이팅 시스템 - 조명 유니폼 자동화
- 포스트 프로세싱 - 효과 파이프라인
