obj-m:=process_list.o

compile:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	gcc userproc.c -o userproc

install: compile
	sudo insmod process_list.ko 

uninstall:
	sudo rmmod process_list 

show_log:
	dmesg | grep Process_List

run: clean install
	sudo ./userproc

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	find . -type f -executable -delete
