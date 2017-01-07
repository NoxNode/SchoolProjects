package desktop.game;

import java.awt.Graphics;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

import twoD.math.Vector2F;
import desktop.game.entities.Asteroid;
import desktop.game.entities.Bullet;
import desktop.game.entities.Ship;
import desktop.input.Input;

public class Game {
	private ArrayList<Asteroid> asteroids;
	private Ship ship;
	private boolean isPlaying, gameOver, paused, justPaused;
	private final int nShips = 20;
	private final float asteroidSpeed = 2;

	public Game() {
		init();
	}

	public boolean isUpdatingPaused() {
		if(justPaused) {
			justPaused = false;
			return false;
		}
		return isPlaying && (gameOver || paused);
	}

	public void init() {
		justPaused = false;
		isPlaying = true;
		gameOver = false;
		paused = false;
		ship = new Ship(new Vector2F(Settings.WIDTH / 2, Settings.HEIGHT / 2),
				20, 20, 5.0f, 0.02f, 0.97f, 0.1f, 10.0f, 150);
		asteroids = new ArrayList<Asteroid>();
		for(int i = 0; i < nShips; i++) {
			int size = (int) ((Math.random() * 3) + 1) * 15;
			asteroids
					.add(new Asteroid(
							new Vector2F(
									(float) (Math.random() * Settings.WIDTH),
									(float) (Math.random() * Settings.HEIGHT)),
							new Vector2F(
									(float) (Math.random() * asteroidSpeed * 2 - asteroidSpeed),
									(float) (Math.random() * asteroidSpeed * 2 - asteroidSpeed)),
							size, size));
		}
	}

	public void update(Input input) {
		if(input.isKeyHeld(KeyEvent.VK_ESCAPE)) {
			System.exit(0);
		}
		if(input.isKeyPressed(KeyEvent.VK_P)) {
			if(paused) {
				paused = false;
			}
			else {
				paused = true;
				justPaused = true;
			}
		}
		if(isPlaying && !paused && !gameOver) {
			ship.update(input);
			ArrayList<Bullet> bullets = ship.getBullets();
			for(int j = 0; j < asteroids.size(); j++) {
				asteroids.get(j).update();
				// check ship to asteroid collision
				if(asteroids.get(j).hitByShip(ship.getCP(), ship.getRadius())) {
					gameOver = true;
					justPaused = true;
				}
				// check bullet to asteroid collision
				boolean hitBullet = false;
				for(int i = 0; i < bullets.size(); i++) {
					if(asteroids.get(j).hitByBullet(bullets.get(i).getP())) {
						hitBullet = true;
						bullets.remove(i);
						i--;
					}
				}
				if(hitBullet) {
					asteroids.remove(j);
					j--;
				}
			}
			if(asteroids.size() == 0) {
				justPaused = true;
				gameOver = true;
			}
		}
		else if(gameOver) {
			if(input.isKeyPressed(KeyEvent.VK_R)) {
				init();
			}
		}
	}

	public synchronized void draw(Graphics g) {
		if(isPlaying) {
			ship.draw(g);
			for(int i = 0; i < asteroids.size(); i++) {
				asteroids.get(i).draw(g);
			}
		}
		if(gameOver) {
			g.drawString("Game Over! Press r to restart", 100,
					Settings.HEIGHT / 2);
		}
		if(paused) {
			g.drawString("Game paused, Press p to resume", 100,
					Settings.HEIGHT / 2);
		}
	}
}
