/*
 *
 * CSEE 4840 Lab 2 for 2019
 *
 * Name/UNI: Emma Li (eql2002)
 */

#include "fbputchar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "usbkeyboard.h"
#include <pthread.h>

/* Update SERVER_HOST to be the IP address of
 * the chat server you are connecting to
 */
/* arthur.cs.columbia.edu */
#define SERVER_HOST "128.59.19.114"
#define SERVER_PORT 42000

#define BUFFER_SIZE 128
struct usb_keyboard_packet prev_pack;
size_t input_buffer_loc = 0;
size_t input_buffer_end = 0;
size_t last_pack_len = 0;
int curr_row = 0;
int hold = 0;
char input_buf [BUFFER_SIZE];
int modify_buffer(struct usb_keyboard_packet curr_pack) {
	size_t last_char = 0;
	size_t curr_pack_len = 0;
	char target;
	int shift_pressed = (curr_pack.modifiers == 0x02 || curr_pack.modifiers == 0x20) ? 1 : 0;
	int prev_last_char = (last_pack_len == 0) ? 0 : last_pack_len - 1;
	while (curr_pack.keycode[last_char] != 0 && last_char < 6)
		last_char ++;
	if (last_char == 0)
		goto exit_release;
	last_char --;
	curr_pack_len = last_char + 1;
	
	if (curr_pack_len < last_pack_len && curr_pack.keycode[last_char] != prev_pack.keycode[prev_last_char])
		goto exit_release;
	if (curr_pack_len < last_pack_len)
		goto exit;
	if (curr_pack_len == last_pack_len && curr_pack.modifiers != prev_pack.modifiers)
		goto exit_release;
	if (curr_pack.keycode[last_char] == 0x2A)
		goto backspace;
	else if (curr_pack.keycode[last_char] == 0x50)
		goto cursor_left;
	else if (curr_pack.keycode[last_char] == 0x4F)
		goto cursor_right;
	else if (curr_pack.keycode[last_char] == 0x28)
		goto exit_enter;
	/*Then handle the characters input, first check whether the buffer is out of bound. If so, do not modify the buffer*/
	if (input_buffer_loc >= (BUFFER_SIZE - 3)|| input_buffer_loc < 0 || input_buffer_end >= (BUFFER_SIZE - 3)|| input_buffer_end < 0)
		goto exit;
	/*Then we are allowed to enter a character*/
	if (curr_pack.keycode[last_char] >= 0x04 && curr_pack.keycode[last_char] <= 0x1D && shift_pressed)
		/*A-Z*/
		target = 'A' - 0x04 + curr_pack.keycode[last_char];
	else if (curr_pack.keycode[last_char] >= 0x04 && curr_pack.keycode[last_char] <= 0x1D)
		/*a-z*/
		target = 'a' - 0x04 + curr_pack.keycode[last_char];
	else if (curr_pack.keycode[last_char] >= 0x1e && curr_pack.keycode[last_char] <= 0x26 && !shift_pressed)
		/*1-9*/
		target = '1' - 0x1e + curr_pack.keycode[last_char];
	else if (shift_pressed) {
		switch (curr_pack.keycode[last_char]) {
			case 0x1e: target = '!';
				break;
			case 0x1f: target = '@';
				break;
			case 0x20: target = '#';
				break;
			case 0x21: target = '$';
				break;
			case 0x22: target = '%';
				break;
			case 0x23: target = '^';
				break;
			case 0x24: target = '&';
				break;
			case 0x25: target = '*';
				break;
			case 0x26: target = '(';
				break;
			case 0x27: target = ')';
				break;
			case 0x2d: target = '_';
				break;
			case 0x2e: target = '+';
				break;
			case 0x2f: target = '{';
				break;
			case 0x30: target = '}';
				break;
			case 0x31: target = '|';
				break;
			case 0x33: target = ':';
				break;
			case 0x34: target = '"';
				break;
			case 0x36: target = '<';
				break;
			case 0x37: target = '>';
				break;
			case 0x38: target = '?';
				break;
			case 0x35: target = '~';
				break;
			case 0x2c: target = ' ';
				break;
			case 0x28: goto exit_enter;
				break;
			default:
				goto exit;
		}
	}
	else {
		switch (curr_pack.keycode[last_char]) {
			case 0x27: target = '0';
				break;
			case 0x2d: target = '-';
				break;
			case 0x2e: target = '=';
				break;
			case 0x2f: target = '[';
				break;
			case 0x30: target = ']';
				break;
			case 0x31: target = '\\';
				break;
			case 0x33: target = ';';
				break;
			case 0x34: target = '\'';
				break;
			case 0x36: target = ',';
				break;
			case 0x37: target = '.';
				break;
			case 0x38: target = '/';
				break;
			case 0x35: target = '`';
				break;
			case 0x2c: target = ' ';
				break;
			case 0x28: goto exit_enter;
				break;
			default:
					goto exit;
		}
	}
	/*Now do the insertion*/
	if (input_buffer_loc < input_buffer_end)
		for (size_t i = input_buffer_end; i != input_buffer_loc; i--)
			input_buf[i] = input_buf[i - 1];
	input_buf[input_buffer_loc] = target;
	input_buffer_loc ++;
	input_buffer_end ++;
	input_buf[input_buffer_end] = '\0';
	goto exit;
backspace:
	if (input_buffer_loc == 0)
		goto exit;
	if (input_buffer_loc >= BUFFER_SIZE) {
		input_buffer_loc = BUFFER_SIZE - 1;
		input_buffer_end = BUFFER_SIZE - 1;
		input_buf[input_buffer_end] = '\0';
		goto exit;
	}
	for (size_t i = input_buffer_loc; i != input_buffer_end; i ++)
		input_buf[i - 1] = input_buf[i];
	input_buffer_loc --;
	input_buffer_end --;
	input_buf[input_buffer_end] = '\0';
	goto exit;
cursor_left:
	if (input_buffer_loc == 0)
		goto exit;
	input_buffer_loc --;
	goto exit;
cursor_right:
	if (input_buffer_loc < input_buffer_end)
		input_buffer_loc ++;
exit:
	last_pack_len = curr_pack_len;
	prev_pack = curr_pack;
	hold = 1;
	return 0;
exit_release:
	last_pack_len = curr_pack_len;
	prev_pack = curr_pack;
	hold = 0;
	return 0;
exit_enter:
	last_pack_len = curr_pack_len;
	prev_pack = curr_pack;
	hold = 0;
	return 1;
}

