package desktop.input;

public class KeyStroke {
	private int keyCode;
	private long pressedTime;

	public KeyStroke(int keyCode, long pressedTime) {
		this.keyCode = keyCode;
		this.pressedTime = pressedTime;
	}

	public int getKeyCode() {
		return keyCode;
	}

	public long getTimeHeld(long currentTime) {
		return currentTime - pressedTime;
	}
}
