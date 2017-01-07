package twoD.gfx;

import twoD.math.Vector2F;

public class TurningSprite extends Sprite {
	protected float angle, speed, acceleration, maxSpeed, friction,
			turningSpeed;

	public TurningSprite(Vector2F p, int width, int height, float acceleration,
			float maxSpeed, float friction, float turningSpeed) {
		super(p, width, height);

		this.angle = 0;
		this.speed = 0;
		this.acceleration = acceleration;
		this.maxSpeed = maxSpeed;
		this.friction = friction;
		this.turningSpeed = turningSpeed;
	}

	public TurningSprite(float x, float y, int width, int height,
			float acceleration, float maxSpeed, float friction,
			float turningSpeed) {
		super(x, y, width, height);

		this.angle = 0;
		this.speed = 0;
		this.acceleration = acceleration;
		this.maxSpeed = maxSpeed;
		this.friction = friction;
		this.turningSpeed = turningSpeed;
	}
}