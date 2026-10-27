#ifndef HEARTBEAT_HPP
#define HEARTBEAT_HPP

namespace heartbeat {
  void hook(std::uintptr_t scheduler);
  void connect(std::function<void()> fn);
}

#endif
