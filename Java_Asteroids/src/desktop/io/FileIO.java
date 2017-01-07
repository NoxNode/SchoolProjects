package desktop.io;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class FileIO {
	public static void writeToFile(String filePathAndName, String data) {
		try {
			FileWriter fw = new FileWriter(filePathAndName);

			fw.write(data);

			fw.close();
		} catch(IOException e) {
			e.printStackTrace();
		}
	}

	public static String readFromFile(String filePathAndName) {
		String contents = "";
		try {
			FileReader fr = new FileReader(filePathAndName);

			int newByte = 0;
			while((newByte = fr.read()) != -1) {
				contents += (char) newByte;
			}

			fr.close();
		} catch(FileNotFoundException e) {
			e.printStackTrace();
		} catch(IOException e) {
			e.printStackTrace();
		}
		return contents;
	}
}
