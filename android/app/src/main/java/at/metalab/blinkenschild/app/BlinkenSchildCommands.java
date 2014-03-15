/**
 * This file is part of the BlinkenSchild Android app.
 *
 * The BlinkenSchild Android app is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The BlinkenSchild Android app is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the BlinkenSchild Android app. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Created by Chris Hager, March 2014
 */
package at.metalab.blinkenschild.app;

/**
 * BlinkenSchildCommands helps building the commands for the
 * BlinkenSchild Arduino.
 */
public class BlinkenSchildCommands {
    public final static String COMMAND_NOOP = "x";

    public final static String COMMAND_GET_ANIMATIONS = "+list:";

    public final static String COMMAND_SET_ANIMATION = "+anim:%s";
    public final static String COMMAND_SET_TEXT = "+text:%s";
    public final static String COMMAND_SET_TEXTCOLOR = "+text-color:%s";
    public final static String COMMAND_SET_OUTLINECOLOR = "+outline-color:%s";
    public final static String COMMAND_SET_BRIGHTNESS_TEXT = "+text-brightness:%d";
    public final static String COMMAND_SET_BRIGHTNESS_ANIM = "+anim-brightness:%d";

    public static String setAnimation(String filename) {
        return String.format(COMMAND_SET_ANIMATION, filename);
    }

    public static String setText(String text) {
        return String.format(COMMAND_SET_TEXT, text);
    }

    public static String setTextColor(String textColor) {
        return String.format(COMMAND_SET_TEXTCOLOR, textColor);
    }

    public static String setOutlineColor(String outlineColor) {
        return String.format(COMMAND_SET_OUTLINECOLOR, outlineColor);
    }

    /**
     * Build command to set text brightness.
     * @param brightness [1..9] (1=100%, 9=10%)
     */
    public static String setTextBrightness(int brightness) {
        return String.format(COMMAND_SET_BRIGHTNESS_TEXT, brightness);
    }

    /**
     * Build command to set animation brightness.
     * @param brightness [1..9] (1=100%, 9=10%)
     */
    public static String setAnimationBrightness(int brightness) {
        return String.format(COMMAND_SET_BRIGHTNESS_ANIM, brightness);
    }
}
