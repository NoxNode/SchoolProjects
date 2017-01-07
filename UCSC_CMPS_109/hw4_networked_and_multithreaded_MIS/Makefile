default:
	echo "run 'make client' to build the client_misvm executable or 'make server' to build the server_misvm executable"

client: Code/Sources/clientmain.cpp Code/Sources/Error.cpp Code/Sources/Instruction.cpp Code/Sources/InstructionFunctions.cpp Code/Sources/MISProgram.cpp Code/Sources/Parameter.cpp Code/Sources/Util.cpp Code/Sources/Variable.cpp Code/Sources/MicroInstructions.cpp Code/Sources/TCPSocket.cpp Code/Sources/TCPServerSocket.cpp Code/Sources/Thread.cpp Code/Sources/Connection.cpp Code/Sources/ThreadManager.cpp Code/Sources/ChildThread.cpp
	g++ -pthread -o client_misvm -std=c++11 Code/Sources/clientmain.cpp Code/Sources/Error.cpp Code/Sources/Instruction.cpp Code/Sources/InstructionFunctions.cpp Code/Sources/MISProgram.cpp Code/Sources/Parameter.cpp Code/Sources/Util.cpp Code/Sources/Variable.cpp Code/Sources/MicroInstructions.cpp Code/Sources/TCPSocket.cpp Code/Sources/TCPServerSocket.cpp Code/Sources/Thread.cpp Code/Sources/Connection.cpp Code/Sources/ThreadManager.cpp Code/Sources/ChildThread.cpp

server: Code/Sources/servermain.cpp Code/Sources/Error.cpp Code/Sources/Instruction.cpp Code/Sources/InstructionFunctions.cpp Code/Sources/MISProgram.cpp Code/Sources/Parameter.cpp Code/Sources/Util.cpp Code/Sources/Variable.cpp Code/Sources/MicroInstructions.cpp Code/Sources/TCPSocket.cpp Code/Sources/TCPServerSocket.cpp Code/Sources/Thread.cpp Code/Sources/Connection.cpp Code/Sources/ThreadManager.cpp Code/Sources/ChildThread.cpp
	g++ -pthread -o server_misvm -std=c++11 Code/Sources/servermain.cpp Code/Sources/Error.cpp Code/Sources/Instruction.cpp Code/Sources/InstructionFunctions.cpp Code/Sources/MISProgram.cpp Code/Sources/Parameter.cpp Code/Sources/Util.cpp Code/Sources/Variable.cpp Code/Sources/MicroInstructions.cpp Code/Sources/TCPSocket.cpp Code/Sources/TCPServerSocket.cpp Code/Sources/Thread.cpp Code/Sources/Connection.cpp Code/Sources/ThreadManager.cpp Code/Sources/ChildThread.cpp

clean:
	rm *misvm; rm *.err; rm *.out

