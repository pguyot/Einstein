package com.newtonforever.einstein.action;

/** Helper class for determining which action the C side wants us to do. @author Frank Gruendel */
public class EinsteinActionHelper {
    
    private final int NO_ACTION_MASK = 0x00;
    private final int REFRESH_SCREEN_MASK = 0x01;
    private final int PLAY_SOUND_MASK = 0x02;
    private final int STOP_SOUND_MASK = 0x04;
    private final int CHANGE_VOLUME_MASK = 0x08;

    enum ActionType{DO_NOTHING, REFRESH_SCREEN, PLAY_SOUND, STOP_SOUND, CHANGE_VOLUME};

    EinsteinActionHelper() {
        super();
    }

    ActionType getActionForMask(final int mask) // TODO FG Rename parameter to action unless the bitmap can contain more than one action
    {
        if (NO_ACTION_MASK == mask)
        {
            return ActionType.DO_NOTHING;
        }
        else if (0 != (mask & REFRESH_SCREEN_MASK))
        {
            return ActionType.REFRESH_SCREEN;
        }
        else if (0 != (mask & PLAY_SOUND_MASK))
        {
            return ActionType.PLAY_SOUND;
        }
        else if (0 != (mask & STOP_SOUND_MASK))
        {
            return ActionType.STOP_SOUND;
        }
        else if (0 != (mask & CHANGE_VOLUME_MASK))
        {
            return ActionType.CHANGE_VOLUME;
        }
        else
        {
            return ActionType.DO_NOTHING; // TODO FG if we can't have more than one action in the int, throw IllegalStateException.
        }
    }

}
