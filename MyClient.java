import java.io.*;
import java.net.*;
import java.util.Vector;
import java.util.Scanner;


public class MyClient {

	public static void main(String[] args) throws IOException{
		int user_ok=0,pass_ok=0,pasiveport=0;
		String clientDirectoryPath = "./client-directory/";

		Vector<String> serverResponses = new Vector<String>();
		BufferedReader inp = new BufferedReader (new InputStreamReader(System.in));
		String serverAddress = inp.readLine();
		int controlPortNum = Integer.parseInt(inp.readLine());
		Socket controlSocket = new Socket(serverAddress, controlPortNum);
		Socket dataSocket = new Socket();
		// control socket I/O
		BufferedReader controlSocketIn = new BufferedReader(new InputStreamReader(controlSocket.getInputStream()));

		PrintWriter controlSocketOut = new PrintWriter(controlSocket.getOutputStream(), true);
		String controlResponse = controlSocketIn.readLine();
		serverResponses.add(controlResponse);	// log
		System.out.println(controlResponse);
		String method=" ";
			controlResponse = controlSocketIn.readLine();
			serverResponses.add(controlResponse);	// log
			System.out.println(controlResponse);
		while(true)
		{
		
			System.out.print("ftp<");
			method = inp.readLine();
			
			if(method.contains("USER"))
				{	
					//String[] inpToken = method.split(" ");
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
					user_ok=1;
				}
			 if(method.contains("PASS"))
				{	
					//String[] inpToken = method.split(" ");
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
					pass_ok=1;
				}
			 if(method.contains("SYST"))
				{	
					//String[] inpToken = method.split(" ");
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
				}


			 if(method.contains("PASV")&&(user_ok==1)&&(pass_ok==1))
				{	
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
					String[] responseToken = controlResponse.split(" ");
					
					//Socket dataSocket = new Socket(serverAddress, 1220);

					dataSocket.connect(new InetSocketAddress(serverAddress, 1220));

					BufferedReader dataSocketIn = new BufferedReader(new InputStreamReader(dataSocket.getInputStream()));
					
					System.out.println("Connected to data port 1220");
					pasiveport=1;
					controlResponse = dataSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
					System.out.println(dataSocket);	
				}
			 if(method.contains("PWD"))
				{	
					//String[] inpToken = method.split(" ");
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
					//controlSocketOut.println(method);
					BufferedReader datapSocketIn = new BufferedReader(new InputStreamReader(dataSocket.getInputStream()));
					controlResponse = datapSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
				}
			 if(method.contains("LIST"))
				{	
					//String[] inpToken = method.split(" ");
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
					byte[] bytes = new byte[16 * 1024];
			        InputStream in = dataSocket.getInputStream();
			        //OutputStream out = new FileOutputStream(file);
					long timeOut = System.currentTimeMillis();
			        int count;
			        while ((count = in.read(bytes)) > 0) {
			            System.out.write(bytes, 0, count);
			            if (bytes[count] == '\0')// || count == 0 || (System.currentTimeMillis()-timeOut) > 5)
		        		break;

			        }
				}
			if(method.contains("CWD"))
				{	
					//String[] inpToken = method.split(" ");
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
				}

			if(method.contains("MKD"))
				{	
					//String[] inpToken = method.split(" ");
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);
				}

			 if (method.contains("RETR")&&(user_ok==1)&&(pass_ok==1)){
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);

					//Socket dataSocket = new Socket(serverAddress, 1220);
					//System.out.println(dataSocket);	
					// BufferedReader SocketIn = new BufferedReader(new InputStreamReader(dataSocket.getInputStream()));
					//this.socketIn = new BufferedReader(new InputStreamReader(this.dataSocket.getInputStream()));
					String[] responseToken = method.split(" ");
					String fileName = responseToken[1];
					System.out.println("Filename:"+fileName);
					        File file = new File(fileName);
        // Get the size of the file
					        //if (!file.exists()) 
					        	file.createNewFile();
			        long length = file.length();
			        byte[] bytes = new byte[16 * 1024];
			        InputStream in = dataSocket.getInputStream();
			        OutputStream out = new FileOutputStream(file);
					long timeOut = System.currentTimeMillis();
			        int count;
			        while ((count = in.read(bytes)) > 0) {
			            out.write(bytes, 0, count);
			            if (bytes[count] == '\0')// || count == 0 || (System.currentTimeMillis()-timeOut) > 5)
		        		break;

			        }

			      // out.close();
			      //  in.close();
				}

		if (method.contains("STOR")&&(user_ok==1)&&(pass_ok==1)){
					controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					serverResponses.add(controlResponse);	// log
					System.out.println(controlResponse);

					//Socket dataSocket = new Socket(serverAddress, 1220);
					//System.out.println(dataSocket);	
					// BufferedReader SocketIn = new BufferedReader(new InputStreamReader(dataSocket.getInputStream()));
					//this.socketIn = new BufferedReader(new InputStreamReader(this.dataSocket.getInputStream()));
					String[] responseToken = method.split(" ");
					String fileName = responseToken[1];
					System.out.println("Filename:"+fileName);
					      //  File file = new File(fileName);
        // Get the size of the file
					        //if (!file.exists()) 
					        	//file.createNewFile();
			        File myFile = new File(fileName);

					//Socket socket = server_socket.accept();
					int count;
					byte[] buffer = new byte[16 * 1024];

					OutputStream out = dataSocket.getOutputStream();
					BufferedInputStream in = new BufferedInputStream(new FileInputStream(myFile));
					while ((count = in.read(buffer)) > 0) {
					     out.write(buffer, 0, count);
					     out.flush();
						}

			    }

			 if(method.contains("QUIT"))
				{	controlSocketOut.println(method);
					controlResponse = controlSocketIn.readLine();
					System.out.println(controlResponse);
					controlResponse = controlSocketIn.readLine();
					System.out.println(controlResponse);

					dataSocket.close();
					controlSocketIn.close();
					controlSocketOut.close();
					controlSocket.close();
					
					break;
				}
		}
	}
}

