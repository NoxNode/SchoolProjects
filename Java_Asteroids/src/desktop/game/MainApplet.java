package desktop.game;

import java.applet.Applet;
import java.awt.Color;
import java.awt.Graphics;

import desktop.input.Input;

public class MainApplet extends Applet {
	private static final long serialVersionUID = 1L;
	private static Game game = new Game();
	private static Input input = new Input();

	public void init() {
		setName("Framework");
		setSize(Settings.WIDTH, Settings.HEIGHT);
		setVisible(true);
		addKeyListener(input);
		setFocusable(true);
		requestFocusInWindow();

		Thread updater = new Thread(new Runnable() {
			@Override
			public void run() {
				while(true) {
					input.refresh(500);
					game.update(input);
					repaint();
					try {
						Thread.sleep(20);
					} catch(InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		});
		updater.start();
	}

	@Override
	public void paint(Graphics g) {
		g.setColor(Color.BLACK);
		g.fillRect(0, 0, Settings.WIDTH - 1, Settings.HEIGHT - 1);
		g.setColor(Color.WHITE);
		game.draw(g);
	}
}
