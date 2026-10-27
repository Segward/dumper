#ifndef OFFSETS_HPP
#define OFFSETS_HPP

#include <cstdint>

namespace offsets {
  constexpr std::uintptr_t instance_name = 0x68;
  constexpr std::uintptr_t instance_children = 0x70;
  constexpr std::uintptr_t humanoid_walk_speed = 0x1b4;
  constexpr std::uintptr_t humanoid_jump_height = 0x184;
  constexpr std::uintptr_t humanoid_jump_power = 0x188;
  constexpr std::uintptr_t humanoid_health = 0x174;
  constexpr std::uintptr_t humanoid_max_health = 0x18c;
  constexpr std::uintptr_t humanoid_hip_height = 0x178;
  constexpr std::uintptr_t basepart_primitive = 0x168;
  constexpr std::uintptr_t basepart_transparency = 0x110;
  constexpr std::uintptr_t basepart_reflectance = 0x0ec;
}

#endif
