#include <dumper/common.hpp>
#include <dumper/memory.hpp>
#include <dumper/instance.hpp>
#include <dumper/offsets.hpp>

std::string instance::get_name(std::uintptr_t instance) {
  std::uintptr_t name_ptr = memory::read_ptr(instance + offsets::instance_name);
  if (!memory::is_valid_ptr(name_ptr)) return {};
  char name[64] = {};
  if (!memory::read_safe(name_ptr, name, 63)) return {};
  return name;
}

std::vector<std::uintptr_t> instance::get_children(std::uintptr_t instance) {
  std::vector<std::uintptr_t> result;
  std::uintptr_t children_ptr = memory::read_ptr(instance + offsets::instance_children);
  if (!memory::is_valid_ptr(children_ptr)) return result;
  std::uintptr_t begin = memory::read_ptr(children_ptr);
  std::uintptr_t end = memory::read_ptr(children_ptr + 8);
  if (!memory::is_valid_ptr(begin) || !memory::is_valid_ptr(end) || end <= begin) return result;
  int count = (int)((end - begin) / 16);
  for (int i = 0; i < count; i++) {
    std::uintptr_t child = memory::read_ptr(begin + i * 16);
    if (memory::is_valid_ptr(child)) result.push_back(child);
  }
  return result;
}

std::uintptr_t instance::find_first_child(std::uintptr_t instance, const std::string& name) {
  for (std::uintptr_t child : get_children(instance)) {
    if (get_name(child) == name) return child;
  }
  return 0;
}
