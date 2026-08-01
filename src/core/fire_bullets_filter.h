#pragma once

#include <cstdint>

namespace cs2fow
{

inline constexpr uint32_t k_fire_bullets_message_id = 452;
inline constexpr int k_fire_bullets_player_field_number = 6;
inline constexpr uint32_t k_invalid_packed_entity_handle = 0x00ffffff;

struct fire_bullets_recipient_split
{
	uint64_t normal {};
	uint64_t sanitized {};
};

inline fire_bullets_recipient_split split_fire_bullets_recipients(uint64_t recipients, uint64_t hidden)
{
	hidden &= recipients;
	return {recipients & ~hidden, hidden};
}

} // namespace cs2fow
