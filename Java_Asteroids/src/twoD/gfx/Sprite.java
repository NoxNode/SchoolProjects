package twoD.gfx;

import twoD.math.Vector2F;

public class Sprite {
	public float x, y;
	public int width, height;

	public Sprite(float x, float y, int width, int height) {
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
	}

	public Sprite(Vector2F p, int width, int height) {
		this.x = p.x;
		this.y = p.y;
		this.width = width;
		this.height = height;
	}
}