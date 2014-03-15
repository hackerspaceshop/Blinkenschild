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

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Collections;
import java.util.Comparator;


public class MainActivity extends ActionBarActivity {
    // Debugging
    private static final String TAG = "BlinkenSchild";
    private static final boolean D = true; // Debug

    // Message types sent from the BluetoothChatService Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    // Key names received from the BluetoothChatService Handler
    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";

    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE_SECURE = 1;
    private static final int REQUEST_CONNECT_DEVICE_INSECURE = 2;
    private static final int REQUEST_ENABLE_BT = 3;

    // Layout Views
    private ListView mConversationView;
//    private EditText mOutEditText;
    private Button mSendButton;

    private boolean isFirstStart = true;

    // Name of the connected device
    private String mConnectedDeviceName = null;

    // Array adapter for the conversation thread
    private ArrayAdapter<String> mConversationArrayAdapter;
    private Comparator<String> mConversationArrayAdapterComparator;

    // String buffer for outgoing messages
    private StringBuffer mOutStringBuffer;
    // Local Bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter = null;
    // Member object for the chat services
    private BluetoothChatService mChatService = null;
    private Button mSendDebugButton;
    private Dialog dialogText = null;

    private int currentAnimation = -1;
    private String currentText = null;
    private int currentTextColor = 0;
    private int currentOutlineColor = 0;
    private int currentTextBrightness = 9;
    private int currentAnimBrightness = 9;

    private void log(String s) {
        if (D) Log.v(TAG, s);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        log("+++ ON CREATE +++");

        // Set up the window layout
        setContentView(R.layout.main);

        // Get local Bluetooth adapter
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // If the adapter is null, then Bluetooth is not supported
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
    }

    @Override
    public void onStart() {
        super.onStart();
        log("++ ON START ++");

        // If BT is not on, request that it be enabled.
        // setupChat() will then be called during onActivityResult
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
            // Otherwise, setup the chat session
        } else {
            if (mChatService == null) setupChat();
        }
    }

    @Override
    public synchronized void onResume() {
        super.onResume();
        log("+ ON RESUME +");

        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mChatService != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mChatService.getState() == BluetoothChatService.STATE_NONE) {
                // Start the Bluetooth chat services
                mChatService.start();
            }
        }

        if (isFirstStart) {
            startDeviceSelectActivity();
            isFirstStart = false;
        }
    }

    private void setupChat() {
        log("setupChat()");

        // Initialize the array adapter for the conversation thread
        mConversationArrayAdapter = new ArrayAdapter<String>(this, R.layout.message);
        mConversationArrayAdapterComparator = new Comparator<String>() {
            @Override
            public int compare(String lhs, String rhs) {
                return lhs.compareTo(rhs);
            }
        };

        mConversationView = (ListView) findViewById(R.id.in);
        mConversationView.setAdapter(mConversationArrayAdapter);
        mConversationView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String fn = mConversationArrayAdapter.getItem(position);
                sendBTMessage(BlinkenSchildCommands.setAnimation(fn));
            }
        });

        // Initialize the send button with a listener that for click events
        mSendButton = (Button) findViewById(R.id.button_text);
        mSendButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                showTextDialog();
            }
        });

        ((Button) findViewById(R.id.button_color)).setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                showColorDialog();
            }
        });

        // Initialize the BluetoothChatService to perform bluetooth connections
        mChatService = new BluetoothChatService(this, mHandler);

        // Initialize the buffer for outgoing messages
        mOutStringBuffer = new StringBuffer("");
    }

    @Override
    public synchronized void onPause() {
        super.onPause();
        if(D) Log.e(TAG, "- ON PAUSE -");
    }

    @Override
    public void onStop() {
        super.onStop();
        if(D) Log.e(TAG, "-- ON STOP --");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // Stop the Bluetooth chat services
        if (mChatService != null) mChatService.stop();
        if(D) Log.e(TAG, "--- ON DESTROY ---");
    }

    private void ensureDiscoverable() {
        if(D) Log.d(TAG, "ensure discoverable");
        if (mBluetoothAdapter.getScanMode() !=
                BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
            startActivity(discoverableIntent);
        }
    }

    /**
     * Sends a message.
     * @param message  A string of text to send.
     */
    private void sendBTMessage(String message) {
        // Check that we're actually connected before trying anything
        if (message == null) {
            Log.e(TAG, "sendBTMessage(null) doesnt work");
            return;
        }

        message = message.trim();
        Log.v(TAG, "sendBTMessage: '" + message + "'");

        if (mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // Check that there's actually something to send
        if (message.length() > 0) {
            // Get the message bytes and tell the BluetoothChatService to write
//            Log.v(TAG, "message to send: '" + message + "'");
            byte[] send = message.getBytes();
            mChatService.write(send);

            // Reset out string buffer to zero and clear the edit text field
            mOutStringBuffer.setLength(0);
        }
    }

    // The action listener for the EditText widget, to listen for the return key
    private TextView.OnEditorActionListener mWriteListener =
            new TextView.OnEditorActionListener() {
                public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
                    // If the action is a key-up event on the return key, send the message
                    if (actionId == EditorInfo.IME_NULL && event.getAction() == KeyEvent.ACTION_UP) {
                        String message = view.getText().toString();
                        sendBTMessage(message);
                    }
                    if(D) Log.i(TAG, "END onEditorAction");
                    return true;
                }
            };

    private final void setStatus(int resId) {
        final ActionBar actionBar = getActionBar();
        actionBar.setSubtitle(resId);
    }

    private final void setStatus(CharSequence subTitle) {
        final ActionBar actionBar = getActionBar();
        actionBar.setSubtitle(subTitle);
    }

    // The Handler that gets information back from the BluetoothChatService
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_STATE_CHANGE:
                    if(D) Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                    switch (msg.arg1) {
                        case BluetoothChatService.STATE_CONNECTED:
                            setStatus(getString(R.string.title_connected_to, mConnectedDeviceName));
                            mConversationArrayAdapter.clear();
                            ((Button) findViewById(R.id.button_text)).setEnabled(true);
                            ((Button) findViewById(R.id.button_color)).setEnabled(true);
                            sendBTMessage(BlinkenSchildCommands.COMMAND_NOOP);
                            sendBTMessage(BlinkenSchildCommands.COMMAND_GET_ANIMATIONS);
                            break;
                        case BluetoothChatService.STATE_CONNECTING:
                            setStatus(R.string.title_connecting);
                            break;
                        case BluetoothChatService.STATE_LISTEN:
                        case BluetoothChatService.STATE_NONE:
                            setStatus(R.string.title_not_connected);
                            break;
                    }
                    break;

                case MESSAGE_WRITE:
