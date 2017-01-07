package twoD.math;

public class Vector2F {
	public float x;
	public float y;

	/**
	 * inits x and y as 0
	 */
	public Vector2F() {
		this.x = 0;
		this.y = 0;
	}

	public Vector2F(Vector2F v) {
		this.x = v.x;
		this.y = v.y;
	}

	public Vector2F(float x, float y) {
		this.x = x;
		this.y = y;
	}

	public Vector2F add(Vector2F v) {
		return new Vector2F(this.x + v.x, this.y + v.y);
	}

	public Vector2F add(float scalar) {
		return new Vector2F(this.x + scalar, this.y + scalar);
	}

	public Vector2F sub(Vector2F v) {
		return new Vector2F(this.x - v.x, this.y - v.y);
	}

	public Vector2F sub(float scalar) {
		return new Vector2F(this.x - scalar, this.y - scalar);
	}

	public Vector2F mul(Vector2F v) {
		return new Vector2F(this.x * v.x, this.y * v.y);
	}

	public Vector2F mul(float scalar) {
		return new Vector2F(this.x * scalar, this.y * scalar);
	}

	public Vector2F div(Vector2F v) {
		return new Vector2F(this.x / v.x, this.y / v.y);
	}

	public Vector2F div(float scalar) {
		return new Vector2F(this.x / scalar, this.y / scalar);
	}

	public float length() {
		return (float) Math.sqrt((this.x - this.x) * (this.x - this.x)
				+ (this.y - this.y) * (this.y - this.y));
	}

	public float dot(Vector2F v) {
		return this.x * v.x + this.y * v.y;
	}

	public Vector2F normalized() {
		float length = this.length();
		return new Vector2F(this.x / length, this.y / length);
	}

	public float cross(Vector2F v) {
		return this.x * v.y - this.y * v.x;
	}

	public Vector2F rotate(float angle) {
		double rad = Math.toRadians(angle);
		double cos = Math.cos(rad);
		double sin = Math.sin(rad);

		return new Vector2F((float) (this.x * cos - this.y * sin),
				(float) (this.x * sin + this.y * cos));
	}

	/**
	 * @return the angle of the vector in radians within the range [-pi..pi]
	 */
	public float angleInRadians() {
		return (float) Math.atan2(this.y, this.x);
	}

	/**
	 * @return the angle of the vector in degrees
	 */
	public float angleInDegrees() {
		return (float) (this.angleInRadians() * 180 / Math.PI);
	}

	public float distFrom(Vector2F v) {
		return (float) Math.sqrt((this.x - v.x) * (this.x - v.x)
				+ (this.y - v.y) * (this.y - v.y));
	}

	public static float dist(Vector2F v1, Vector2F v2) {
		return (float) Math.sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y)
				* (v1.y - v2.y));
	}
}
