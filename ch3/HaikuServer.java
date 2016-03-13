import java.net.*;
import java.io.*;
import java.util.Calendar;

public class HaikuServer {
	private static String myHaiku = "Do the code quickly\nDebug forever when done\nwithout very much thought.";
	public static void main(String[] args) {
		try {
			ServerSocket sock = new ServerSocket(5575);

			/* listen for connections */
			while (true)
			{
				/* blocks until request */
				Socket client = sock.accept();
				PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

				pout.println(myHaiku);

				/* close and resume listening */
				client.close();
			}
		}
		catch(IOException ioe) {
			System.err.println(ioe);
		}
	}
}
