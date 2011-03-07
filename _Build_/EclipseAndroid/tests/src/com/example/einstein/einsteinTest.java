package com.example.einstein;

import android.test.ActivityInstrumentationTestCase;

/**
 * This is a simple framework for a test of an Application.  See
 * {@link android.test.ApplicationTestCase ApplicationTestCase} for more information on
 * how to write and extend Application tests.
 * <p/>
 * To run this test, you can type:
 * adb shell am instrument -w \
 * -e class com.example.einstein.einsteinTest \
 * com.example.einstein.tests/android.test.InstrumentationTestRunner
 */
public class einsteinTest extends ActivityInstrumentationTestCase<einstein> {

    public einsteinTest() {
        super("com.example.einstein", einstein.class);
    }

}
