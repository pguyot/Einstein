
AndroidStudioNative
===================

This version of Einstein for Android is independent of the AndroidStudio project.

The original ANdroidStudio project is a great and complete/complex port of Einstein
onto an adavanced Android platform. The Native port of Einstein uses the 
NativeActivity interface to Android which is targeting pure C/C++ app, avoiding 
the Java overhead, but also missing out on Java features.

I have started AndroidStudioNative to make Einstein available to low-end 
Android devices. Native code gives me the opportunity to minimize CPU time for
screen rendering and event handling, at the cost of having no additional user
interface, preferences or, at least in Feb 2020, no sound.

The current target machine is the MobiScribe 6.8" with a 1357x1080 (1440x1080)
e-ink display. It runs at Android OS API Level 19. We can detect that by queying
android.os.Build.DEVICE which returns "ntx\_6sl".

 - Matthias, Feb 7 2020



