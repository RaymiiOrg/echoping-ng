# echoping-ng

echoping-ng is an attempt to modernize echoping:

- Make it compile with modern C compilers (C11)
- Use a modern build system (CMake)
- Drop support for anything that is not Linux or *BSD

Possibly even:

- Add more plugins
- Add modern protocols
- Improve defaults (use HTTP instead of echo?)

This is very much a work in progress. As of 2023-11 I can compile echoping-ng and use HTTP:

    ./echoping-ng -n 5 -R -h / -H raymii.org raymii.org

Output:

    Elapsed time: 0.043191 seconds
    Elapsed time: 0.063264 seconds
    Elapsed time: 0.039433 seconds
    Elapsed time: 0.048628 seconds
    Elapsed time: 0.050021 seconds
    ---
    Minimum time: 0.039433 seconds (6492 bytes per sec.)
    Maximum time: 0.063264 seconds (4047 bytes per sec.)
    Average time: 0.048907 seconds (5234 bytes per sec.)
    Standard deviation: 0.008122
    Median time: 0.048628 seconds (5264 bytes per sec.)

Using the `echo` (port 7) protocol is working as well:

    ./echoping-ng -n 5 127.0.0.1

Output

    Elapsed time: 0.002616 seconds
    Elapsed time: 0.002139 seconds
    Elapsed time: 0.002591 seconds
    Elapsed time: 0.004721 seconds
    Elapsed time: 0.006094 seconds
    ---
    Minimum time: 0.002139 seconds (119682 bytes per sec.)
    Maximum time: 0.006094 seconds (42009 bytes per sec.)
    Average time: 0.003632 seconds (70485 bytes per sec.)
    Standard deviation: 0.001522
    Median time: 0.002616 seconds (97859 bytes per sec.)


All other stuff is not yet working (plugins, using CMake to find packages, SMTP)


## Enabling echo (port 7) on Debian

(via: https://unix.stackexchange.com/a/361712)

For setting up a echo service in Debian, you can install xinetd with:

    sudo apt install xinetd

Than you have to change the `disable` directive to `no` in `/etc/xinetd.d/echo`; 
or if the file does not exist, create it as shown here:

    # default: off
    # description: An xinetd internal service which echo's characters back to
    # clients.
    # This is the tcp version.
    service echo
    {
    disable     = no
    type        = INTERNAL
    id      = echo-stream
    socket_type = stream
    protocol    = tcp
    user        = root
    wait        = no
    }
    
    # This is the udp version.
    service echo
    {
    disable     = yes
    type        = INTERNAL
    id      = echo-dgram
    socket_type = dgram
    protocol    = udp
    user        = root
    wait        = yes
    }

After setting `disable = no`, or creating the file, your restart xinetd with:

    sudo systemctl restart xinetd 

To test the echo TCP service:
    
    $nc localhost echo
    testing...
    testing...
    xxxx
    xxxx
    ^C
    

# Original README

"echoping" is a small program to test (approximatively) performances
of a remote host by sending it TCP "echo" (or other protocol, like
HTTP) packets.

To install it, see the INSTALL file. Or type "./configure; make" if
you're in a hurry :-)

To use it, simply:

% echoping machine.somewhere.org

or use the options before the machine name (see the man page).

See the DETAILS file for various traps when benchmarking networks,
specially with this program

In any case, be polite: don't bother the remote host with many repeated 
requests, especially with large size. Ask for permission if you often
test hosts which aren't yours.

Current features:

- uses the protocols echo, discard, chargen, HTTP (with SSL/TLS if you
  wish), ICP or SMTP,
- uses UDP instead of TCP for the protocols which accept it (like echo), 
- can repeat the test and display various measures about it,
- supports IPv6 as well as IPv4,
- supports IDN (Unicode domain names),
- supports plugins written by you, to test even more protocols (see PLUGINS),

Examples of output:

(Simple test with 1000 bytes echo TCP packets)
% echoping -v -s 1000 mycisco
This is echoping, version 2.0.
Trying to connect to internet address 10.99.64.1 to transmit 256 bytes...
Connected...
Sent (1000 bytes)...
Checked
Elapsed time: 0.059597 seconds
%

(Repeated tests with average and median displayed.)
% echoping -n 10 mymachine
[...]
Minimum time: 0.170719 seconds (1500 bytes per sec.)
Maximum time: 0.211176 seconds (1212 bytes per sec.)
Average time: 0.184577 seconds (1387 bytes per sec.)
Median time: 0.181332 seconds (1412 bytes per sec.)

(Testing a Web server with an HTTP request for its home page.)
%  echoping -h / mywww
Elapsed time: 0.686792 seconds

(The exit status is set if there is any problem, so you can use echoping
to test repeatedly a Web server, to be sure it runs fine.)



-------------
The reference site for echoping is:

http://echoping.sourceforge.net/

The distribution is from:

http://sourceforge.net/project/showfiles.php?group_id=4581

Stephane Bortzmeyer <bortz@users.sourceforge.net>. October 1995 for the
first version. 

--------------------- 
If you want to help and/or motivate echoping and its developer, you
can (but are not forced to do so, echoping is free software):

* Give money through Sourceforge's donation system
  (http://sourceforge.net/project/project_donations.php?group_id=4581)

* Use my Amazon's wish list
  (http://www.amazon.com/gp/registry/23ELBV1YZ93SC)

* Send me a postcard. I love postcards:
       Stephane Bortzmeyer
       127, rue Brancion
       75015 Paris
       France



$Id$



