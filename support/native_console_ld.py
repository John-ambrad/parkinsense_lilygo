# Force console subsystem on Windows so the linker expects main() not WinMain
Import("env")
env.Prepend(LINKFLAGS=["-mconsole", "-Wl,-subsystem,console"])
