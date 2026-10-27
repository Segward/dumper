#include <dumper/common.hpp>
#include <dumper/memory.hpp>
#include <dumper/instance.hpp>
#include <dumper/engine.hpp>
#include <dumper/scheduler.hpp>
#include <dumper/heartbeat.hpp>

namespace {
  void dumper_thread() {
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::uintptr_t scheduler = scheduler::find_task_scheduler();
    std::uintptr_t render_job = scheduler::find_job(scheduler, "RenderJob");
    std::uintptr_t fake_data_model = memory::read_ptr(render_job + 0x30);
    std::uintptr_t data_model = fake_data_model + 0x1E8;

    std::uintptr_t workspace = instance::find_first_child(data_model, "Workspace");
    std::uintptr_t player_model = instance::find_first_child(workspace, "Segward313131");
    std::uintptr_t humanoid = instance::find_first_child(player_model, "Humanoid");

    heartbeat::connect([humanoid]() {
      if (!humanoid) return;
      engine::set_float(humanoid, "WalkSpeed", 0x98, 100.0f);
      engine::set_float(humanoid, "JumpHeight", 0x98, 50.0f);
    });

    heartbeat::hook(scheduler);
  }
}

__attribute__((constructor))
void entry() {
  setbuf(stdout, nullptr);
  const char* image_name = _dyld_get_image_name(1);
  if (!strstr(image_name, "RobloxPlayer")) return;
  std::thread(dumper_thread).detach();
}
