import java.net.*;
import java.io.*;
import java.util.Scanner;

public class EchoClient {
	public static void main(String[] args) {
		try {
			/* connect to localhost:6017 */
			Socket server = new Socket("127.0.0.1", 5575);
			Scanner sc = new Scanner(System.in);
			String line = sc.nextLine();

			PrintWriter pout = new PrintWriter(server.getOutputStream(), true);
			pout.println(line);

			server.close();
		}
		catch(IOException ioe) {
			System.err.println(ioe);
		}
	}
}
