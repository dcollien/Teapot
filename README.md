For use on a Raspberry Pi or similar device, concealed inside a teapot.
 
This is an HTTP Teapot Server. In order for this device to not to be confused with an HTCPCP device, the HTTP Teapot Server will respond with the HTTP error code 418: I'm a teapot. For more information on the HTCPCP (based on HTTP), see RFC 2324 (1998). Chapter 2.3.2. outlines the use of this particular HTTP error code for the disambiguation of teapots from HTCPCP entities.

Example use case (may also be used with shorter and stouter varieties):
<img src="serving_suggestion.jpg">

N.B. This project requires libevent to build (on Debian/Rasbian one can normally use <code>sudo apt-get install libevent-dev</code> to install this dependency)

