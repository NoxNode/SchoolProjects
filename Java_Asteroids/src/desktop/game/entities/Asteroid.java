package desktop.game.entities;

import java.awt.Graphics;

import twoD.gfx.MovingSprite;
import twoD.math.RectF;
import twoD.math.SpriteMath;
import twoD.math.Vector2F;
import desktop.game.Settings;

public class Asteroid extends MovingSprite {
	public Asteroid(Vector2F p, Vector2F v, int width, int height) {
		super(p, v, width, height);

	}

	@Override
	public void update() {
		SpriteMath.wrap(this, new RectF(Settings.WIDTH, Settings.HEIGHT));
		x += moveX;
		y += moveY;
	}

	public void draw(Graphics g) {
		g.drawRect((int) x, (int) y, width, height);
	}

	public boolean hitByBullet(Vector2F bulletP) {
		if(bulletP.x + 2 > x && bulletP.x < x + width && bulletP.y + 2 > y
				&& bulletP.y < y + height) {
			return true;
		}
		return false;
	}

	public boolean hitByShip(Vector2F shipCP, int radius) {
		if(shipCP.x > x && shipCP.x < x + width && shipCP.y > y
				&& shipCP.y < y + height) {
			return true;
		}
		for(int i = 0; i < width; i++) {
			if(Vector2F.dist(new Vector2F(x + i, y), shipCP) < radius) {
				return true;
			}
		}
		for(int i = 0; i < height; i++) {
			if(Vector2F.dist(new Vector2F(x + width, y + i), shipCP) < radius) {
				return true;
			}
		}
		for(int i = 0; i < width; i++) {
			if(Vector2F.dist(new Vector2F(x + i, y + height), shipCP) < radius) {
				return true;
			}
		}
		for(int i = 0; i < height; i++) {
			if(Vector2F.dist(new Vector2F(x, y + i), shipCP) < radius) {
				return true;
			}
		}
		return false;
	}
}
