import java.net.*;
import java.io.*;

public class HaikuClient {
	public static void main(String[] args) {
		try {
			/* connect to localhost:6017 */
			Socket sock = new Socket("127.0.0.1", 5575);

			InputStream in = sock.getInputStream();
			BufferedReader brin = new BufferedReader(new InputStreamReader(in));

			/* buffer */
			String line;
			while ((line = brin.readLine()) != null)
			{
				System.out.println(line);
			}
			sock.close();
		}
		catch(IOException ioe) {
			System.err.println(ioe);
		}
	}
}
