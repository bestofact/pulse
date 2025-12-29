#pragma once

#include "ecs/concepts/dataconcept.h"

#include "ecs/concepts/entityconcept.h"
#include "ecs/utils/meta.h"
#include "foundation/meta.h"

#include <memory>
#include <meta>
#include <array>
#include <bitset>

namespace pulse::ecs
{
	enum class EArchetypeChunkModificationMethod : pulse::u8
	{
		Immediate = 0,
		Deferred = 1,
	};

	template<
		EArchetypeChunkModificationMethod MODIFICATION_METHOD, 
		pulse::ecs::concepts::Data... DATA_TYPES>
	struct ArchetypeChunk
	{
		struct Archetype;
		struct Occupation;

#pragma region --------------------- META -----------------------
		consteval static pulse::u64 get_entity_capacity()
		{
			const auto info = pulse::meta::get_first_arg<DATA_TYPES...>();
			return pulse::ecs::meta::get_entity_capacity(info);
		}

		consteval static pulse::u64 get_data_count()
		{
			return pulse::meta::get_arg_count<DATA_TYPES...>();
		}

		consteval static std::array<std::meta::info, get_data_count()> get_data_types()
		{
			const std::array<std::meta::info, get_data_count()> types = {
				^^DATA_TYPES...
			};
			return types;
		}

		consteval static pulse::u64 get_data_index(std::meta::info in_dataType)
		{
			return pulse::meta::get_arg_index<DATA_TYPES...>(in_dataType);
		}

		consteval static std::meta::info get_archetype_type()
		{
			return ^^Archetype;
		}

		consteval static std::meta::data_member_options construct_data_member_options(std::meta::info in_dataType)
		{
			return std::meta::data_member_options{
				.name = std::meta::identifier_of(in_dataType)
			};
		}

		consteval static std::meta::info construct_archetype_member(std::meta::info in_dataType)
		{
			return std::meta::data_member_spec(
				in_dataType,
				construct_data_member_options(in_dataType)
			);
		}

		consteval static void define_archetype_type()
		{
			const std::array<std::meta::info, get_data_count()> members = {
				construct_archetype_member(^^DATA_TYPES)...
			};

			std::meta::define_aggregate(get_archetype_type(), members);
		}

		consteval static std::meta::info get_occupation_type()
		{
			return ^^Occupation;
		}

		consteval static std::meta::info construct_occupation_member(std::meta::info in_dataType)
		{
			const auto type = ^^std::bitset<get_entity_capacity()>;
			return std::meta::data_member_spec(
				type,
				construct_data_member_options(in_dataType)
			);
		}

		consteval static void define_occupation_type()
		{
			const std::array<std::meta::info, get_data_count()> members = {
				construct_occupation_member(^^DATA_TYPES)...
			};

			std::meta::define_aggregate(get_occupation_type(), members);
		}

		consteval
		{
			define_archetype_type();
			define_occupation_type();
		}

		consteval static std::meta::info get_chunk_type()
		{
			const auto array = ^^std::array;
			const auto chunk = get_archetype_type();
			const auto count = std::meta::reflect_constant(get_entity_capacity());
			const auto type = std::meta::substitute(array, { chunk, count });
			return type;
		}

		consteval static std::meta::info get_chunk_member(std::meta::info in_dataType)
		{
			constexpr auto chunkType = get_archetype_type();
			constexpr auto ctx = std::meta::access_context::unchecked();

			const auto index = get_data_index(in_dataType);
			const auto member = std::meta::nonstatic_data_members_of(chunkType, ctx)[index];
			return member;
		}

		consteval static std::meta::info get_occupation_member(std::meta::info in_dataType)
		{
			constexpr auto occupationType = get_occupation_type();
			constexpr auto ctx = std::meta::access_context::unchecked();

			const auto index = get_data_index(in_dataType);
			const auto member = std::meta::nonstatic_data_members_of(occupationType, ctx)[index];
			return member;
		}

#pragma endregion

		consteval static pulse::ecs::EArchetypeChunkModificationMethod get_modification_method()
		{
			return MODIFICATION_METHOD;
		}

		consteval static pulse::u32 get_occupation_count()
		{
			return static_cast<pulse::u32>(get_modification_method()) + 1;
		}

