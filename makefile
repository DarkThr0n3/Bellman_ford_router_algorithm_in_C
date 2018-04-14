all:
	gcc -o router final_router_code.c -lpthread
	gcc -o data_sender data_generator.c 
