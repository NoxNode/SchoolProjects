package util;

public class Time {
	/**
	 * Returns a double containing the current time in milliseconds since epoch
	 */
	public static long getTimeInMillis() {
		return System.currentTimeMillis();
	}

	public static long getTimeInNanos() {
		return System.nanoTime();
	}

	public static long getTimeSinceMillis(long millis) {
		return System.currentTimeMillis() - millis;
	}

	public static long getTimeSinceNanos(long nanos) {
		return System.nanoTime() - nanos;
	}
}
