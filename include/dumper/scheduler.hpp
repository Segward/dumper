#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

namespace scheduler {
  std::uintptr_t find_task_scheduler();
  std::uintptr_t find_job(std::uintptr_t scheduler, const std::string& name);
  void dump_jobs(std::uintptr_t scheduler);
}

#endif