void clear_chat_region(void)
{
	for (int row = 0; row < 21; row ++)
		clear_row(row);
}

void print_input_buffer(void)
{
	clear_row(22);
	clear_row(23);
	fbputs(input_buf, 22, 0);
	if (input_buffer_loc < 125)
		fbputcursor(22 + input_buffer_loc / 63, input_buffer_loc % 63);
	else
		fbputredcursor(22 + input_buffer_loc / 63, input_buffer_loc % 63);
	return;
}

/*
 * References:
 *
 * https://web.archive.org/web/20130307100215/http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 *
 * http://www.thegeekstuff.com/2011/12/c-socket-programming/
 * 
 */

int sockfd; /* Socket file descriptor */

struct libusb_device_handle *keyboard;
uint8_t endpoint_address;
pthread_t network_thread;
void *network_thread_f(void *);

int main()
{
  int err;

  struct sockaddr_in serv_addr;

  struct usb_keyboard_packet packet;
  int transferred;
  char keystate[12];

  if ((err = fbopen()) != 0) {
    fprintf(stderr, "Error: Could not open framebuffer: %d\n", err);
    exit(1);
  }
  /* Draw rows of asterisks across the top and bottom of the screen */
  
  
	clear_screen();
	fbputline(21);
  /* Open the keyboard */
  if ( (keyboard = openkeyboard(&endpoint_address)) == NULL ) {
    fprintf(stderr, "Did not find a keyboard\n");
    exit(1);
  }
    
  /* Create a TCP communications socket */
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    fprintf(stderr, "Error: Could not create socket\n");
    exit(1);
  }

  /* Get the server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  if ( inet_pton(AF_INET, SERVER_HOST, &serv_addr.sin_addr) <= 0) {
    fprintf(stderr, "Error: Could not convert host IP \"%s\"\n", SERVER_HOST);
    exit(1);
  }

  /* Connect the socket to the server */
  if ( connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error: connect() failed.  Is the server running?\n");
    exit(1);
  }

  /* Start the network thread */
  pthread_create(&network_thread, NULL, network_thread_f, NULL);

  /* Look for and handle keypresses */
  int timeout = 300;
  for (;;) {
    int ret = libusb_interrupt_transfer(keyboard, endpoint_address,
			      (unsigned char *) &packet, sizeof(packet),
			      &transferred, timeout);
		if (ret && hold) {
			modify_buffer(prev_pack);
			print_input_buffer();
			printf("%s\n", input_buf);
			timeout = 80;
		}
    else if (transferred == sizeof(packet)) {
    	timeout = 300;
      sprintf(keystate, "%02x %02x %02x %02x", packet.modifiers, packet.keycode[0],
	      packet.keycode[1], packet.keycode[2]);
	    int enter_pressed = modify_buffer(packet);
      printf("%s\n", keystate);
      printf("%s\n", input_buf);
      if (enter_pressed) {
      	write(sockfd, input_buf, input_buffer_end);
      	input_buffer_end = 0;
      	input_buffer_loc = 0;
      	memset((void *) input_buf, 0, BUFFER_SIZE * sizeof(char));
      	print_input_buffer();
      	continue;
      }
      print_input_buffer();
      if (packet.keycode[0] == 0x29) { /* ESC pressed? */
				break;
      }
    }
  }

  /* Terminate the network thread */
  pthread_cancel(network_thread);

  /* Wait for the network thread to finish */
  pthread_join(network_thread, NULL);

  return 0;
}

void *network_thread_f(void *ignored)
{
  char recvBuf[BUFFER_SIZE * 2];
  int n;
  /* Receive data */
  while ( (n = read(sockfd, &recvBuf, BUFFER_SIZE * 2 - 1)) > 0 ) {
  	int num_rows = (n + 62) / 63;
  	if ((curr_row + num_rows) > 20) {
  		curr_row = 0;
  		clear_chat_region();
  	}
    recvBuf[n] = '\0';
    printf("%s", recvBuf);
    fbputs(recvBuf, curr_row, 0);
    curr_row += num_rows;
  }
  return NULL;
}
