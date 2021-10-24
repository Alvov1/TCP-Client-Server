# TCP-Server-Client
TCP Server-Client interaction program. Works for both Windows and Linux. Tested in MS Visual Studio 2019 and Cygwin 3.2.0. Messages arriving at the server are handled with multithreading via std::thread. Data itself is encoded according to ASN1 Tag-Length-Value principle.

To build and run, copy the client files to the client project and the server files to the server project. LengthWrapper files need to be connected to both projects. You will need to specify a port number for both client and server objects. 
