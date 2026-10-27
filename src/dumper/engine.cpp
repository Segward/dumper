#include <dumper/common.hpp>
#include <dumper/memory.hpp>
#include <dumper/engine.hpp>

namespace {
  std::uintptr_t find_prop_getset(std::uintptr_t instance, const std::string& property) {
    std::uintptr_t class_desc = memory::read_ptr(instance + 0x18);
    std::uintptr_t arr = memory::read_ptr(class_desc + 0x40);
    uint32_t count = 0;
    memory::read_safe(class_desc + 0x48, &count, 4);
    for (uint32_t i = 0; i < count; i++) {
      std::uintptr_t desc = memory::read_ptr(arr + i * 16);
      std::uintptr_t name_ptr = memory::read_ptr(desc + 0x08);
      char buf[64] = {};
      if (!memory::read_safe(name_ptr, buf, 63)) continue;
      if (property == buf) return memory::read_ptr(desc + 0xa8);
    }
    return 0;
  }
}

float engine::get_float(std::uintptr_t instance, const std::string& property, std::uintptr_t sub_obj_offset) {
  std::uintptr_t getset = find_prop_getset(instance, property);
  if (!getset) return 0.0f;
  auto getter = reinterpret_cast<float(*)(std::uintptr_t)>(memory::read_ptr(getset + 0x08));
  return getter(instance + sub_obj_offset);
}

void engine::set_float(std::uintptr_t instance, const std::string& property, std::uintptr_t sub_obj_offset, float value) {
  std::uintptr_t getset = find_prop_getset(instance, property);
  if (!getset) return;
  auto setter = reinterpret_cast<void(*)(std::uintptr_t, float)>(memory::read_ptr(getset + 0x18));
  setter(instance + sub_obj_offset, value);
}
