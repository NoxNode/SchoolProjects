package twoD.math;

public class CircleF {
	public float cx, cy, radius;

	/**
	 * center = (0, 0)
	 */
	public CircleF(float radius) {
		this.cx = 0;
		this.cy = 0;
		this.radius = radius;
	}

	/**
	 * radius = 1
	 */
	public CircleF(float x, float y) {
		this.cx = x;
		this.cy = y;
		this.radius = 1;
	}

	public CircleF(float x, float y, float radius) {
		this.cx = x;
		this.cy = y;
		this.radius = radius;
	}

	public CircleF(Vector2F center, float radius) {
		this.cx = center.x;
		this.cy = center.y;
		this.radius = radius;
	}

	public CircleF(CircleF circle) {
		this.cx = circle.cx;
		this.cy = circle.cy;
		this.radius = circle.radius;
	}

	public boolean isInCircle(float x, float y) {
		if(Vector2F.dist(new Vector2F(x, y), new Vector2F(this.cx, this.cy)) < radius) {
			return true;
		}
		return false;
	}

	public boolean isInCircle(Vector2F v) {
		if(Vector2F
				.dist(new Vector2F(v.x, v.y), new Vector2F(this.cx, this.cy)) < radius) {
			return true;
		}
		return false;
	}

	public boolean collidingWithCircle(CircleF circle) {
		if(Vector2F.dist(new Vector2F(circle.cx, circle.cy), new Vector2F(
				this.cx, this.cy)) < radius + circle.radius) {
			return true;
		}
		return false;
	}

	public boolean collidingWithRect(RectF rect) {
		if(this.cx > rect.x && this.cx < rect.x + rect.width
				&& this.cy > rect.y && this.cy < rect.y + rect.height) {
			return true;
		}
		for(int i = 0; i < rect.width; i++) {
			if(Vector2F.dist(new Vector2F(rect.x + i, rect.y), new Vector2F(
					this.cx, this.cy)) < radius) {
				return true;
			}
			if(Vector2F.dist(new Vector2F(rect.x + i, rect.y + rect.height),
					new Vector2F(this.cx, this.cy)) < radius) {
				return true;
			}
		}
		for(int i = 0; i < rect.height; i++) {
			if(Vector2F.dist(new Vector2F(rect.x + rect.width, rect.y + i),
					new Vector2F(this.cx, this.cy)) < radius) {
				return true;
			}
			if(Vector2F.dist(new Vector2F(rect.x, rect.y + i), new Vector2F(
					this.cx, this.cy)) < radius) {
				return true;
			}
		}
		return false;
	}

	public static boolean collidingWithRect(RectF rect, CircleF circle) {
		if(circle.cx > rect.x && circle.cx < rect.x + rect.width
				&& circle.cy > rect.y && circle.cy < rect.y + rect.height) {
			return true;
		}
		for(int i = 0; i < rect.width; i++) {
			if(Vector2F.dist(new Vector2F(rect.x + i, rect.y), new Vector2F(
					circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
			if(Vector2F.dist(new Vector2F(rect.x + i, rect.y + rect.height),
					new Vector2F(circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
		}
		for(int i = 0; i < rect.height; i++) {
			if(Vector2F.dist(new Vector2F(rect.x + rect.width, rect.y + i),
					new Vector2F(circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
			if(Vector2F.dist(new Vector2F(rect.x, rect.y + i), new Vector2F(
					circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
		}
		return false;
	}
}
