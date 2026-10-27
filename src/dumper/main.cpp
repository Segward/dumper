#include <cstdlib>
#include <filesystem>
#include <unistd.h>

int main() {
  std::filesystem::path dumper = std::filesystem::current_path() / "libdumper.dylib";
  const char* app = "/Applications/Roblox.app/Contents/MacOS/RobloxPlayer";

  setenv("DYLD_INSERT_LIBRARIES", dumper.c_str(), 1);
  setenv("DYLD_FORCE_FLAT_NAMESPACE", "1", 1);

  char* args[] = {const_cast<char*>(app), nullptr};
  execv(app, args);

  return 0;
}
