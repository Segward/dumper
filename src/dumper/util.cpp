#include <dumper/util.hpp>

bool util::safe_read(std::uintptr_t addr, void* out, std::size_t size) {
  vm_size_t read_size = size;
  return vm_read_overwrite(mach_task_self(), addr, size, reinterpret_cast<vm_address_t>(out), &read_size) == KERN_SUCCESS;
}

std::uintptr_t util::read_ptr(std::uintptr_t addr) {
  std::uintptr_t value = 0;
  safe_read(addr, &value, sizeof(value));
  return value;
}

bool util::is_valid_pointer(std::uintptr_t ptr) {
  return ptr > 0x100000000 && ptr < 0x800000000000 && (ptr & 0x7) == 0;
}

std::string util::read_std_string(std::uintptr_t addr) {
  std::uint8_t raw[24] = {};
  if (!safe_read(addr, raw, 24)) return {};
  bool is_long = (raw[23] & 0x80) != 0;
  if (!is_long) {
    std::uint8_t length = raw[23];
    if (length == 0 || length > 22) return {};
    for (int i = 0; i < length; i++)
      if (raw[i] < 32 || raw[i] > 126) return {};
    return {reinterpret_cast<char*>(raw), length};
  }

  std::uintptr_t ptr = 0;
  std::uint64_t size = 0;
  std::memcpy(&ptr, raw, 8);
  std::memcpy(&size, raw + 8, 8);
  if (size == 0 || size > 256 || ptr == 0) return {};
  char buffer[257] = {};
  if (!safe_read(ptr, buffer, size)) return {};
  for (std::uint64_t i = 0; i < size; i++)
    if (buffer[i] < 32 || buffer[i] > 126) return {};
  return {buffer, static_cast<std::size_t>(size)};
}

std::string util::read_instance_name(std::uintptr_t instance) {
  std::uintptr_t name_ptr = read_ptr(instance + 0x68);
  if (!is_valid_pointer(name_ptr)) return {};
  char name[64] = {};
  if (!safe_read(name_ptr, name, 63)) return {};
  return name;
}

std::vector<std::uintptr_t> util::get_children(std::uintptr_t instance) {
  std::vector<std::uintptr_t> result;
  std::uintptr_t children_ptr = read_ptr(instance + 0x70);
  if (!is_valid_pointer(children_ptr)) return result;
  std::uintptr_t begin = read_ptr(children_ptr);
  std::uintptr_t end = read_ptr(children_ptr + 8);
  if (!is_valid_pointer(begin) || !is_valid_pointer(end) || end <= begin) return result;
  int count = (int)((end - begin) / 16);
  for (int i = 0; i < count; i++) {
    std::uintptr_t child = read_ptr(begin + i * 16);
    if (is_valid_pointer(child)) result.push_back(child);
  }
  return result;
}

std::uintptr_t util::find_first_child(std::uintptr_t instance, const std::string& name) {
  std::vector<std::uintptr_t> children = get_children(instance);
  for (std::uintptr_t child : children) {
    if (read_instance_name(child) == name) return child;
  }
  return 0;
}

void util::print_children(std::uintptr_t instance) {
  std::vector<std::uintptr_t> children = get_children(instance);
  printf("[dumper] Children of \"%s\": %zu\n", read_instance_name(instance).c_str(), children.size());
  for (std::uintptr_t child : children) {
    printf("[dumper]   0x%lx \"%s\"\n", child, read_instance_name(child).c_str());
  }
}
