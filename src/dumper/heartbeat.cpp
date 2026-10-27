#include <dumper/common.hpp>
#include <dumper/memory.hpp>
#include <dumper/heartbeat.hpp>
#include <dumper/scheduler.hpp>
#include <libkern/OSCacheControl.h>
#include <mach/mach_vm.h>

namespace {
  using step_fn = uint64_t(*)(void*, void*);
  static step_fn g_original_step = nullptr;
  static std::vector<std::function<void()>> g_callbacks = {};

  uint64_t step_hook(void* job, void* arg2) {
    for (auto fn : g_callbacks)
      fn();
    return g_original_step(job, arg2);
  }

  std::uintptr_t hook_function(std::uintptr_t target, void* hook) {
    constexpr size_t kJumpLen = 16;
    static const uint8_t kJumpBytes[16] = {
      0x43, 0x00, 0x00, 0x58,
      0x60, 0x00, 0x1F, 0xD6,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    mach_vm_address_t tramp = 0;
    mach_vm_allocate(mach_task_self(), &tramp, kJumpLen * 2, VM_FLAGS_ANYWHERE);
    mach_vm_protect(mach_task_self(), tramp, kJumpLen * 2, false, VM_PROT_READ | VM_PROT_WRITE);

    memcpy(reinterpret_cast<void*>(tramp), reinterpret_cast<void*>(target), kJumpLen);
    memcpy(reinterpret_cast<void*>(tramp + kJumpLen), kJumpBytes, kJumpLen);
    *reinterpret_cast<uint64_t*>(tramp + kJumpLen * 2 - 8) = target + kJumpLen;
    mach_vm_protect(mach_task_self(), tramp, kJumpLen * 2, false, VM_PROT_READ | VM_PROT_EXECUTE);
    sys_icache_invalidate(reinterpret_cast<void*>(tramp), kJumpLen * 2);

    mach_vm_protect(mach_task_self(), target, kJumpLen, false,
                    VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
    uint8_t patch[16];
    memcpy(patch, kJumpBytes, kJumpLen);
    *reinterpret_cast<uint64_t*>(patch + 8) = reinterpret_cast<uint64_t>(hook);
    memcpy(reinterpret_cast<void*>(target), patch, kJumpLen);
    mach_vm_protect(mach_task_self(), target, kJumpLen, false,
                    VM_PROT_READ | VM_PROT_EXECUTE);
    sys_icache_invalidate(reinterpret_cast<void*>(target), kJumpLen);

    return tramp;
  }
}

void heartbeat::hook(std::uintptr_t scheduler) {
  std::uintptr_t job = scheduler::find_job(scheduler, "Heartbeat");
  std::uintptr_t vtable = memory::read_ptr(job);
  std::uintptr_t step = memory::read_ptr(vtable + 9 * 8);
  printf("[heartbeat] job=0x%lx step=0x%lx\n", job, step);

  g_original_step = reinterpret_cast<step_fn>(hook_function(step, reinterpret_cast<void*>(step_hook)));
  printf("[heartbeat] hooked, trampoline=%p\n", (void*)g_original_step);
}

void heartbeat::connect(std::function<void()> fn) {
  g_callbacks.push_back(fn);
}
