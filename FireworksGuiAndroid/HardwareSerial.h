/*
    Copyright 2016 Chris Arrant
    
    This file is part of FireworksGuiAndroid.

    FireworksGuiAndroid is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireworksGuiAndroid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireworksGuiAndroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <string>


//mimic arduino
class HardwareSerial
{
public:
    HardwareSerial()
    {
        m_fdSerial = -1;
    }
    
    HardwareSerial(const char *pPort)
    {
        m_fdSerial = -1;
        init(pPort);
    }
    
    virtual ~HardwareSerial()
    {
        if (m_fdSerial > 0)
            close(m_fdSerial);
    }
    
public:
    bool init(const char *pPort)
    {
        if (m_fdSerial > 0)
            close(m_fdSerial);

        m_sPort = pPort;

        m_fdSerial = open (m_sPort.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (m_fdSerial < 0)
        {
            printf("Error opening file: %s\n", strerror(errno));
            return false;
        }

        set_interface_attribs (m_fdSerial, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
        set_blocking (m_fdSerial, 0);                // set no blocking
/*
        usleep ((7 + 25) * 100);             // sleep enough to transmit the 7 plus
                                     // receive 25:  approx 100 uS per char transmit
        char buf [100];
        int n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read    
*/
        return true;
    }    

    int set_interface_attribs (int fd, int speed, int parity)
    {
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
            printf("Error tcgetattr: %s\n", strerror(errno));
            return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        if (speed == B110)
            tty.c_cflag |= CSTOPB;      // 2 stop bits for 110        
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
            printf("error %s from tcsetattr", strerror(errno));
            return -1;
        }
        return 0;
    }

    void set_blocking (int fd, int should_block)
    {
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
            printf("Error tggetattr: %s\n", strerror(errno));
            return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
            printf("Error setting term attributes: %s\n", strerror(errno));
    }


public:
    bool isValid() { return m_fdSerial > 0; }

    bool available()
    {
        fd_set readSet;

        FD_ZERO(&readSet);
        FD_SET(m_fdSerial, &readSet);
        int fdMax = m_fdSerial;

        //  select should timeout every so often so we can check IsRunning
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10; // about 100us to transmit one char

        int rc = select(fdMax+1, &readSet, NULL, NULL, &tv);

        if (rc == 0)
            return false;

        //  If select returns a non-recoverable error, exit
        if (rc < 0)
        {
            if (errno == EINTR)
                return false;

            return false;
        }

        if (FD_ISSET(m_fdSerial, &readSet))
            return true;
            
        return false;
    }    
    
    int read()
    {
        int8_t ch = -1;
        ::read (m_fdSerial, &ch, sizeof(int8_t));
        return ch;    
    }
    
    int writeln(const char *fmt, ...)
    {
      char buf[1024 + 2]; // resulting string limited to 128 chars
      va_list args;
      va_start (args, fmt );
      int n = vsnprintf(buf, 1024, fmt, args);
      va_end (args);
    
      buf[n++] = '\r';
      buf[n] = '\n';
      
      int ret = ::write(m_fdSerial, buf, n);
      return ret == n;
    }

    int write(const char *fmt, ...)
    {
      char buf[1024 + 2]; // resulting string limited to 128 chars
      va_list args;
      va_start (args, fmt );
      int n = vsnprintf(buf, 1024, fmt, args);
      va_end (args);

      int ret = ::write(m_fdSerial, buf, n);
      return ret == n;
    }

    int write(void *pData, uint16_t u16Size)
    {
        int ret = ::write(m_fdSerial, pData, u16Size);

        return ret == u16Size;
    }

private:
    std::string m_sPort;
    int m_fdSerial;
};


#endif // #ifndef HardwareSerial_h


