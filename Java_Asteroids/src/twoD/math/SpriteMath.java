package twoD.math;

import twoD.gfx.Sprite;

public class SpriteMath {
	public static boolean wrap(Sprite sprite, RectF boundingBox) {
		boolean wrapped = false;
		if(sprite.x < boundingBox.x) {
			sprite.x = boundingBox.width;
			wrapped = true;
		}
		if(sprite.x > boundingBox.width) {
			sprite.x = boundingBox.x;
			wrapped = true;
		}
		if(sprite.y < boundingBox.y) {
			sprite.y = boundingBox.height;
			wrapped = true;
		}
		if(sprite.y > boundingBox.height) {
			sprite.y = boundingBox.y;
			wrapped = true;
		}
		return wrapped;
	}

	public static boolean lock(Sprite sprite, RectF boundingBox) {
		boolean locked = false;
		if(sprite.x < boundingBox.x) {
			sprite.x = boundingBox.x;
			locked = true;
		}
		if(sprite.x + sprite.width > boundingBox.width) {
			sprite.x = boundingBox.width - sprite.width;
			locked = true;
		}
		if(sprite.y < boundingBox.y) {
			sprite.y = boundingBox.y;
			locked = true;
		}
		if(sprite.y + sprite.height > boundingBox.height) {
			sprite.y = boundingBox.height - sprite.height;
			locked = true;
		}
		return locked;
	}
}
