package twoD.math;

public class RectF {
	public float x, y, width, height;

	public RectF(float width, float height) {
		this.x = 0;
		this.y = 0;
		this.width = width;
		this.height = height;
	}

	public RectF(float x, float y, float width, float height) {
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
	}

	public RectF(Vector2F topLeft, float width, float height) {
		this.x = topLeft.x;
		this.y = topLeft.y;
		this.width = width;
		this.height = height;
	}

	public RectF(RectF rect) {
		this.x = rect.x;
		this.y = rect.y;
		this.width = rect.width;
		this.height = rect.height;
	}

	public boolean isInRect(float x, float y) {
		if(x >= this.x && x < this.x + width && y >= this.y
				&& y < this.y + height) {
			return true;
		}
		return false;
	}

	public boolean isInRect(Vector2F v) {
		if(v.x >= x && v.x < x + width && v.y >= y && v.y < y + height) {
			return true;
		}
		return false;
	}

	public static boolean isInRect(float x, float y, RectF rect) {
		if(x >= rect.x && x < rect.x + rect.width && y >= rect.y
				&& y < rect.y + rect.height) {
			return true;
		}
		return false;
	}

	public static boolean isInRect(Vector2F v, RectF rect) {
		if(v.x >= rect.x && v.x < rect.x + rect.width && v.y >= rect.y
				&& v.y < rect.y + rect.height) {
			return true;
		}
		return false;
	}

	public boolean collidingWithRect(RectF rect) {
		if(this.x + this.width > rect.x && this.x < rect.x + rect.width
				&& this.y + this.height > rect.y
				&& this.y < rect.y + rect.height) {
			return true;
		}
		return false;
	}

	public boolean collidingWithCircle(CircleF circle) {
		if(circle.cx >= this.x && circle.cx < this.x + this.width
				&& circle.cy > this.y && circle.cy < this.y + this.height) {
			return true;
		}
		for(int i = 0; i < this.width; i++) {
			if(Vector2F.dist(new Vector2F(this.x + i, this.y), new Vector2F(
					circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
			if(Vector2F.dist(new Vector2F(this.x + i, this.y + this.height),
					new Vector2F(circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
		}
		for(int i = 0; i < this.height; i++) {
			if(Vector2F.dist(new Vector2F(this.x + this.width, this.y + i),
					new Vector2F(circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
			if(Vector2F.dist(new Vector2F(this.x, this.y + i), new Vector2F(
					circle.cx, circle.cy)) < circle.radius) {
				return true;
			}
		}
		return false;
	}

	public static boolean collidingWithCircle(RectF rect, CircleF circle) {
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
