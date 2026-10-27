#ifndef SEGMENTS_HPP
#define SEGMENTS_HPP

namespace segments {
  struct Segment {
    std::uintptr_t start;
    std::uintptr_t end;
  };

  std::vector<Segment> get_writable();
  std::vector<Segment> get_executable();
}

#endif
