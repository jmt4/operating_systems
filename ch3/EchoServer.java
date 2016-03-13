import java.net.*;
import java.io.*;

public class EchoServer {
	public static void main(String[] args) {
		try {
			ServerSocket sock = new ServerSocket(5575);

			/* listen for connections */
			while (true)
			{
				/* blocks until request */
				Socket client = sock.accept();

				InputStream cin = client.getInputStream();
				InputStreamReader in = new InputStreamReader(cin);

				char[] buff = new char[4096];
				int bytes = 0, offset = 0;
				while (true)
				{
					bytes = in.read(buff, offset, 64);
					if (bytes == -1) {
						System.out.println(new String(buff, 0, offset));
						break;
					}
					else if (offset == 4096) {
						/* buffer overflow essentially */
						break;
					}
					else {
						offset += bytes;
					}
				}
				/* close and resume listening */
				client.close();
			}
		}
		catch(IOException ioe) {
			System.err.println(ioe);
		}
	}
}
