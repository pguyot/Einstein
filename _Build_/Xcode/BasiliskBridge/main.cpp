//
//  main.cpp
//  BasiliskBridge
//
//  Created by Matthias Melcher on 5/2/18.
//

/*
 This tool creates a link between the EInstein serial port emulation and the
 BasiliskII serial port emulation without blocking either app. Make sure that
 BasiliskBridge runs before BasiliskII or EInstein is launched.

 To the outside, BBridge create two Unix files that act like a pseudo terminal,
 connecting Einstein and BasiliskII. Einstein should emulate its serial port
 through "/tmp/BBridge.Einstein", which it does automatically if the serial
 option is set to BBridge. BasiliskII should set "/tmp/BBridge.BasiliskII"
 for the Modem port emulation.

 Under the hood, BBridge generates two pseudo terminal connactions via pty.
 Those terminals are linked to the files in "/tmp" whic are then accessible
 form those apps. BBridge then manages the other ends of those terminals so
 that neithe rprogram blocks (BasiliskII will block when using standard
 pseudo terminals).
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h>


const char *gBasiliskPipe = "/tmp/BBridge.BasiliskII";
const char *gBasiliskSlaveName = nullptr;
int gBasiliskMaster = -1;
int gBasiliskSlave = -1;

const char *gEinsteinPipe = "/tmp/BBridge.Einstein";
int gEinsteinMaster = -1;
int gEinsteinSlave = -1;


void dumpRead(const char *pre, int fd, int fdOut)
{
	int nAvail = 0;
	int ret = ioctl(fd, FIONREAD, &nAvail);
	nAvail = 1024;
	if (nAvail) {
		printf("    reading %d bytes\n", nAvail);
		uint8_t *buf = (uint8_t*)malloc(nAvail);
		nAvail = ::read(fd, buf, nAvail);
		printf("    %s ", pre);
		int i;
		for (i=0; i<nAvail; i++) {
			printf("%02X ", buf[i]);
		}
		for (i=0; i<nAvail; i++) {
			uint8_t c = buf[i];
			printf("%c", c>' '&&c<127 ? c : '.');
		}
		printf("\n");
		if (fdOut!=-1) {
			::write(fdOut, buf, nAvail);
		}
		free(buf);
	}
}


void dumpPacketRead(const char *pre, int fd, int fdOut)
{
	int nAvail = 0;
	int ret = ioctl(fd, FIONREAD, &nAvail);
	nAvail = 1024;
	if (nAvail) {
		printf("    reading %d bytes\n", nAvail);
		uint8_t *buf = (uint8_t*)malloc(nAvail);
		nAvail = ::read(fd, buf, nAvail);
		printf("    %s ", pre);
		int i;
		for (i=0; i<nAvail; i++) {
			printf("%02X ", buf[i]);
		}
		for (i=0; i<nAvail; i++) {
			uint8_t c = buf[i];
			printf("%c", c>' '&&c<127 ? c : '.');
		}
		printf("\n");
		if (nAvail==1) {
			uint8_t c = buf[0];
			if (c&TIOCPKT_FLUSHREAD) puts("     - TIOCPKT_FLUSHREAD");
			if (c&TIOCPKT_FLUSHWRITE) puts("     - TIOCPKT_FLUSHWRITE");
			if (c&TIOCPKT_STOP) puts("     - TIOCPKT_STOP");
			if (c&TIOCPKT_START) puts("     - TIOCPKT_START");
			if (c&TIOCPKT_DOSTOP) puts("     - TIOCPKT_DOSTOP");
			if (c&TIOCPKT_NOSTOP) puts("     - TIOCPKT_NOSTOP");
			/*
			 Basilisk app starts:
			 TIOCPKT_NOSTOP
			 then
			 TIOCPKT_FLUSHREAD
			 then
			 TIOCPKT_FLUSHREAD
			 TIOCPKT_FLUSHWRITE

			 Basilisk app stops:
			 TIOCPKT_FLUSHREAD
			 TIOCPKT_FLUSHWRITE
			 then locked, close and open pty, then
			 TIOCPKT_NOSTOP

			 */
		} else if (nAvail>1) {
			if (fdOut!=-1) {
				::write(fdOut, buf+1, nAvail-1);
			}
		}
		free(buf);
	}
}


