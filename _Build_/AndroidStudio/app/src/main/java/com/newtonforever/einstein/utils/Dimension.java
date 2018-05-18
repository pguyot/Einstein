/**
 * A Dimension class that encapsulates width and height values. This class is here because Android
 * surprisingly does not know the java.awt.Dimension class.
 */

package com.newtonforever.einstein.utils;

public class Dimension {

    /** The width. 0 by default. */
    public int width = 0;

    /** The height. 0 by default. */
    public int height = 0;

    public Dimension(int width, int height) {
        this.width = width;
        this.height = height;
    }

    public String toString() {
        return ("Dimension (width = " + width + " height = " + height + ")");
    }

}
