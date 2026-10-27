#include <dumper/common.hpp>
#include <dumper/memory.hpp>
#include <dumper/scheduler.hpp>
#include <dumper/segments.hpp>

namespace {
  constexpr int kJobsBegin = 0xD0;
  constexpr int kJobsEnd = 0xD8;
  constexpr int kJobStride = 16;
  constexpr int kJobName = 0x18;
}

std::uintptr_t scheduler::find_task_scheduler() {
  std::vector<segments::Segment> segments = segments::get_writable();
  for (const segments::Segment& segment : segments) {
    for (std::uintptr_t addr = segment.start; addr < segment.end; addr += 8) {
      std::uintptr_t candidate = memory::read_ptr(addr);
      if (!memory::is_valid_ptr(candidate)) continue;
      std::uintptr_t begin = memory::read_ptr(candidate + kJobsBegin);
      std::uintptr_t end = memory::read_ptr(candidate + kJobsEnd);
      if (!memory::is_valid_ptr(begin) || !memory::is_valid_ptr(end)) continue;
      if (end <= begin) continue;
      std::uintptr_t size = end - begin;
      if (size % kJobStride != 0 || size > 4096) continue;
      int count = (int)(size / kJobStride);
      if (count < 10 || count > 200) continue;
      for (int i = 0; i < count; i++) {
        std::uintptr_t job = memory::read_ptr(begin + i * kJobStride);
        if (!memory::is_valid_ptr(job)) continue;
        if (memory::read_std_string(job + kJobName) == "Heartbeat") return candidate;
      }
    }
  }
  return 0;
}

std::uintptr_t scheduler::find_job(std::uintptr_t scheduler, const std::string& name) {
  std::uintptr_t begin = memory::read_ptr(scheduler + kJobsBegin);
  std::uintptr_t end = memory::read_ptr(scheduler + kJobsEnd);
  int count = (int)((end - begin) / kJobStride);
  for (int i = 0; i < count; i++) {
    std::uintptr_t job = memory::read_ptr(begin + i * kJobStride);
    if (!memory::is_valid_ptr(job)) continue;
    if (memory::read_std_string(job + kJobName) == name) return job;
  }
  return 0;
}

void scheduler::dump_jobs(std::uintptr_t scheduler) {
  std::uintptr_t begin = memory::read_ptr(scheduler + kJobsBegin);
  std::uintptr_t end = memory::read_ptr(scheduler + kJobsEnd);
  int count = (int)((end - begin) / kJobStride);
  printf("[dumper] Jobs: %d\n", count);
  for (int i = 0; i < count; i++) {
    std::uintptr_t job = memory::read_ptr(begin + i * kJobStride);
    if (!memory::is_valid_ptr(job)) continue;
    std::string name = memory::read_std_string(job + kJobName);
    printf("[dumper]   [%d] 0x%lx \"%s\"\n", i, job, name.c_str());
  }
}
