import java.net.*;
import java.io.*;

public class DateServer
{
	public static void main(String[] args) {
		new DateServer().startServer();
	}

	public void startServer() {
		ServeDate serveDate = new ServeDate();

		try {
				ServerSocket sock = new ServerSocket(6013);

				while (true)
				{
					/* blocks until something makes a request */
					Socket client = sock.accept();
					serveDate.build(client);
					Thread thread = new Thread(serveDate);
					thread.start();
					try {
						thread.join();
					} catch (InterruptedException ie) { }
				}
		} catch(IOException ioe) {
			System.err.println(ioe);
		}
	}
}

class ServeDate implements Runnable 
{
	private PrintWriter pout;
	private Socket client;

	public ServeDate() {
	}

	public void build(Socket sock) {
		try {
			this.client = sock;
			this.pout = new PrintWriter(sock.getOutputStream(), true);
		} catch (IOException ioe) {
			System.err.println(ioe);
		}
	}

	public void run() {
		try {
			pout.println(new java.util.Date().toString());
			// tear down
			pout.close();
			client.close();			
		} catch (IOException ioe) { 
			System.err.println(ioe);
		}

	}
}