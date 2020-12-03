# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f D:\ti\ccs1010\workspace_v10\Lab8_Ex3/src/makefile.libs

clean::
	$(MAKE) -f D:\ti\ccs1010\workspace_v10\Lab8_Ex3/src/makefile.libs clean

