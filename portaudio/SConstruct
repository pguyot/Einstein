import sys, os.path

class ConfigurationError(Exception):
    def __init__(self, reason):
        Exception.__init__(self, "Configuration failed: %s" % reason)

def _PackageOption(pkgName, default="yes"):
    return BoolOption("use%s" % pkgName[0].upper() + pkgName[1:], "use %s if available" % (pkgName), default)

def _BoolOption(opt, explanation, default="yes"):
    return BoolOption("enable%s" % opt[0].upper() + opt[1:], explanation, default)

def _DirectoryOption(path, help, default):
    return PathOption(path, help, default)
    # Incompatible with the latest stable SCons
    # return PathOption(path, help, default, PathOption.PathIsDir)

def _EnumOption(opt, explanation, allowedValues, default):
    assert default in allowedValues
    return EnumOption("with%s" % opt[0].upper() + opt[1:], explanation, default, allowed_values=allowedValues)

def getPlatform():
    global __platform
    try: return __platform
    except NameError:
        env = Environment()
        if env["PLATFORM"] == "posix":
            if sys.platform[:5] == "linux":
                __platform = "linux"
            else:
                raise ConfigurationError("Unknown platform %s" % sys.platform)
        else:
            if not env["PLATFORM"] in ("win32", "cygwin") + Posix:
                raise ConfigurationError("Unknown platform %s" % env["PLATFORM"])
            __platform = env["PLATFORM"]

    return __platform

# sunos, aix, hpux, irix, sunos appear to be platforms known by SCons, assuming they're POSIX compliant
Posix = ("linux", "darwin", "sunos", "aix", "hpux", "irix", "sunos")
Windows = ("win32", "cygwin")
env = Environment(CPPPATH="pa_common")

Platform = getPlatform()

opts = Options("options.cache", args=ARGUMENTS)
if Platform in Posix:
    opts.AddOptions(
            _DirectoryOption("prefix", "installation prefix", "/usr/local"),
            _PackageOption("ALSA"),
            _PackageOption("OSS"),
            _PackageOption("JACK"),
            )
elif Platform in Windows:
    if Platform == "cygwin":
        opts.AddOptions(_DirectoryOption("prefix", "installation prefix", "/usr/local"))
        opts.AddOptions(_EnumOption("winAPI", "Windows API to use", ("wmme", "directx", "asio"), "wmme"))

if Platform == "darwin":
    opts.AddOptions(_EnumOption("macAPI", "Mac API to use", ("asio", "core", "sm"), "core"))

opts.AddOptions(
        _BoolOption("shared", "create shared library"),
        _BoolOption("static", "create static library"),
        _BoolOption("debug", "compile with debug symbols"),
        _BoolOption("optimize", "compile with optimization", default="no"),
        _BoolOption("asserts", "runtime assertions are helpful for debugging, but can be detrimental to performance", default="yes"),
        _BoolOption("debugOutput", "enable debug output", default="no"),
        ("customCFlags", "customize compilation of C code", ""),
        )

opts.Update(env)
opts.Save("options.cache", env)

Help(opts.GenerateHelpText(env))

env.SConscriptChdir(False)
SConscript("SConscript", build_dir=".build_scons", exports=["env", "Platform", "Posix", "ConfigurationError"], duplicate=False)
