package at.metalab.blinkenschild.app;

import android.app.Activity;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.Window;

/**
 * Created by Chris Hager <chris@linuxuser.at> on 13/03/14.
 */
public class TextActivity extends ActionBarActivity {
    private static final String TAG = "BlinkenSchild-Text";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Setup the window
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setContentView(R.layout.device_list);

        // Set result CANCELED in case the user backs out
        setResult(Activity.RESULT_CANCELED);
    }
}
