#include <dumper/scheduler.hpp>
#include <dumper/util.hpp>
#include <dumper/offsets.hpp>

namespace {
  void dumper_thread() {
    setbuf(stdout, nullptr);
    const char* image_name = _dyld_get_image_name(1);
    if (!strstr(image_name, "RobloxPlayer")) return;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::uintptr_t scheduler = scheduler::find_task_scheduler();
    std::uintptr_t render_job = scheduler::find_job(scheduler, "RenderJob");
    std::uintptr_t fake_data_model = util::read_ptr(render_job + 0x30);
    std::uintptr_t data_model = fake_data_model + 0x1E8;

    std::uintptr_t workspace = util::find_first_child(data_model, "Workspace");
    std::uintptr_t player_model = util::find_first_child(workspace, "Segward313131");
    std::uintptr_t humanoid = util::find_first_child(player_model, "Humanoid");

    while (true) {
      util::engine_set_float(humanoid, "WalkSpeed", 0x98, 100.0f);
      util::engine_set_float(humanoid, "JumpHeight", 0x98, 100.0f);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

__attribute__((constructor))
void entry() {
  std::thread(dumper_thread).detach();
}
