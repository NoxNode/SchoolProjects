package twoD.gfx;

import twoD.math.Vector2F;

public class MovingSprite extends Sprite {
	protected float moveX, moveY;

	public MovingSprite(float x, float y, float moveX, float moveY, int width,
			int height) {
		super(x, y, width, height);
		this.moveX = moveX;
		this.moveY = moveY;
	}

	public MovingSprite(Vector2F p, Vector2F v, int width, int height) {
		super(p, width, height);
		moveX = v.x;
		moveY = v.y;
	}

	public void update() {
		x += moveX;
		y += moveY;
	}
}