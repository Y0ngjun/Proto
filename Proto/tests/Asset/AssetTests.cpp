#include <doctest/doctest.h>
#include "../../src/Asset/AssetManager.h"

namespace
{
	class MockMeshAsset : public Proto::Asset
	{
	public:
		Proto::AssetType GetType() const override { return Proto::AssetType::Mesh; }
	};

	class MockShaderAsset : public Proto::Asset
	{
	public:
		Proto::AssetType GetType() const override { return Proto::AssetType::Shader; }
	};
}

TEST_SUITE("Asset System")
{
	TEST_CASE("빈 상태에서 GetAsset은 nullptr 반환")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();

		// 2. Act (실행)
		auto result = Proto::AssetManager::GetAsset(Proto::UUID(999));

		// 3. Assert (검증)
		CHECK(result.get() == nullptr);
	}

	TEST_CASE("AddAsset 후 GetAsset은 동일 에셋 반환")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();
		auto asset = std::make_shared<MockMeshAsset>();
		asset->Handle = Proto::UUID(1);

		// 2. Act (실행)
		Proto::AssetManager::AddAsset(asset);
		auto result = Proto::AssetManager::GetAsset(Proto::UUID(1));

		// 3. Assert (검증)
		REQUIRE(result.get() != nullptr);
		CHECK(result.get() == asset.get());
	}

	TEST_CASE("nullptr AddAsset은 무시")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();

		// 2. Act & Assert (실행 및 검증) — 크래시 없이 통과
		CHECK_NOTHROW(Proto::AssetManager::AddAsset(nullptr));
		CHECK(Proto::AssetManager::GetAsset(Proto::UUID(1)).get() == nullptr);
	}

	TEST_CASE("없는 UUID로 RemoveAsset은 false 반환")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();

		// 2. Act (실행)
		bool result = Proto::AssetManager::RemoveAsset(Proto::UUID(999));

		// 3. Assert (검증)
		CHECK(result == false);
	}

	TEST_CASE("있는 UUID로 RemoveAsset은 true 반환하고 이후 GetAsset은 nullptr")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();
		auto asset = std::make_shared<MockMeshAsset>();
		asset->Handle = Proto::UUID(1);
		Proto::AssetManager::AddAsset(asset);

		// 2. Act (실행)
		bool removed = Proto::AssetManager::RemoveAsset(Proto::UUID(1));
		auto result = Proto::AssetManager::GetAsset(Proto::UUID(1));

		// 3. Assert (검증)
		CHECK(removed == true);
		CHECK(result.get() == nullptr);
	}

	TEST_CASE("Clear 후 모든 에셋 조회 불가")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();
		auto asset1 = std::make_shared<MockMeshAsset>();
		asset1->Handle = Proto::UUID(1);
		auto asset2 = std::make_shared<MockMeshAsset>();
		asset2->Handle = Proto::UUID(2);
		Proto::AssetManager::AddAsset(asset1);
		Proto::AssetManager::AddAsset(asset2);

		// 2. Act (실행)
		Proto::AssetManager::Clear();

		// 3. Assert (검증)
		CHECK(Proto::AssetManager::GetAsset(Proto::UUID(1)).get() == nullptr);
		CHECK(Proto::AssetManager::GetAsset(Proto::UUID(2)).get() == nullptr);
	}

	TEST_CASE("GetAssetAs 올바른 타입은 캐스팅 성공")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();
		auto asset = std::make_shared<MockMeshAsset>();
		asset->Handle = Proto::UUID(1);
		Proto::AssetManager::AddAsset(asset);

		// 2. Act (실행)
		auto result = Proto::AssetManager::GetAssetAs<MockMeshAsset>(Proto::UUID(1));

		// 3. Assert (검증)
		REQUIRE(result.get() != nullptr);
		CHECK(result->GetType() == Proto::AssetType::Mesh);
	}

	TEST_CASE("GetAssetAs 잘못된 타입은 nullptr 반환")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();
		auto asset = std::make_shared<MockMeshAsset>();
		asset->Handle = Proto::UUID(1);
		Proto::AssetManager::AddAsset(asset);

		// 2. Act (실행)
		auto result = Proto::AssetManager::GetAssetAs<MockShaderAsset>(Proto::UUID(1));

		// 3. Assert (검증)
		CHECK(result.get() == nullptr);
	}

	TEST_CASE("같은 UUID로 AddAsset 재등록 시 덮어쓰기")
	{
		// 1. Arrange (준비)
		Proto::AssetManager::Clear();
		auto asset1 = std::make_shared<MockMeshAsset>();
		asset1->Handle = Proto::UUID(1);
		auto asset2 = std::make_shared<MockShaderAsset>();
		asset2->Handle = Proto::UUID(1);
		Proto::AssetManager::AddAsset(asset1);

		// 2. Act (실행)
		Proto::AssetManager::AddAsset(asset2);
		auto result = Proto::AssetManager::GetAsset(Proto::UUID(1));

		// 3. Assert (검증)
		REQUIRE(result.get() != nullptr);
		CHECK(result.get() == asset2.get());
		CHECK(result->GetType() == Proto::AssetType::Shader);
	}
}
