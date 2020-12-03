# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f D:\ti\ccs1010\workspace_v10\Lab7_Ex2.4/src/makefile.libs

clean::
	$(MAKE) -f D:\ti\ccs1010\workspace_v10\Lab7_Ex2.4/src/makefile.libs clean

