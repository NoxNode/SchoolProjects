package desktop.game.entities;

import java.awt.Graphics;

import twoD.gfx.MovingSprite;
import twoD.math.RectF;
import twoD.math.SpriteMath;
import twoD.math.Vector2F;
import desktop.game.Settings;

public class Bullet extends MovingSprite {
	private boolean outOfBounds;

	public Bullet(Vector2F p, Vector2F v, int width, int height) {
		super(p, v, width, height);

		outOfBounds = false;
	}

	public boolean isOutOfBounds() {
		return outOfBounds;
	}

	public Vector2F getP() {
		return new Vector2F(x, y);
	}

	@Override
	public void update() {
		outOfBounds = SpriteMath.lock(this, new RectF(Settings.WIDTH,
				Settings.HEIGHT));
		x += moveX;
		y += moveY;
	}

	public void draw(Graphics g) {
		g.fillRect((int) x, (int) y, width, height);
	}
}
