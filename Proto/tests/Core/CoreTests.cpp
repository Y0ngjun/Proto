#include <doctest/doctest.h>
#include <unordered_map>

#include "../../src/Core/UUID.h"
#include "../../src/Core/Log.h"
#include "../../src/Core/YAMLHelpers.h"
#include "../../src/Core/KeyCodes.h"
#include "../../src/Core/MouseCodes.h"
#include "../../src/Core/EventBus.h"

// ────────────────────────────────────────────────────────────────────────────
// UUID
// ────────────────────────────────────────────────────────────────────────────

TEST_SUITE("UUID")
{
    TEST_CASE("기본 생성된 UUID는 0이 아님")
    {
        // Arrange & Act
        Proto::UUID uuid;

        // Assert
        CHECK(static_cast<uint64_t>(uuid) != 0);
    }

    TEST_CASE("지정 값으로 생성")
    {
        // Arrange & Act
        Proto::UUID uuid(12345u);

        // Assert
        CHECK(static_cast<uint64_t>(uuid) == 12345u);
    }

    TEST_CASE("두 UUID는 서로 다름")
    {
        // Arrange & Act
        Proto::UUID a;
        Proto::UUID b;

        // Assert
        CHECK(static_cast<uint64_t>(a) != static_cast<uint64_t>(b));
    }

    TEST_CASE("std::hash 동작 확인")
    {
        // Arrange
        Proto::UUID uuid(99999u);

        // Act
        size_t h = std::hash<Proto::UUID>{}(uuid);

        // Assert
        CHECK(h != 0);
    }

    TEST_CASE("unordered_map 키로 사용")
    {
        // Arrange
        std::unordered_map<Proto::UUID, int> map;
        Proto::UUID key(42u);

        // Act
        map[key] = 7;

        // Assert
        REQUIRE(map.count(key) == 1);
        CHECK(map[key] == 7);
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Log
// ────────────────────────────────────────────────────────────────────────────

TEST_SUITE("Log")
{
    TEST_CASE("Clear 후 메시지 없음")
    {
        // Arrange
        Proto::Log::Info("임시 메시지");

        // Act
        Proto::Log::Clear();

        // Assert
        CHECK(Proto::Log::GetMessages().empty());
    }

    TEST_CASE("Info 로그 저장")
    {
        // Arrange
        Proto::Log::Clear();

        // Act
        Proto::Log::Info("정보 메시지");

        // Assert
        REQUIRE(!Proto::Log::GetMessages().empty());
        CHECK(Proto::Log::GetMessages().back().Level == Proto::LogLevel::Info);
    }

    TEST_CASE("Warn 로그 저장")
    {
        // Arrange
        Proto::Log::Clear();

        // Act
        Proto::Log::Warn("경고 메시지");

        // Assert
        REQUIRE(!Proto::Log::GetMessages().empty());
        CHECK(Proto::Log::GetMessages().back().Level == Proto::LogLevel::Warn);
    }

    TEST_CASE("Error 로그 저장")
    {
        // Arrange
        Proto::Log::Clear();

        // Act
        Proto::Log::Error("에러 메시지");

        // Assert
        REQUIRE(!Proto::Log::GetMessages().empty());
        CHECK(Proto::Log::GetMessages().back().Level == Proto::LogLevel::Error);
    }

    TEST_CASE("메시지 내용 일치")
    {
        // Arrange
        Proto::Log::Clear();

        // Act
        Proto::Log::Info("hello");

        // Assert
        REQUIRE(!Proto::Log::GetMessages().empty());
        CHECK(Proto::Log::GetMessages().back().Message == "hello");
    }

    TEST_CASE("타임스탬프 형식")
    {
        // Arrange
        Proto::Log::Clear();

        // Act
        Proto::Log::Info("타임스탬프 테스트");

        // Assert — 형식: "HH:MM:SS" (길이 8, 콜론 포함)
        REQUIRE(!Proto::Log::GetMessages().empty());
        const std::string& ts = Proto::Log::GetMessages().back().Timestamp;
        REQUIRE(ts.size() == 8);
        CHECK(ts[2] == ':');
        CHECK(ts[5] == ':');
    }

    TEST_CASE("여러 로그 누적")
    {
        // Arrange
        Proto::Log::Clear();

        // Act
        Proto::Log::Info("첫 번째");
        Proto::Log::Info("두 번째");
        Proto::Log::Info("세 번째");

        // Assert
        CHECK(Proto::Log::GetMessages().size() == 3);
    }
}

// ────────────────────────────────────────────────────────────────────────────
// YAMLHelpers
// ────────────────────────────────────────────────────────────────────────────

TEST_SUITE("YAMLHelpers")
{
    TEST_CASE("glm::vec3 직렬화")
    {
        // Arrange
        glm::vec3 v(1.0f, 2.0f, 3.0f);
        YAML::Emitter out;

        // Act
        Proto::operator<<(out, v);
        std::string result = out.c_str();

        // Assert
        CHECK(result.find("1") != std::string::npos);
        CHECK(result.find("2") != std::string::npos);
        CHECK(result.find("3") != std::string::npos);
    }

    TEST_CASE("glm::vec3 역직렬화")
    {
        // Arrange
        YAML::Node node = YAML::Load("[4.0, 5.0, 6.0]");
        glm::vec3 v;

        // Act
        bool ok = YAML::convert<glm::vec3>::decode(node, v);

        // Assert
        REQUIRE(ok);
        CHECK(v.x == doctest::Approx(4.0f));
        CHECK(v.y == doctest::Approx(5.0f));
        CHECK(v.z == doctest::Approx(6.0f));
    }

    TEST_CASE("잘못된 YAML vec3 - 요소 수 부족")
    {
        // Arrange
        YAML::Node node = YAML::Load("[1.0, 2.0]");
        glm::vec3 v;

        // Act
        bool ok = YAML::convert<glm::vec3>::decode(node, v);

        // Assert
        CHECK(!ok);
    }

    TEST_CASE("UUID 직렬화")
    {
        // Arrange
        Proto::UUID uuid(777u);

        // Act
        YAML::Node node = YAML::convert<Proto::UUID>::encode(uuid);

        // Assert
        CHECK(node.as<uint64_t>() == 777u);
    }

    TEST_CASE("UUID 역직렬화")
    {
        // Arrange
        YAML::Node node = YAML::Load("12345");
        Proto::UUID uuid;

        // Act
        bool ok = YAML::convert<Proto::UUID>::decode(node, uuid);

        // Assert
        REQUIRE(ok);
        CHECK(static_cast<uint64_t>(uuid) == 12345u);
    }
}

// ────────────────────────────────────────────────────────────────────────────
// KeyCodes / MouseCodes
// ────────────────────────────────────────────────────────────────────────────

TEST_SUITE("KeyCodes")
{
    TEST_CASE("GLFW 키 코드 매핑")
    {
        CHECK(Proto::Key::SPACE   == 32);
        CHECK(Proto::Key::ESCAPE  == 256);
        CHECK(Proto::Key::ENTER   == 257);
        CHECK(Proto::Key::A       == 65);
        CHECK(Proto::Key::Z       == 90);
        CHECK(Proto::Key::D0      == 48);
        CHECK(Proto::Key::D9      == 57);
        CHECK(Proto::Key::F1      == 290);
        CHECK(Proto::Key::F12     == 301);
        CHECK(Proto::Key::LEFT_SHIFT   == 340);
        CHECK(Proto::Key::LEFT_CONTROL == 341);
    }

    TEST_CASE("마우스 버튼 코드 및 별칭")
    {
        CHECK(Proto::Mouse::BUTTON_0 == 0);
        CHECK(Proto::Mouse::BUTTON_7 == 7);
        CHECK(Proto::Mouse::BUTTON_LEFT   == Proto::Mouse::BUTTON_0);
        CHECK(Proto::Mouse::BUTTON_RIGHT  == Proto::Mouse::BUTTON_1);
        CHECK(Proto::Mouse::BUTTON_MIDDLE == Proto::Mouse::BUTTON_2);
        CHECK(Proto::Mouse::BUTTON_LAST   == Proto::Mouse::BUTTON_7);
    }
}

// ────────────────────────────────────────────────────────────────────────────
// EventBus
// ────────────────────────────────────────────────────────────────────────────

namespace
{
    struct TestEvent
    {
        int Value;
    };

    struct OtherEvent
    {
        float Data;
    };
}

TEST_SUITE("EventBus")
{
    TEST_CASE("Subscribe 후 Publish → 콜백 호출")
    {
        // Arrange
        Proto::EventBus bus;
        int received = -1;
        bus.Subscribe<TestEvent>([&](const TestEvent& e) { received = e.Value; });

        // Act
        bus.Publish(TestEvent{ 42 });

        // Assert
        CHECK(received == 42);
    }

    TEST_CASE("Unsubscribe 후 Publish → 콜백 미호출")
    {
        // Arrange
        Proto::EventBus bus;
        int received = -1;
        uint32_t id = bus.Subscribe<TestEvent>([&](const TestEvent& e) { received = e.Value; });

        // Act
        bus.Unsubscribe(id);
        bus.Publish(TestEvent{ 99 });

        // Assert
        CHECK(received == -1);
    }

    TEST_CASE("다중 구독자 모두 호출")
    {
        // Arrange
        Proto::EventBus bus;
        int countA = 0;
        int countB = 0;
        bus.Subscribe<TestEvent>([&](const TestEvent&) { countA++; });
        bus.Subscribe<TestEvent>([&](const TestEvent&) { countB++; });

        // Act
        bus.Publish(TestEvent{ 1 });

        // Assert
        CHECK(countA == 1);
        CHECK(countB == 1);
    }

    TEST_CASE("다른 이벤트 타입은 수신하지 않음")
    {
        // Arrange
        Proto::EventBus bus;
        bool called = false;
        bus.Subscribe<OtherEvent>([&](const OtherEvent&) { called = true; });

        // Act
        bus.Publish(TestEvent{ 7 });

        // Assert
        CHECK(!called);
    }

    TEST_CASE("리스너 없이 Publish해도 오류 없음")
    {
        // Arrange
        Proto::EventBus bus;

        // Act & Assert
        CHECK_NOTHROW(bus.Publish(TestEvent{ 0 }));
    }
}
