#include <dumper/common.hpp>
#include <dumper/memory.hpp>

bool memory::read_safe(std::uintptr_t address, void* out, std::size_t size) {
  vm_size_t read_size = size;
  return vm_read_overwrite(mach_task_self(), address, size, reinterpret_cast<vm_address_t>(out), &read_size) == KERN_SUCCESS;
}

std::uintptr_t memory::read_ptr(std::uintptr_t address) {
  std::uintptr_t value = 0;
  read_safe(address, &value, sizeof(value));
  return value;
}

bool memory::is_valid_ptr(std::uintptr_t ptr) {
  return ptr > 0x100000000 && ptr < 0x800000000000 && (ptr & 0x7) == 0;
}

std::string memory::read_std_string(std::uintptr_t address) {
  std::uint8_t raw[24] = {};
  if (!read_safe(address, raw, 24)) return {};
  bool is_long = (raw[23] & 0x80) != 0;
  if (!is_long) {
    std::uint8_t length = raw[23];
    if (length == 0 || length > 22) return {};
    for (int i = 0; i < length; i++) {
      if (raw[i] < 32 || raw[i] > 126) return {};
    }
    return {reinterpret_cast<char*>(raw), length};
  }

  std::uintptr_t ptr = 0;
  std::uint64_t size = 0;
  std::memcpy(&ptr, raw, 8);
  std::memcpy(&size, raw + 8, 8);
  if (size == 0 || size > 256 || ptr == 0) return {};
  char buffer[257] = {};
  if (!read_safe(ptr, buffer, size)) return {};
  for (std::uint64_t i = 0; i < size; i++) {
    if (buffer[i] < 32 || buffer[i] > 126) return {};
  }
  return {buffer, static_cast<std::size_t>(size)};
}
