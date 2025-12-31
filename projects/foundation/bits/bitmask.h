#pragma once

#include "foundation/types.h"
#include "foundation/limits.h"

#include <bitset>
#include <meta>

namespace pulse
{
	template<u32 COUNT>
	struct BitMask
	{
		static_assert(COUNT > 0);

		consteval static u32 get_bit_count()
		{
			return COUNT;
		}

		consteval static u32 get_word_bit_count()
		{
			return 64;
		}

		consteval static u32 get_word_count()
		{
			return (get_bit_count() + get_word_bit_count() - 1) / get_word_bit_count();
		}

		consteval static u64 get_one()
		{
			return 1ull;
		}

		consteval static u64 get_zero()
		{
			return 0ull;
		}

		consteval static u64 get_max()
		{
			return ~get_zero();
		}

		constexpr static u32 get_word_index(u32 in_bit)
		{
			return in_bit / get_word_bit_count();
		}

		constexpr static u32 get_bit_index(u32 in_bit)
		{
			return in_bit % get_word_bit_count();
		}

		// ---- single-bit ops ----
		bool test(u32 in_bit) const
		{
			const auto word = m_words[get_word_index(in_bit)];
			const auto bit = get_bit_index(in_bit);
			return ( word >> bit ) & get_one();
		}

		void set(u32 in_bit)
		{
			const auto index = get_bit_index(in_bit);
			auto& word = m_words[get_word_index(in_bit)];
			word |= (get_one() << index);
		}

		void reset(u32 in_bit)
		{
			const auto index = get_bit_index(in_bit);
			auto& word = m_words[get_word_index(in_bit)];
			word &= ~(1ull << index);
		}

		// ---- bulk operations ----
		void set()
		{
			template for(u32 index = 0; index < get_word_count(); ++index)
			{
				m_words[index] = get_max();
			}
		}

		void reset()
		{
			template for(u32 index = 0; index < get_word_count(); ++index)
			{
				m_words[index] = get_zero();
			}
		}

		// ---- queries ----
		bool any() const
		{
			u64 accumulated = 0;
			template for(u32 index = 0; index < get_word_count(); ++index)
			{
				const auto word = m_words[index];
				accumulated |= word;
			}

			return accumulated != 0;
		}

		bool none() const 
		{
			return !any(); 
		}

		bool all() const
		{
			const u64 full = pulse::max<u64>();
			u64 accumulated = full;
			template for(u32 index = 0; index < get_word_count(); ++index)
			{
				const auto word = m_words[index];
				accumulated &= word;
			}
			return accumulated == full;
		}

		u32 pop_count() const
		{
			u32 count = 0;
			template for(u32 index = 0; index < get_word_count(); ++index)
			{
				const auto word = m_words[index];
				count += static_cast<u32>(std::popcount(word));
			}
			return count;
		}

		// ---- find helpers ----
	    // Return N if not found.
	    u32 find_first_one() const
	    {
	    	return find_next_one(0);
	    }

	    u32 find_first_zero() const
	    {
	    	return find_next_zero(0);
	    }

	    // Find first 1-bit at or after `from`.
	    u32 find_next_one(u32 in_fromBit) const
	    {
	        if(in_fromBit >= get_bit_count())
	       	{
	       		return get_bit_count();
	       	}

	       	const auto wordIndex = get_word_index(in_fromBit);
	       	const auto bitIndex = get_bit_index(in_fromBit);

	        // First word: mask off bits below from bit
	        const auto remaining  = (m_words[wordIndex]) & (get_max() << bitIndex);
	        if(remaining != 0)
	        {
	        	const auto trailing = static_cast<u32>(std::countr_zero(remaining));
	            return wordIndex * get_word_bit_count() + trailing;
	        }

	        // Remaining full words
	        for(u32 index = wordIndex + 1; index < get_word_count(); ++index)
	        {
	            const auto word = m_words[index];
	            if(word != 0)
	            {
	            	const auto trailing = static_cast<u32>(std::countr_zero(word));
	                return index * get_word_bit_count() + trailing;
	            }
	        }

	        return get_bit_count();
	    }

	    // Find first 0-bit at or after `from`.
	    u32 find_next_zero(u32 in_fromBit) const
	    {
	        if(in_fromBit >= get_bit_count())
	       	{
	       		return get_bit_count();
	       	}

	       	const auto wordIndex = get_word_index(in_fromBit);
	       	const auto bitIndex = get_bit_index(in_fromBit);

	        // First word: invert, apply valid bits, then mask off below bi
	        const auto inverse = (~m_words[wordIndex]) & (get_max() << bitIndex);
	        if(inverse != 0)
	        {
	        	const auto trailing = static_cast<u32>(std::countr_zero(inverse));
	            return wordIndex * get_word_bit_count() + trailing;
	        }

	        // Remaining words
	        for(u32 index = wordIndex + 1; index < get_word_count(); ++index)
	        {
	            const auto wordInverse = ~m_words[index];
	            if(wordInverse != 0)
	            {
	                const auto trailing = static_cast<u32>(std::countr_zero(wordInverse));
	                return index * get_word_bit_count() + trailing;
	            }
	        }

	        return get_bit_count();
	    }

	    // ---- in-place ops ----
		constexpr BitMask& operator&=(const BitMask& rhs)
		{
		    template for(u32 index = 0; index < get_word_count(); ++index)
	        {
	        	m_words[index] &= rhs.m_words[index];
	        }
		    return *this;
		}

		constexpr BitMask& operator|=(const BitMask& rhs)
		{
			template for(u32 index = 0; index < get_word_count(); ++index)
	        {
	        	m_words[index] |= rhs.m_words[index];
	        }
	        return *this;
		}

		constexpr BitMask& operator^=(const BitMask& rhs)
		{
		    template for(u32 index = 0; index < get_word_count(); ++index)
	        {
	        	m_words[index] ^= rhs.m_words[index];
	        }
	        return *this;
		}

		// ---- unary ----
		constexpr BitMask operator~() const
		{
		    BitMask out;
		    template for(u32 index = 0; index < get_word_count(); ++index)
		    {
		        out.m_words[index] = ~m_words[index];
		    }
		    return out;
		}

		// ---- binary ----
		friend constexpr BitMask operator&(BitMask in_lhs, const BitMask& in_rhs)
		{
		    in_lhs &= in_rhs;
		    return in_lhs;
		}

		friend constexpr BitMask operator|(BitMask in_lhs, const BitMask& in_rhs)
		{
		    in_lhs |= in_rhs;
		    return in_lhs;
		}

		friend constexpr BitMask operator^(BitMask in_lhs, const BitMask& in_rhs)
		{
		    in_lhs ^= in_rhs;
		    return in_lhs;
		}

		// ---- equality ----
		friend constexpr bool operator==(const BitMask& in_lhs, const BitMask& in_rhs)
		{
		    template for(u32 index = 0; index < get_word_count(); ++index)
		    {
		        if(in_lhs.m_words[index] != in_rhs.m_words[index])
		        {
		        	return false;
		        }
		    }
		    return true;
		}

		friend constexpr bool operator!=(const BitMask& in_lhs, const BitMask& in_rhs)
		{
		    return !(in_lhs == in_rhs);
		}

	private:
		u64 m_words[get_word_count()] = {};
	};
}