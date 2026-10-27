#ifndef ENGINE_HPP
#define ENGINE_HPP

namespace engine {
  float get_float(std::uintptr_t instance, const std::string& property, std::uintptr_t sub_obj_offset);
  void set_float(std::uintptr_t instance, const std::string& property, std::uintptr_t sub_obj_offset, float value);
}

#endif