int setupBasiliskPipe()
{
	int ret = -1;

	ret = unlink(gBasiliskPipe);
	//ret = remove(gBasiliskPipe);
	if (ret==-1 && errno!=2) {
		perror("Can't unlink Basilisk Pipe");
		return -1;
	}
	//	}

	gBasiliskMaster = posix_openpt(O_RDWR | O_NOCTTY);
	if (gBasiliskMaster==-1) {
		perror("Can't open Basilisk Master");
		return -1;
	}

	ret = grantpt(gBasiliskMaster);
	if (ret==-1) {
		perror("Can't grant Basilisk Master pseudo terminal");
		return -1;
	}

	ret = unlockpt(gBasiliskMaster);
	if (ret==-1) {
		perror("Can't unlock Basilisk Master pseudo terminal");
		return -1;
	}

	// ptsname_r(int fildes, char *buffer, size_t buflen);
	const char *slaveName = ptsname(gBasiliskMaster);
	if (slaveName==nullptr) {
		perror("Can't get name of Basilisk Slave pseudo terminal");
		return -1;
	}
	gBasiliskSlaveName = strdup(slaveName);

	gBasiliskSlave = open(slaveName, O_RDWR | O_NOCTTY);
	if (gBasiliskSlave==-1) {
		perror("Can't open Basilisk Slave pseudo terminal");
		return -1;
	}

	ret = symlink(slaveName, gBasiliskPipe);
	if (ret==-1) {
		perror("Can't link Basilisk pipe");
		return -1;
	}

	//	tcgetattr(fd, &struct termios)
	struct termios tios;

	ret = tcgetattr(gBasiliskMaster, &tios);
	if (ret==-1) { perror("tcgetattr"); return -1; }
	cfmakeraw(&tios);
	tios.c_cflag |= (HUPCL | CLOCAL);
	ret = tcsetattr(gBasiliskMaster, TCSANOW, &tios);
	if (ret==-1) { perror("tcsetattr"); return -1; }

	ret = tcgetattr(gBasiliskSlave, &tios);
	if (ret==-1) { perror("tcgetattr"); return -1; }
	cfmakeraw(&tios);
	tios.c_cflag |= (HUPCL | CLOCAL);
	ret = tcsetattr(gBasiliskSlave, TCSANOW, &tios);
	if (ret==-1) { perror("tcsetattr"); return -1; }

	int pkt = 1;
	ret = ioctl(gBasiliskMaster, TIOCPKT, &pkt);
	if (ret==-1) { perror("TIOCPKT"); return -1; }

	return 0;
}

int setupEinsteinPipe()
{
	int ret = -1;

	ret = unlink(gEinsteinPipe);
	//ret = remove(gEinsteinPipe);
	if (ret==-1 && errno!=2) {
		perror("Can't unlink Einstein Pipe");
		return -1;
	}
	//	}

	gEinsteinMaster = posix_openpt(O_RDWR | O_NOCTTY);
	if (gEinsteinMaster==-1) {
		perror("Can't open Einstein Master");
		return -1;
	}

	ret = grantpt(gEinsteinMaster);
	if (ret==-1) {
		perror("Can't grant Einstein Master pseudo terminal");
		return -1;
	}

	ret = unlockpt(gEinsteinMaster);
	if (ret==-1) {
		perror("Can't unlock Einstein Master pseudo terminal");
		return -1;
	}

	// ptsname_r(int fildes, char *buffer, size_t buflen);
	const char *slaveName = ptsname(gEinsteinMaster);
	if (slaveName==nullptr) {
		perror("Can't get name of Einstein Slave pseudo terminal");
		return -1;
	}
	slaveName = strdup(slaveName);

	gEinsteinSlave = open(slaveName, O_RDWR | O_NOCTTY);
	if (gEinsteinSlave==-1) {
		perror("Can't open Einstein Slave pseudo terminal");
		return -1;
	}

	ret = symlink(slaveName, gEinsteinPipe);
	if (ret==-1) {
		perror("Can't link Einstein pipe");
		return -1;
	}

	//	tcgetattr(fd, &struct termios)
	struct termios tios;

	ret = tcgetattr(gEinsteinMaster, &tios);
	if (ret==-1) { perror("tcgetattr"); return -1; }
	cfmakeraw(&tios);
	ret = tcsetattr(gEinsteinMaster, TCSANOW, &tios);
	if (ret==-1) { perror("tcsetattr"); return -1; }

	ret = tcgetattr(gEinsteinSlave, &tios);
	if (ret==-1) { perror("tcgetattr"); return -1; }
	cfmakeraw(&tios);
	ret = tcsetattr(gEinsteinSlave, TCSANOW, &tios);
	if (ret==-1) { perror("tcsetattr"); return -1; }

	return 0;
}

//  getpt(3), grantpt(3), ptsname(3), unlockpt(3), pty(7)

