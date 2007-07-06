import os.path, copy, sys

def checkSymbol(conf, header, library=None, symbol=None, autoadd=True, critical=False, pkgName=None):
    env = conf.env
    if library is None:
        library = "c"   # Standard library
        autoadd = False

    if pkgName is not None:
        origLibPath = copy.copy(env.get("LIBPATH", None))
        origLibs = copy.copy(env.get("LIBS", None))
        origLinkFlags = copy.copy(env.get("LINKFLAGS", None))
        origCppFlags = copy.copy(env.get("CPPFLAGS", None))
        origCppPath = copy.copy(env.get("CPPPATH", None))
        origCcFlags = copy.copy(env.get("CCFLAGS", None))
        origAsFlags = copy.copy(env.get("ASFLAGS", None))

        try: env.ParseConfig("pkg-config --silence-errors %s --cflags --libs" % pkgName)
        except: pass
        else:
            # I see no other way of checking that the parsing succeeded, if it did add no more linking parameters
            if env["LIBS"] != origLibs:
                autoadd = False

    try:
        if not conf.CheckCHeader(header, include_quotes="<>"):
            raise ConfigurationError("missing header %s" % header)
        if symbol is not None and not conf.CheckLib(library, symbol, language="C", autoadd=autoadd):
            raise ConfigurationError("missing symbol %s in library %s" % (symbol, library))
    except ConfigurationError:
        if pkgName is not None:
            # Restore any changes made by ParseConfig
            if origLibPath is not None:
                env["LIBPATH"] = origLibPath
            if origLibs is not None:
                env["LIBS"] = origLibs
            if origLinkFlags is not None:
                env["LINKFLAGS"] = origLinkFlags
            if origCppFlags is not None:
                env["CPPFLAGS"] = origCppFlags
            if origCppPath is not None:
                env["CPPPATH"] = origCppPath
            if origCcFlags is not None:
                env["CCFLAGS"] = origCcFlags
            if origAsFlags is not None:
                env["ASFLAGS"] = origAsFlags

        if not critical:
            return False
        raise

    return True

Import("env", "Platform", "Posix", "ConfigurationError")

# Store all signatures in one file, otherwise .sconsign files will get installed along with our own files
env.SConsignFile()

neededLibs = []
optionalImpls = {}
if Platform in Posix:
    neededLibs += [("pthread", "pthread.h", "pthread_create"), ("m", "math.h", "sin")]
    if env["useALSA"]:
        optionalImpls["ALSA"] = ("asound", "alsa/asoundlib.h", "snd_pcm_open")
    if env["useJACK"]:
        optionalImpls["JACK"] = ("jack", "jack/jack.h", "jack_client_new")
    if env["useOSS"]:
        # TODO: It looks like the prefix for soundcard.h depends on the platform
        optionalImpls["OSS"] = ("oss", "sys/soundcard.h", None)
else:
    raise ConfigurationError("unknown platform %s" % Platform)

if Platform == "darwin":
    env.Append(LINKFLAGS=["-framework CoreAudio", "-framework AudioToolBox"])
    env.Append(CPPDEFINES=["PA_USE_COREAUDIO"])
elif Platform == "cygwin":
    env.Append(LIBS=["winmm"])
elif Platform == "irix":
    neededLibs +=  [("audio", "dmedia/audio.h", "alOpenPort"), ("dmedia", "dmedia/dmedia.h", "dmGetUST")]
    env.Append(CPPDEFINES=["PA_USE_SGI"])

def CheckCTypeSize(context, tp):
    context.Message("Checking the size of C type %s..." % tp)
    ret = context.TryRun("""
#include <stdio.h>

int main() {
    printf("%%d", sizeof(%s));
    return 0;
}
""" % tp, ".c")
    if not ret[0]:
        context.Result(" Couldn't obtain size of type %s!" % tp)
        return None

    assert ret[1]
    sz = int(ret[1])
    context.Result("%d" % sz)
    return sz

if sys.byteorder == "little":
    env.Append(CPPDEFINES=["PA_LITTLE_ENDIAN"])
elif sys.byteorder == "big":
    env.Append(CPPDEFINES=["PA_BIG_ENDIAN"])
else:
    raise ConfigurationError("unknown byte order: %s" % sys.byteorder)
if env["enableDebugOutput"]:
    env.Append(CPPDEFINES=["PA_ENABLE_DEBUG_OUTPUT"])

# Start configuration