		consteval static pulse::u32 get_occupation_index(pulse::ecs::EArchetypeChunkModificationMethod in_method)
		{
			return static_cast<pulse::u64>(in_method);
		}

private:
		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		std::bitset<get_entity_capacity()> get_occupation(const pulse::u64 in_index) const
		{
			std::bitset<get_entity_capacity()> result;
			result.set();

			PULSE_FOR_EACH_VARIADIC_ARGUMENT(
				result &= m_occupations[in_index].[:get_occupation_member(^^TARGET_DATA_TYPES):]
			);

			return result;
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		bool test_occupation(const ENTITY_TYPE in_entity, const pulse::u64 in_index) const
		{
			const auto occupation = get_occupation<TARGET_DATA_TYPES...>();
			return occupation[in_index].test(in_entity.get_index());
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set_occupation(const ENTITY_TYPE in_entity, const pulse::u64 in_index)
		{
			PULSE_FOR_EACH_VARIADIC_ARGUMENT(
				m_occupations[in_index].[:get_occupation_member(^^TARGET_DATA_TYPES):].set(in_entity.get_index())
			);
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void reset_occupation(const ENTITY_TYPE in_entity, const pulse::u64 in_index)
		{
			PULSE_FOR_EACH_VARIADIC_ARGUMENT(
				m_occupations[in_index].[:get_occupation_member(^^TARGET_DATA_TYPES):].reset(in_entity.get_index())
			);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set_occupations(const pulse::u64 in_index)
		{
			PULSE_FOR_EACH_VARIADIC_ARGUMENT(
				m_occupations[in_index].[:get_occupation_member(^^TARGET_DATA_TYPES):].set()
			);
		}

		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void reset_occupations(const pulse::u64 in_index)
		{
			PULSE_FOR_EACH_VARIADIC_ARGUMENT(
				m_occupations[in_index].[:get_occupation_member(^^TARGET_DATA_TYPES):].reset()
			);
		}

public:
		template<pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		std::bitset<get_entity_capacity()> get_occupation() const
		{
			return get_occupation<TARGET_DATA_TYPES...>(get_occupation_index(EArchetypeChunkModificationMethod::Immediate));
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		bool test_occupation(const ENTITY_TYPE in_entity) const
		{
			return test_occupation<ENTITY_TYPE, TARGET_DATA_TYPES...>(in_entity, get_occupation_index(EArchetypeChunkModificationMethod::Immediate));
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set_occupation(const ENTITY_TYPE in_entity)
		{
			set_occupation<ENTITY_TYPE, TARGET_DATA_TYPES...>(in_entity, get_occupation_index(get_modification_method()));
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void reset_occupation(const ENTITY_TYPE in_entity)
		{
			reset_occupation<ENTITY_TYPE, TARGET_DATA_TYPES...>(in_entity, get_occupation_index(get_modification_method()));
		}

		template<
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void set_occupations()
		{
			set_occupations<TARGET_DATA_TYPES...>(get_occupation_index(get_modification_method()));
		}

		template<
			pulse::ecs::concepts::Data... TARGET_DATA_TYPES>
		void reset_occupations()
		{
			reset_occupations<TARGET_DATA_TYPES...>(get_occupation_index(get_modification_method()));
		}

		void set_occupations()
		{
			set_occupations<DATA_TYPES...>();
		}

		void reset_occupations()
		{
			reset_occupations<DATA_TYPES...>();
		}

		void apply_occupations()
		{
			if constexpr(get_modification_method() == EArchetypeChunkModificationMethod::Deferred)
			{
				const auto& deferred = m_occupations[get_occupation_index(EArchetypeChunkModificationMethod::Deferred)];
				auto& immediate = m_occupations[get_occupation_index(EArchetypeChunkModificationMethod::Immediate)];
				immediate = deferred;
			}
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data DATA_TYPE>
		const DATA_TYPE& get_data(const ENTITY_TYPE in_entity) const
		{
			constexpr auto member = get_chunk_member(^^DATA_TYPE);
			const u64 index = in_entity.get_index();
			const auto& archetype = (*m_chunk)[index];
			const auto& data = archetype.[:member:];
			return data;
		}

		template<
			pulse::ecs::concepts::Entity ENTITY_TYPE,
			pulse::ecs::concepts::Data DATA_TYPE>
		DATA_TYPE& get_data_mutable(const ENTITY_TYPE in_entity)
		{
			constexpr auto member = get_chunk_member(^^DATA_TYPE);
			const u64 index = in_entity.get_index();
			auto& archetype = (*m_chunk)[index];
			auto& data = archetype.[:member:];
			return data;
		}

private:
		[:get_occupation_type():] m_occupations[get_occupation_count()];
		std::unique_ptr<typename [:get_chunk_type():]> m_chunk = std::make_unique<typename [:get_chunk_type():]>();
	};
}