//                    byte[] writeBuf = (byte[]) msg.obj;
//                    String writeMessage = new String(writeBuf);
//                    Toast.makeText(getApplicationContext(), "Sent: " + writeMessage, Toast.LENGTH_LONG).show();
                    break;

                case MESSAGE_READ:
                    String readMessage = (String) msg.obj;
                    Log.v(TAG, "received: " + readMessage);
                    if (readMessage.startsWith("+list:")) {
                        readMessage = readMessage.substring(6);
                        mConversationArrayAdapter.add(readMessage);
                        mConversationArrayAdapter.sort(mConversationArrayAdapterComparator);
                    }
                    break;

                case MESSAGE_DEVICE_NAME:
                    // save the connected device's name
                    mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
                    Toast.makeText(getApplicationContext(), "Connected to "
                            + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                    break;

                case MESSAGE_TOAST:
                    Toast.makeText(getApplicationContext(), msg.getData().getString(TOAST),
                            Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(D) Log.d(TAG, "onActivityResult " + resultCode);
        switch (requestCode) {
            case REQUEST_CONNECT_DEVICE_SECURE:
                // When DeviceListActivity returns with a device to connect
                if (resultCode == Activity.RESULT_OK) {
                    connectDevice(data, true);
                }
                break;
            case REQUEST_CONNECT_DEVICE_INSECURE:
                // When DeviceListActivity returns with a device to connect
                if (resultCode == Activity.RESULT_OK) {
                    connectDevice(data, false);
                }
                break;
            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    // Bluetooth is now enabled, so set up a chat session
                    setupChat();
                } else {
                    // User did not enable Bluetooth or an error occurred
                    Log.d(TAG, "BT not enabled");
                    Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                    finish();
                }
        }
    }

    private void connectDevice(Intent data, boolean secure) {
        // Get the device MAC address
        String address = data.getExtras()
                .getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
        // Get the BluetoothDevice object
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        // Attempt to connect to the device
        mChatService.connect(device, secure);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main, menu);
        return true;
    }

    private void startDeviceSelectActivity() {
        Intent serverIntent = new Intent(this, DeviceListActivity.class);
        startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE_SECURE);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.secure_connect_scan:
                // Launch the DeviceListActivity to see devices and do scan
                startDeviceSelectActivity();
                return true;
