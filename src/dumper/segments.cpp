#include <dumper/segments.hpp>

namespace {
  std::vector<segments::Segment> get_segments(std::uint32_t protection_flag) {
    std::vector<segments::Segment> result;
    const struct mach_header_64* header = reinterpret_cast<const struct mach_header_64*>(_dyld_get_image_header(1));
    if (!header) return result;

    std::uintptr_t base = reinterpret_cast<std::uintptr_t>(header);
    const std::uint8_t* cmd_ptr = reinterpret_cast<const std::uint8_t*>(header + 1);

    for (std::uint32_t i = 0; i < header->ncmds; i++) {
      const struct load_command* command = reinterpret_cast<const struct load_command*>(cmd_ptr);
      if (command->cmd == LC_SEGMENT_64) {
        const struct segment_command_64* segment =
            reinterpret_cast<const struct segment_command_64*>(cmd_ptr);
        if (segment->initprot & protection_flag) {
          std::uintptr_t start = base + segment->vmaddr - 0x100000000;
          result.push_back({start, start + segment->vmsize});
        }
      }

      cmd_ptr += command->cmdsize;
    }

    return result;
  }
}

std::vector<segments::Segment> segments::get_writable() {
  return get_segments(VM_PROT_WRITE);
}

std::vector<segments::Segment> segments::get_executable() {
  return get_segments(VM_PROT_EXECUTE);
}
