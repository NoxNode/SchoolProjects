package desktop.input;

import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

import util.Time;

public class Input extends KeyAdapter {
	private ArrayList<Integer> keysPressed = new ArrayList<Integer>();
	private ArrayList<Integer> keysPressedUsed = new ArrayList<Integer>();
	private ArrayList<KeyStroke> keysHeld = new ArrayList<KeyStroke>();
	private ArrayList<Integer> keysReleased = new ArrayList<Integer>();
	private ArrayList<Integer> keysReleasedUsed = new ArrayList<Integer>();
	private ArrayList<KeyStroke> keysTapped = new ArrayList<KeyStroke>();

	/**
	 * Use this method at the start or end of the method or thread that updates
	 * the program based on input
	 */
	public synchronized void refresh(long maxDoubleTapUpTimeInMillis) {
		for(int k : keysReleasedUsed) {
			for(int i = 0; i < keysHeld.size(); i++) {
				if(keysHeld.get(i).getKeyCode() == k) {
					keysHeld.remove(i);
					i--;
				}
			}
		}
		keysReleasedUsed.clear();
		keysPressedUsed.clear();
		for(int k : keysReleased) {
			keysReleasedUsed.add(k);
		}
		for(int k : keysPressed) {
			keysPressedUsed.add(k);
		}
		keysReleased.clear();
		keysPressed.clear();
		for(int i = 0; i < keysTapped.size(); i++) {
			if(keysTapped.get(i).getTimeHeld(Time.getTimeInMillis()) >= maxDoubleTapUpTimeInMillis) {
				keysTapped.remove(i);
				i--;
			}
		}
	}

	public synchronized boolean isKeyPressed(int key) {
		for(int k : keysPressedUsed) {
			if(k == key) {
				return true;
			}
		}
		return false;
	}

	public synchronized boolean isKeyHeld(int key) {
		for(KeyStroke k : keysHeld) {
			if(k.getKeyCode() == key) {
				return true;
			}
		}
		return false;
	}

	public synchronized boolean isKeyReleased(int key) {
		for(int k : keysReleasedUsed) {
			if(k == key) {
				return true;
			}
		}
		return false;
	}

	public synchronized boolean isKeyDoubleTapped(int key,
			long maxTimeDownInMillis) {
		for(int i = 0; i < keysTapped.size(); i++) {
			KeyStroke tappedKey = keysTapped.get(i);
			if(tappedKey.getKeyCode() == key) {
				if(tappedKey.getTimeHeld(Time.getTimeInMillis()) < maxTimeDownInMillis) {
					for(int k : keysReleasedUsed) {
						if(k == key) {
							keysTapped.remove(i);
							return true;
						}
					}
				}
				else {
					return false;
				}
			}
		}
		return false;
	}

	public synchronized boolean isKeyTapped(int key, long maxTimeDownInMillis) {
		for(int k : keysReleasedUsed) {
			if(k == key) {
				for(int i = 0; i < keysHeld.size(); i++) {
					KeyStroke heldKey = keysHeld.get(i);
					if(heldKey.getKeyCode() == key) {
						if(heldKey.getTimeHeld(Time.getTimeInMillis()) < maxTimeDownInMillis) {
							keysTapped.add(new KeyStroke(key, Time
									.getTimeInMillis()));
							return true;
						}
						else {
							return false;
						}
					}
				}
			}
		}
		return false;
	}

	@Override
	public void keyPressed(KeyEvent e) {
		int key = e.getKeyCode();

		boolean inKeysHeld = false;
		for(KeyStroke k : keysHeld) {
			if(k.getKeyCode() == key) {
				inKeysHeld = true;
			}
		}
		if(!inKeysHeld) {
			keysPressed.add(key);
			keysHeld.add(new KeyStroke(key, Time.getTimeInMillis()));
		}
	}

	@Override
	public void keyReleased(KeyEvent e) {
		int key = e.getKeyCode();

		keysReleased.add(key);
	}
}
