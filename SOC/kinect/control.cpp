#include <XnCppWrapper.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

char buf[256];
int _server = -1;

int connect_to_server(const char *address, int portno);

void init_control(const char *serveraddr, int portno) {
	_server = connect_to_server(serveraddr, portno);

	if(_server == -1) {
		fprintf(stderr, "Error connecting to fakenav\n");
		exit(1);
	}
}

/* Update fakenave with latest controls.
 * deltaX should pan the camera left/right.
 * deltaY should move you closer/farther to the center of the earth.
 * deltaZ should control the speed with which you move forward.
 */
void update_control(XnFloat deltaX, XnFloat deltaY, XnFloat deltaZ) {
	float scale = 1.0f;
	float thresh = 5.0f;
	float horizontal;
	float vertical;
	float acceleration;

	if(deltaX < thresh && deltaX > -thresh){
		horizontal = 0.0f;
	} else if (deltaX < 0.0f) {
		horizontal = (deltaX + thresh) * -scale;
	} else{
		horizontal = (deltaX + thresh) * -scale;
	}

	if(deltaY < thresh && deltaY > -thresh){
		vertical = 0.0f;
	} else if (deltaY < 0.0f) {
		vertical = (deltaY + thresh) * -scale;
	} else{
		vertical = (deltaY + thresh) * -scale;
	}

	if(deltaZ < thresh && deltaZ > -thresh){
		acceleration = 0.0f;
	} else if (deltaZ < 0.0f) {
		acceleration = (deltaZ + thresh) * -scale;
	} else{
		acceleration = (deltaZ + thresh) * -scale;
	}

	acceleration *= 2;

	printf("vertical: %f \n" ,vertical);
	printf("horizontal: %f \n" ,horizontal);
	printf("acceleration: %f \n" ,acceleration);
	//x, y, z, yaw, pitch, roll
	sprintf(buf, "%f, %f, %f, %f, %f, %f\n", 0.0f, acceleration, vertical, horizontal, 0.0f, 0.0f);

	int len = strlen(buf);

	if(_server > 0 && write(_server, buf, len) != len) {
		fprintf(stderr, "Error writing to fakenav\n");
		exit(1);
	}
}

int connect_to_server(const char *address, int portno) {
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		fprintf(stderr, "Error opening socket.\n");
		exit(1);
	}

	server = gethostbyname(address);
	if(server == NULL) {
		fprintf(stderr, "ERROR: No such host.\n");
		exit(1);
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0) {
		fprintf(stderr, "Error connecting.\n");
		exit(1);
	}

	return sockfd;
}
