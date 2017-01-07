package desktop.game.entities;

import java.awt.Graphics;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

import twoD.gfx.TurningSprite;
import twoD.math.Vector2F;
import util.Time;
import desktop.game.Settings;
import desktop.input.Input;

public class Ship extends TurningSprite {
	private Vector2F momentum;
	private ArrayList<Bullet> bullets;
	private float bulletSpeed;
	private int bulletCD;
	private long lastBulletShotTime;

	public Ship(Vector2F p, int width, int height, float maxSpeed,
			float acceleration, float friction, float turningSpeed,
			float bulletSpeed, int bulletCD) {
		super(p, width, height, acceleration, maxSpeed, friction, turningSpeed);

		this.lastBulletShotTime = (int) Time.getTimeInMillis();
		this.bulletSpeed = bulletSpeed;
		this.bulletCD = bulletCD;
		bullets = new ArrayList<Bullet>();
		momentum = new Vector2F(0, 0);
	}

	public Vector2F getCP() {
		return new Vector2F(x + width / 2, y + height / 2);
	}

	public int getRadius() {
		return width / 2;
	}

	public ArrayList<Bullet> getBullets() {
		return bullets;
	}

	public void update(Input input) {
		// bullets
		if(input.isKeyHeld(KeyEvent.VK_SPACE)
				&& Time.getTimeInMillis() - lastBulletShotTime > bulletCD) {
			lastBulletShotTime = Time.getTimeInMillis();
			float cx = x + width / 2;
			float cy = y + height / 2;
			float xMoveBullet = (float) (Math.cos(angle) * width);
			float yMoveBullet = (float) (Math.sin(angle) * height);
			float xMoveBulletSpeed = (float) (Math.cos(angle) * bulletSpeed);
			float yMoveBulletSpeed = (float) (Math.sin(angle) * bulletSpeed);
			bullets.add(new Bullet(new Vector2F(cx + xMoveBullet, cy
					+ yMoveBullet), new Vector2F(xMoveBulletSpeed,
					yMoveBulletSpeed), 2, 2));
		}
		for(int i = 0; i < bullets.size(); i++) {
			Bullet b = bullets.get(i);
			if(b.isOutOfBounds()) {
				bullets.remove(i);
				i--;
			}
			else {
				b.update();
			}
		}

		// ship movement
		if(x < 0) {
			x = Settings.WIDTH;
		}
		if(x > Settings.WIDTH) {
			x = 0;
		}
		if(y < 0) {
			y = Settings.HEIGHT;
		}
		if(y > Settings.HEIGHT) {
			y = 0;
		}
		float xMove = (float) (Math.cos(angle) * speed);
		float yMove = (float) (Math.sin(angle) * speed);
		if(input.isKeyHeld(KeyEvent.VK_W) || input.isKeyHeld(KeyEvent.VK_UP)) {
			speed += acceleration;
			momentum.x += xMove;
			momentum.y += yMove;
		}
		if(input.isKeyHeld(KeyEvent.VK_A) || input.isKeyHeld(KeyEvent.VK_LEFT)) {
			angle -= turningSpeed;
		}
		if(input.isKeyHeld(KeyEvent.VK_D) || input.isKeyHeld(KeyEvent.VK_RIGHT)) {
			angle += turningSpeed;
		}
		if(speed > maxSpeed) {
			speed = maxSpeed;
		}
		x += momentum.x;
		y += momentum.y;
		momentum.x *= friction;
		momentum.y *= friction;
		speed *= friction;
	}

	public void draw(Graphics g) {
		// draw bullets
		for(int i = 0; i < bullets.size(); i++) {
			bullets.get(i).draw(g);
		}
		g.drawOval((int) x, (int) y, width, height);

		// draw ship
		float xMove = (float) (Math.cos(angle) * width / 2);
		float yMove = (float) (Math.sin(angle) * height / 2);
		float xMoveBotLeft = (float) (Math.cos(angle + 3 * Math.PI / 4) * width / 2);
		float yMoveBotLeft = (float) (Math.sin(angle + 3 * Math.PI / 4)
				* height / 2);
		float xMoveBotRight = (float) (Math.cos(angle - 3 * Math.PI / 4)
				* width / 2);
		float yMoveBotRight = (float) (Math.sin(angle - 3 * Math.PI / 4)
				* height / 2);
		float cx = x + width / 2;
		float cy = y + height / 2;
		g.drawLine((int) (cx + xMove), (int) (cy + yMove),
				(int) (cx + xMoveBotLeft), (int) (cy + yMoveBotLeft));
		g.drawLine((int) (cx + xMove), (int) (cy + yMove),
				(int) (cx + xMoveBotRight), (int) (cy + yMoveBotRight));
	}
}
