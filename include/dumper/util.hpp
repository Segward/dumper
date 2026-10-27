#ifndef UTIL_HPP
#define UTIL_HPP

namespace util {
  bool safe_read(std::uintptr_t addr, void* out, std::size_t size);
  std::uintptr_t read_ptr(std::uintptr_t addr);
  bool is_valid_pointer(std::uintptr_t ptr);
  std::string read_std_string(std::uintptr_t addr);
  std::string read_instance_name(std::uintptr_t instance);
  std::vector<std::uintptr_t> get_children(std::uintptr_t instance);
  std::uintptr_t find_first_child(std::uintptr_t instance, const std::string& name);
  void print_children(std::uintptr_t instance);
}

#endif
