#ifndef INSTANCE_HPP
#define INSTANCE_HPP

namespace instance {
  std::string get_name(std::uintptr_t instance);
  std::vector<std::uintptr_t> get_children(std::uintptr_t instance);
  std::uintptr_t find_first_child(std::uintptr_t instance, const std::string& name);
}

#endif