//            case R.id.insecure_connect_scan:
//                // Launch the DeviceListActivity to see devices and do scan
//                serverIntent = new Intent(this, DeviceListActivity.class);
//                startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE_INSECURE);
//                return true;
//            case R.id.discoverable:
//                // Ensure this device is discoverable by others
//                ensureDiscoverable();
//                return true;
        }
        return false;
    }

    private void showTextDialog() {
        LayoutInflater inflater = getLayoutInflater();
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        View v = inflater.inflate(R.layout.dialog_text, null);
        final EditText txt = (EditText) v.findViewById(R.id.editText);
        if (currentText != null) txt.setText(currentText);
        txt.setOnKeyListener(new View.OnKeyListener() {
            @Override
            public boolean onKey(View v, int keyCode, KeyEvent event) {
                Log.v(TAG, "key event: " + keyCode);
                if ((event.getAction() == KeyEvent.ACTION_DOWN) &&
                        (keyCode == KeyEvent.KEYCODE_ENTER)) {
                    // Perform action on key press
                    String text = txt.getText().toString();
                    if (text != null && !text.equals(currentText)) {
                        sendBTMessage(BlinkenSchildCommands.setText(txt.getText().toString()));
                        currentText = text;
                    }
                    if (dialogText != null) {
                        dialogText.dismiss();
                    }
                    return true;
                }
                return false;
            }
        });
        builder.setView(v)
                .setTitle("Custom Text")
                .setPositiveButton("Set Text", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id) {
                        // Set Text Command
                        String text = txt.getText().toString();
                        if (text != null && !text.equals(currentText)) {
                            sendBTMessage(BlinkenSchildCommands.setText(text));
                            currentText = text;
                        }
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });

        dialogText = builder.create();
        dialogText.show();
    }

    private void showColorDialog() {
        LayoutInflater inflater = getLayoutInflater();
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        View v = inflater.inflate(R.layout.dialog_color, null);
        final Button btnColorPicker = (Button) v.findViewById(R.id.buttonPickTextColor);
        btnColorPicker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new ColorPickerDialog(MainActivity.this, new ColorPickerDialog.OnColorChangedListener() {
                    @Override
                    public void colorChanged(int color) {
                        Log.v(TAG, "color changed to " + color);
                        int blue = color & 255;
                        int green = color >> 8 & 255;
                        int red = color >> 16 & 255;
                        sendBTMessage(BlinkenSchildCommands.setTextColor(red + "," + green + "," + blue));
                        currentTextColor = color;
                    }
                }, currentTextColor).show();
            }
        });

        final Button btnOutlineColorPicker = (Button) v.findViewById(R.id.buttonPickOutlineColor);
        btnOutlineColorPicker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new ColorPickerDialog(MainActivity.this, new ColorPickerDialog.OnColorChangedListener() {
                    @Override
                    public void colorChanged(int color) {
                        Log.v(TAG, "color changed to " + color);
                        int blue = color & 255;
                        int green = color >> 8 & 255;
                        int red = color >> 16 & 255;
                        sendBTMessage(BlinkenSchildCommands.setOutlineColor(red + "," + green + "," + blue));
                        currentOutlineColor = color;
                    }
                }, currentOutlineColor).show();
            }
        });

        SeekBar sbText = (SeekBar) v.findViewById(R.id.seekBarText);
        sbText.setProgress(currentTextBrightness);
        sbText.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                currentTextBrightness = seekBar.getProgress();
//                Toast.makeText(MainActivity.this, "Brightness: " + (10 - currentTextBrightness), Toast.LENGTH_SHORT).show();
                sendBTMessage(BlinkenSchildCommands.setTextBrightness(10 - currentTextBrightness));
            }
        });

        SeekBar sbAnim = (SeekBar) v.findViewById(R.id.seekBarAnim);
        sbAnim.setProgress(currentAnimBrightness);
        sbAnim.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            }

            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                currentAnimBrightness = seekBar.getProgress();
//                Toast.makeText(MainActivity.this, "Anim Progress: " + (10 - currentAnimBrightness), Toast.LENGTH_SHORT).show();
                sendBTMessage(BlinkenSchildCommands.setTextBrightness(10 - currentAnimBrightness));
            }
        });

        builder.setView(v)
                .setTitle("Colors & Effects")
                .setPositiveButton("Done", null);
        builder.create().show();

    }
}
