This chat is a client/server application with server implemented as a COM out-of-process server. That means that after that projects are built you have to REGISTER the COM server and the proxy/stub dll.

In other words the following commands must be issued (in an Administrator console):

ComChat.exe /RegServer
regsvr32 ComChatPS.dll

The following commands revert all registration:

ComChat.exe /UnregServer
regsvr32 /u ComChatPS.dll
