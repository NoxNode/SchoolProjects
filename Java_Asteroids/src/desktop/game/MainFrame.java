package desktop.game;

import java.awt.Color;
import java.awt.Graphics;

import javax.swing.JFrame;

import desktop.input.Input;

public class MainFrame extends JFrame {
	private static final long serialVersionUID = 1L;
	private static Game game = new Game();
	private static Input input = new Input();

	public MainFrame() {
		setName("Framework");
		setSize(Settings.WIDTH, Settings.HEIGHT);
		setVisible(true);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		addKeyListener(input);

		Thread updater = new Thread(new Runnable() {
			@Override
			public void run() {
				while(true) {
					input.refresh(500);
					game.update(input);
					if(!game.isUpdatingPaused())
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

	public static void main(String[] args) {
		MainFrame main = new MainFrame();
	}
}
