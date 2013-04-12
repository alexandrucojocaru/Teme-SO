CC = cl
CFLAGS = /nologo /W3 /EHsc
OUT = bin
TESTS = bin\test_mpirun.exe bin\test_simple.exe bin\test_double_init.exe \
	bin\test_double_finalize.exe bin\test_before_after.exe \
	bin\test_send_recv.exe bin\test_send_recv_array.exe bin\test_ring.exe \
	bin\test_matrix_multiply.exe
DLL = ..\sompi.dll ..\sompi.lib

tests: $(DLL) $(OUT) $(TESTS)

$(OUT):
	md $(OUT)

bin\test_mpirun.exe: test_mpirun.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_simple.exe: test_simple.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_double_init.exe: test_double_init.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_double_finalize.exe: test_double_finalize.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_before_after.exe: test_before_after.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_send_recv.exe: test_send_recv.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_send_recv_array.exe: test_send_recv_array.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_ring.exe: test_ring.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

bin\test_matrix_multiply.exe: test_matrix_multiply.c ..\sompi.lib
	$(CC) $(CFLAGS) /Fe$@ $**

clean:
	del /Q *.obj
	del /Q /S $(OUT)
	rd $(OUT)
