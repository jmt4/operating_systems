import java.net.*;
import java.io.*;
import java.util.Calendar;

public class QuoteServer {
	private static String[] quotes = 
	{
		"Be yourself; everyone else is already taken. -- Oscar Wilde",
		"So many books, so little time. -- Frank Zappa",
		"Without music, life would be a mistake. -- Friedrich Nietzsche",
		"The fool doth think he is wise, but the wise man knows himself to be a fool. -- Shakespeare",
		"I have not failed. I've just found 10,000 ways that won't work. -- Thomas Edison",
		"Great spirits have always encountered violent opposition from mediocre minds -- Albert Einstein",
		"You can never get a cup of tea large enough or a book long enough to suit me. -- C.S. Lewis"		
	};
	public static void main(String[] args) {
		try {
			ServerSocket sock = new ServerSocket(6017);
			Calendar cal;

			/* listen for connections */
			while (true)
			{
				/* blocks until request */
				Socket client = sock.accept();
				PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

				cal = Calendar.getInstance();
				int day_of_week = cal.get(Calendar.DAY_OF_WEEK);
				pout.println(QuoteServer.quotes[day_of_week - 1]);

				/* close and resume listening */
				client.close();
			}
		}
		catch(IOException ioe) {
			System.err.println(ioe);
		}
	}
}