conf = env.Configure(log_file="sconf.log", custom_tests={"CheckCTypeSize": CheckCTypeSize})
env.Append(CPPDEFINES=["SIZEOF_SHORT=%d" % conf.CheckCTypeSize("short")])
env.Append(CPPDEFINES=["SIZEOF_INT=%d" % conf.CheckCTypeSize("int")])
env.Append(CPPDEFINES=["SIZEOF_LONG=%d" % conf.CheckCTypeSize("long")])
if checkSymbol(conf, "time.h", "rt", "clock_gettime"):
    env.Append(CPPDEFINES=["HAVE_CLOCK_GETTIME"])
if checkSymbol(conf, "time.h", "nanosleep"):
    env.Append(CPPDEFINES=["HAVE_NANOSLEEP"])

for lib, hdr, sym in neededLibs:
    checkSymbol(conf, hdr, lib, sym, critical=True)
for name, val in optionalImpls.items():
    lib, hdr, sym = val
    if checkSymbol(conf, hdr, lib, sym, critical=False, pkgName=name.lower()):
        env.Append(CPPDEFINES=["PA_USE_%s=1" % name.upper()])
    else:
        del optionalImpls[name]

# Configuration finished
env = conf.Finish()

CommonSources = [os.path.join("pa_common", f) for f in "pa_allocation.c pa_converters.c pa_cpuload.c pa_dither.c pa_front.c \
        pa_process.c pa_skeleton.c pa_stream.c pa_trace.c".split()]

ImplSources = []
if Platform in Posix:
    env.AppendUnique(LINKFLAGS=["-pthread"])
    BaseCFlags = "-Wall -pedantic -pipe -pthread"
    DebugCFlags = "-g"
    OptCFlags  = "-O2"
    ImplSources += [os.path.join("pa_unix", f) for f in "pa_unix_hostapis.c pa_unix_util.c".split()]

if "ALSA" in optionalImpls:
    ImplSources.append(os.path.join("pa_linux_alsa", "pa_linux_alsa.c"))
if "JACK" in optionalImpls:
    ImplSources.append(os.path.join("pa_jack", "pa_jack.c"))
if "OSS" in optionalImpls:
    ImplSources.append(os.path.join("pa_unix_oss", "pa_unix_oss.c"))

env["CCFLAGS"] = BaseCFlags
if env["enableDebug"]:
    env["CCFLAGS"] += " " + DebugCFlags
if env["enableOptimize"]:
    env["CCFLAGS"] += " " + OptCFlags
if not env["enableAsserts"]:
    env.Append(CPPDEFINES=["-DNDEBUG"])
if env["customCFlags"]:
    env["CCFLAGS"] += " " + env["customCFlags"]

sources = CommonSources + ImplSources

if env["enableShared"]:
    sharedLib = env.SharedLibrary(target="portaudio", source=sources)
else:
    sharedLib = None
staticLib = env.StaticLibrary(target="portaudio", source=sources)

if Platform in Posix:
    prefix = env["prefix"]
    env.Alias("install", env.Install(os.path.join(prefix, "include"), os.path.join("pa_common", "portaudio.h")))

    if env["enableStatic"]:
        env.Alias("install", env.Install(os.path.join(prefix, "lib"), staticLib))

    def symlink(env, target, source):
        trgt = str(target[0])
        src = str(source[0])
        print trgt, src

        if os.path.exists(trgt):
            os.remove(trgt)
        os.symlink("libportaudio.so.0.0.19", trgt)
    if env["enableShared"]:
        env.Alias("install", env.InstallAs(target=os.path.join(prefix, "lib", "%s.0.0.19") % sharedLib[0], source=sharedLib))
        env.Alias("install", env.Command(os.path.join(prefix, "lib", "libportaudio.so"), sharedLib, symlink))

testNames = ["patest_sine", "paqa_devs", "paqa_errs", "patest1", "patest_buffer", "patest_callbackstop", "patest_clip", \
        "patest_dither", "patest_hang", "patest_in_overflow", "patest_latency", "patest_leftright", "patest_longsine", \
        "patest_many", "patest_maxsines", "patest_multi_sine", "patest_out_underflow", "patest_pink", "patest_prime", \
        "patest_read_record", "patest_record", "patest_ringmix", "patest_saw", "patest_sine8", "patest_sine", \
        "patest_sine_time", "patest_start_stop", "patest_stop", "patest_sync", \
        "patest_toomanysines", "patest_underflow", "patest_wire", "patest_write_sine", "pa_devs", "pa_fuzz", "pa_minlat"]

# The test directory ("bin") should be in the top-level PA directory, the calling script should ensure that SCons doesn't
# switch current directory by calling SConscriptChdir(False). Specifying an absolute directory for each target means it won't
# be relative to the build directory
tests = [env.Program(target=os.path.join(os.getcwd(), "bin", name), source=[os.path.join("pa_tests", name + ".c"), staticLib]) for name in testNames]

Return("sources", "sharedLib", "staticLib")
