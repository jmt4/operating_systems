import java.net.*;
import java.io.*;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.*;

public class EchoServer {
	public static void main(String[] args) {
		ThreadPool threadPool = new ThreadPool(5, 5);
		try {
			ServerSocket sock = new ServerSocket(5575);

			/* listen for connections */
			while (true)
			{
				/* blocks until request */
				Socket client = sock.accept();
				threadPool.execute((Runnable) new Echo(client));
			}
		}
		catch(Exception ioe) {
			System.err.println(ioe);
		}
	}
}


class ThreadPool {

	private BlockingQueue<Runnable> queue = null;
	private List<WorkerThread> threads = new ArrayList<WorkerThread>();
	private boolean isStopped = false;

	public ThreadPool(int maxThreads, int maxQueueSize) {
		queue = new ArrayBlockingQueue<Runnable>(maxQueueSize);

		for (int i = 0; i < maxThreads; i++) {
			threads.add(new WorkerThread(queue));
		}

		for (WorkerThread thread : threads) {
			thread.start();
		}
	}

	public synchronized void execute(Runnable task) throws Exception {
		if (this.isStopped) throw
			new IllegalStateException("ThreadPool is stopped");

		this.queue.put(task);
	}

	public synchronized void stop() {
		this.isStopped = true;
		for (WorkerThread thread : threads) {
			thread.doStop();
		}
	}
}

class WorkerThread extends Thread {

	private BlockingQueue<Runnable> queue = null;
	private boolean isStopped = false;
	private InputStream is;
	private InputStreamReader isr;
	private char []buff;
	private int offset;

	public WorkerThread(BlockingQueue<Runnable> queue) {
		this.queue = queue;
		buff = new char[4096];
	}

	public void run() {
		while (!isStopped()) {
			try {
				Echo runnable = (Echo) queue.take();
				runnable.run();
			}
			catch (Exception e) {

			}
		}
	}

	public synchronized void doStop() {
		this.isStopped = true;
		this.interrupt();
	}

	public synchronized boolean isStopped() {
		return this.isStopped;
	}

	public void readMessage(Socket socket) {
		int bytes = 0;
		this.offset = 0;
		try {
			this.is = socket.getInputStream();
			this.isr = new InputStreamReader(is);

			while (true) {
				bytes = isr.read(buff, this.offset, 64);
				if (bytes == -1) {
					this.is.close();
					this.isr.close();
					break;
				} else if (this.offset == 4096) {
					/* buffer overflow essentially */
					this.is.close();
					this.isr.close();
					break;
				} else {
					this.offset += bytes;
				}
			}
		}
		catch (IOException ioe) {
			System.err.println(ioe);
		}
	}

	public void writeMessage() {
		System.out.println(new String(buff, 0, offset));
	}

	public static void threadSleep() {
		try {
			System.out.println(
				"Thread: " + Thread.currentThread().getId() + " is sleeping\n");
			Thread.sleep(5000);
			System.out.println(
				"Thread: " + Thread.currentThread().getId() + " is awake\n");
		}
		catch (InterruptedException ie) {
			System.err.println(ie);
		}
	}
}

class Echo implements Runnable {
	
	private Socket clientSocket;

	public Echo(Socket socket) {
		clientSocket = socket;
	}

	public void run() {
		
		// might be able to say WorkerThread.currentThread().readmessage(...)
		((WorkerThread)Thread.currentThread()).readMessage(clientSocket);
		((WorkerThread)Thread.currentThread()).writeMessage();
		try {
			clientSocket.close();
		}
		catch (IOException ioe) {
			System.err.println(ioe);
		}
		WorkerThread.threadSleep();
	}
}