int main(int argc, const char * argv[])
{
	int ret;

	ret = setupBasiliskPipe();
	if (ret!=0)
		return ret;

	ret = setupEinsteinPipe();
	if (ret!=0)
		return ret;

//	struct termios tios;
//	tcgetattr(STDIN_FILENO, &tios);
//	tios.c_cflag &= ~ICANON;
//	tios.c_cflag &= ~ECHO;
//	tios.c_cc[VMIN] = 1;
//	tios.c_cc[VTIME] = 0;
//	tcsetattr(STDIN_FILENO, TCSANOW, &tios);
//	setbuf(stdin, nullptr);
//	int flags;
//	fcntl(STDIN_FILENO, F_GETFL, &flags);
//	flags |= O_NONBLOCK;
//	fcntl(STDIN_FILENO, F_SETFL, &flags);
	int flags = 1;
	fcntl(STDIN_FILENO, F_NOCACHE, &flags);


//	signal(SIGQUIT, slirp_exit);
//	signal(SIGHUP, slirp_exit);
//	signal(SIGINT, slirp_exit);
//	signal(SIGTERM, slirp_exit);

	int maxFD = -1;

	printf("Basilisk Master (%d) is %s\n", gBasiliskMaster, ttyname(gBasiliskMaster));
	printf("Basilisk Slave (%d) is %s\n", gBasiliskSlave, ttyname(gBasiliskSlave));

	if (gBasiliskMaster>maxFD) maxFD = gBasiliskMaster;
	if (gBasiliskSlave>maxFD) maxFD = gBasiliskSlave;
	if (gEinsteinMaster>maxFD) maxFD = gEinsteinMaster;
	if (gEinsteinSlave>maxFD) maxFD = gEinsteinSlave;

	fd_set readSet;
	fd_set writeSet;
	fd_set errorSet;
	struct timeval timeout;
	for (;;) {
		// wait for the next event
		FD_ZERO(&readSet);
		FD_SET(gBasiliskMaster, &readSet);
//		FD_SET(gBasiliskSlave, &readSet);
		FD_SET(gEinsteinMaster, &readSet);
//		FD_SET(gEinsteinSlave, &readSet);
		FD_SET(STDIN_FILENO, &readSet);

		FD_ZERO(&writeSet);

		FD_ZERO(&errorSet);
		FD_SET(gBasiliskMaster, &errorSet);
		FD_SET(gBasiliskSlave, &errorSet);
		FD_SET(gEinsteinMaster, &errorSet);
		FD_SET(gEinsteinSlave, &errorSet);

		//		if (needTimer) {
					timeout.tv_sec = 1;
					timeout.tv_usec = 0; // one byte at 38400bps serial port speed
		//		}
		ret = select(maxFD+1, &readSet, &writeSet, &errorSet, 0L /* &timeout */);
		if (ret==-1) {
			perror("\"slect\" failed");
			return 30;
		}
		printf("+++ select() returned %d fd's\n", ret);

		if (ret==0) {
#if 0
			int flags = 0;
			ret = ioctl(gBasiliskMaster, TIOCMGET, &flags);
			if (ret==-1) perror("Can't get state");
			printf("%s ", flags&TIOCM_LE  ? "DSR" : "dsr");
			printf("%s ", flags&TIOCM_DTR ? "DTR" : "dtr");
			printf("%s ", flags&TIOCM_RTS ? "RTS" : "rts");
			printf("%s ", flags&TIOCM_CTS ? "CTS" : "cts");
			printf("%s ", flags&TIOCM_CAR ? "DCD" : "dcd");
			printf("%s\n", flags&TIOCM_DSR ? "DSR" : "dsr");
			int ldisc = 0;
			ret = ioctl(gBasiliskMaster, TIOCGETD, &ldisc);
			if (ret==-1) perror("Can't get ldisc");
			printf("%d ", ldisc);
			ret = ioctl(gBasiliskSlave, TIOCGETD, &ldisc);
			if (ret==-1) perror("Can't get ldisc");
			printf("%d \n", ldisc);
			int fd = open(gBasiliskSlaveName, O_RDWR|O_EXCL|O_NONBLOCK);
			printf("Open pipe returned %d\n", fd);
			if (fd==-1) {
				perror("Pipe");
			} else {
				printf("Pipe is %s\n", ttyname(fd));
				close(fd);
			}
			int fd2 = access(gBasiliskPipe, W_OK|R_OK);
			if (fd2==-1) {
				perror("Pipe");
			}
#endif
			struct stat s;
			ret = stat(gBasiliskPipe, &s);
			if (ret==-1) {
				perror("stat");
			} else {
				printf("%d %d %d %d\n", s.st_dev, s.st_mode, s.st_flags, s.st_nlink);
			}
		}

		if (FD_ISSET(STDIN_FILENO, &readSet)) {
			char buf[16];
			ssize_t n = read(STDIN_FILENO, buf, 15);
			buf[n] = 0;
			printf("--- stdin: '%s'\n", buf);
			switch (buf[0]) {
				case '0': write(gBasiliskSlave, "\0", 1); break;
				case '1':
					close(gBasiliskSlave);
					open(gBasiliskSlaveName, O_RDWR | O_NOCTTY);
					break;
				case '2':
					unlink(gBasiliskPipe);
					symlink(gBasiliskPipe, ttyname(gBasiliskMaster));
					break;
				case '3':
					unlink(gBasiliskPipe);
					close(gBasiliskMaster);
					close(gBasiliskSlave);
					setupBasiliskPipe();
					break;
				case '4':
					ret = ioctl(gBasiliskMaster, TIOCSTART, 0);
					if (ret==-1) perror("ERR1");
					ret = ioctl(gBasiliskSlave, TIOCSTART, 0);
					if (ret==-1) perror("ERR1");
					break;
			}
		}

		if (FD_ISSET(gBasiliskMaster, &readSet)) {
			printf("--- gBasiliskMaster readSet\n");
			dumpPacketRead(">", gBasiliskMaster, gEinsteinMaster);
		}

		if (FD_ISSET(gBasiliskMaster, &errorSet)) {
			printf("--- gBasiliskMaster errorSet\n");
		}

		if (FD_ISSET(gBasiliskSlave, &readSet)) {
			printf("--- gBasiliskSlave readSet\n");
			dumpRead("<", gBasiliskSlave, gEinsteinSlave);
		}

		if (FD_ISSET(gBasiliskSlave, &errorSet)) {
			printf("--- gBasiliskSlave errorSet\n");
		}

		if (FD_ISSET(gEinsteinMaster, &readSet)) {
			printf("--- gEinsteinMaster readSet\n");
			dumpRead(">", gEinsteinMaster, gBasiliskMaster);
		}

		if (FD_ISSET(gEinsteinMaster, &errorSet)) {
			printf("--- gEinsteinMaster errorSet\n");
		}

		if (FD_ISSET(gEinsteinSlave, &readSet)) {
			printf("--- gEinsteinSlave readSet\n");
			dumpRead("<", gEinsteinSlave, gBasiliskSlave);
		}

		if (FD_ISSET(gEinsteinSlave, &errorSet)) {
			printf("--- gEinsteinSlave errorSet\n");
		}


	}

#if 0
	int
	remove(const char *path);
	// insert code here...

	int fd = -1;

	getpt() opens a pseudoterminal master and returns its file descriptor. It is equivalent to
	open(/dev/ptmx, O_RDWR | O_NOCTTY);
	int posix_openpt(int flags);
	// /dev/ptmx , ptm masster, pts slave in /dev/pts/*
	char *ptsname(int fd);
	int ptsname_r(int fd, char *buf, size_t buflen);
	int ret = grantpt(fd);
	int unlockpt(int fd);

	int
	ttyname_r(int fd, char *buf, size_t len);
	/*
	 #include <signal.h>

	 void (*signal(int sig, void (*func)(int)))(int);

	 or in the equivalent but easier to read typedef'd version:

	 typedef void (*sig_t) (int);

	 sig_t
	 signal(int sig, sig_t func);

	 */


	int
	link(const char *path1, const char *path2);
	int
	symlink(const char *path1, const char *path2);
	int
	unlink(const char *path);


	int
	atexit(void (*function)(void));


BasiliskII:

	static int cfmakeraw(struct termios *termios_p)
	{
		termios_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
		termios_p->c_oflag &= ~OPOST;
		termios_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
		termios_p->c_cflag &= ~(CSIZE|PARENB);
		termios_p->c_cflag |= CS8;
		return 0;
	}

	fd = ::open(device_name, O_RDWR);
	if (protocol == serial || protocol == pty) {
		if (tcgetattr(fd, &mode) < 0)
			goto open_error;
		cfmakeraw(&mode);
		mode.c_cflag |= HUPCL;
		mode.c_cc[VMIN] = 1;
		mode.c_cc[VTIME] = 0;
		tcsetattr(fd, TCSAFLUSH, &mode);
	}

	mode.c_cflag &= ~CRTSCTS;

    ?? tcsendbreak(fd, 0);
	unsigned int status = TIOCM_DTR;
	ioctl(fd, TIOCMBIS, &status);

	unsigned int status = TIOCM_RTS;
	ioctl(fd, TIOCMBIS, &status);



	switch (code) {
		case 1:			// KillIO
			io_killed = true;
			if (protocol == serial)
				tcflush(fd, TCIOFLUSH);
			while (read_pending || write_pending) <<--- This is where we hang!
				usleep(10000);
			io_killed = false;
			return noErr;


#endif

	return 0;
}
