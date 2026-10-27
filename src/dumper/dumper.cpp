#include <dumper/scheduler.hpp>
#include <dumper/util.hpp>

namespace {
  void dumper_thread() {
    setbuf(stdout, nullptr);
    const char* image_name = _dyld_get_image_name(1);
    if (!strstr(image_name, "RobloxPlayer")) return;

    std::this_thread::sleep_for(std::chrono::seconds(15));

    std::uintptr_t scheduler = scheduler::find_task_scheduler();
    if (!scheduler) {
      printf("[dumper] TaskScheduler not found\n");
      return;
    }

    std::uintptr_t render_job = scheduler::find_job(scheduler, "RenderJob");
    std::uintptr_t fake_data_model = util::read_ptr(render_job + 0x30);
    std::uintptr_t data_model = fake_data_model + 0x1E8;

    printf("[dumper] TaskScheduler: 0x%lx\n", scheduler);
    printf("[dumper] RenderJob: 0x%lx\n", render_job);
    printf("[dumper] DataModel: 0x%lx \"%s\"\n", data_model, util::read_instance_name(data_model).c_str());

    std::uintptr_t workspace = util::find_first_child(data_model, "Workspace");
    util::print_children(workspace);
  }
}

__attribute__((constructor))
void entry() {
  std::thread(dumper_thread).detach();
}
