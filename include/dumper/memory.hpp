#ifndef MEMORY_HPP
#define MEMORY_HPP

namespace memory {
  bool read_safe(std::uintptr_t address, void* out, std::size_t size);
  std::uintptr_t read_ptr(std::uintptr_t address);
  bool is_valid_ptr(std::uintptr_t ptr);
  std::string read_std_string(std::uintptr_t address);
}

#endif
