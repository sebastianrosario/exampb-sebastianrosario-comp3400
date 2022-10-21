#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

struct pcb_head {
	struct pcb *head;
	struct pcb *tail;
};

struct pcb {
	int pid;
	int order;
	char *state;
	struct pcb *next;
};

void enQ(struct pcb_head *pcb_header, struct pcb *new_pcb) {
	// point tail to new pcb
	if(pcb_header->head == NULL) {
		pcb_header->head = new_pcb;
		pcb_header->head->state = "RUNNING";
	}
	if( pcb_header->tail == NULL ) {
		pcb_header->tail = new_pcb;
		pcb_header->head->next = pcb_header->tail;
	}

	pcb_header->tail->next = new_pcb;
	pcb_header->tail = new_pcb; 
}

void enQHead(struct pcb_head *pcb_header, struct pcb *new_pcb) {
	new_pcb->next = pcb_header->head;
	new_pcb->state = "RUNNING";
	pcb_header->head = new_pcb;
	new_pcb->next->state = "WAITING";
}

void deQ(struct pcb_head *pcb_header) {
	// make start->head->next new head
	// free old head

	struct pcb *tmp = pcb_header->head;
	printf("\nPCB %d TERMINATED ADDR:%p\n", tmp->order, tmp);

	// last one in the queue
	if(pcb_header->head == pcb_header->tail) {
		pcb_header->head = NULL;
		pcb_header->tail = NULL;
		free(tmp);
		return;
	}
	pcb_header->head = pcb_header->head->next;
	pcb_header->head->state = "RUNNING";
	free(tmp);
}

void print_all(struct pcb_head *start) {
	if(!start->head){
		printf("-------------------------------------\n");
		printf("HEAD:%p, TAIL %p\n", start->head, start->tail);
		printf("ALL PCBs CLEARED\n");
		return;
	}
	struct pcb* iterator = start->head;
	printf("-------------------------------------\n");
	printf("HEAD:%p, TAIL %p\n", start->head, start->tail);
	while( iterator != NULL ){
		printf("PCB %d, PID: %d, STATE: %s, CURR_ADDR:%p, NEXT_PCB:%p\n", iterator->order, iterator->pid, iterator->state, iterator, iterator->next);
		iterator = iterator->next;
	}

}

void populate_pcbs(struct pcb_head *start) {
	struct pcb* init_pcb_7;
	init_pcb_7 = ( struct pcb* )malloc( sizeof( struct pcb* ) );
	init_pcb_7->pid = rand()%1000;
	init_pcb_7->state = "READY";
	init_pcb_7->order = 7;
	init_pcb_7->next = NULL;

	struct pcb* init_pcb_2;
	init_pcb_2 = ( struct pcb* )malloc( sizeof( struct pcb* ) );
	init_pcb_2->pid = rand()%1000;
	init_pcb_2->state = "READY";
	init_pcb_2->order = 2;
	init_pcb_2->next = NULL;

	enQ(start, init_pcb_7);
	enQ(start, init_pcb_2);
}

void clear_pcb_q(struct pcb_head *start) {

	struct pcb* iterator = start->head;
	while(iterator != NULL) {
		print_all(start);
		deQ(start);
		iterator = start->head;
	}
	iterator = NULL;
}

struct pcb_head * init_pcb() {
	struct pcb_head* start;
	start = (struct pcb_head*)malloc( sizeof(struct pcb_head) );
	return start;
}

int main(int argc, char** argv) {
	struct pcb_head *header = (struct pcb_head*)malloc( sizeof(struct pcb_head) );
	struct pcb *in_front = (struct pcb*)malloc(sizeof(struct pcb));
	in_front->pid = rand()%1000;
	in_front->order = 4; 
	in_front->state = "READY";
	struct pcb *in_back = (struct pcb*)malloc(sizeof(struct pcb));
	in_back->pid = rand()%1000;
	in_back->order = 9; 
	in_back->state = "READY";

	// add random pcbs
	populate_pcbs(header);
	print_all(header);

	// put pcb 4 in front, higher priority?
	enQHead(header, in_front);
	print_all(header);

	// put pcb 7 in the back
	enQ(header, in_back); 
	print_all(header);

	// clear all, printing all while doing it, print after
	clear_pcb_q(header);
	print_all(header);
	return 0;
}