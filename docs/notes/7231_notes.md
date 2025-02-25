# Header Fields
### Allow:
>  An origin server MUST generate an Allow field in a 405 (Method Not Allowed) response and MAY do so in any other response.  An empty Allow field value indicates that the resource allows no methods, which might occur in a 405 response if the resource has been temporarily disabled by configuration.  

> Example of use:  
>     Allow: GET, HEAD, PUT

Source:
[Section 7.4.1](https://www.rfc-editor.org/rfc/rfc7231#section-7.4.1)
<br/><br/>

### Content-Type:
> A sender that generates a message containing a payload body SHOULD generate a Content-Type header field in that message unless the intended media type of the enclosed representation is unknown to the sender.  If a Content-Type header field is not present, the recipient MAY either assume a media type of "application/octet-stream" ([RFC2046], Section 4.5.1) or examine the data to determine its type.  

Source:
[Section 3.1.1.5](https://www.rfc-editor.org/rfc/rfc7231.html#section-3.1.1.5)

> text/html has no required parameters, only optional parameter of charset.  

Source:
[IANA Media Type specification](https://www.iana.org/assignments/media-types/text/html)
<br/><br/>

### Date:
> Format: Date = day-name "," SP date1 SP time-of-day SP GMT  
	An example is  
    	Date: Tue, 15 Nov 1994 08:12:31 GMT.  

> An origin server MUST NOT send a Date header field if it does not have a clock capable of providing a reasonable approximation of the current instance in Coordinated Universal Time.  An origin server MAY send a Date header field if the response is in the 1xx (Informational) or 5xx (Server Error) class of status codes.  An origin server MUST send a Date header field in all other cases.

Source:
[Section 7.1.1.2](https://datatracker.ietf.org/doc/html/rfc7231#section-7.1.1.2)
<br/><br/>

### Expect:
> A server that receives an Expect field-value other than 100-continue MAY respond with a 417 (Expectation Failed) status code to indicate that the unexpected expectation cannot be met.

> A 100-continue expectation informs recipients that the client is about to send a (presumably large) message body in this request and wishes to receive a 100 (Continue) interim response if the request-line and header fields are not sufficient to cause an immediate success, redirect, or error response.

> A server MAY omit sending a 100 (Continue) response if it has already received some or all of the message body for the corresponding request, or if the framing indicates that there is no message body.

> A server that sends a 100 (Continue) response MUST ultimately send a final status code, once the message body is received and processed, unless the connection is closed prematurely.  
A server that responds with a final status code before reading the entire message body SHOULD indicate in that response whether it intends to close the connection or continue reading and discarding the request message.

> An origin server MUST, upon receiving an HTTP/1.1 (or later) request-line and a complete header section that contains a 100-continue expectation and indicates a request message body will follow, either send an immediate response with a final status code, if that status can be determined by examining just the request-line and header fields, or send an immediate 100 (Continue) response to encourage the client to send the request's message body.  The origin server MUST NOT wait for the message body before sending the 100 (Continue) response.

Source:
[Section 5.1.1](https://datatracker.ietf.org/doc/html/rfc7231#section-5.1.1)
<br/><br/>

### Server:
> The "Server" header field contains information about the software used by the origin server to handle the request, which is often used by clients to help identify the scope of reported interoperability problems, to work around or tailor requests to avoid particular server limitations, and for analytics regarding server or operating system use.  An origin server MAY generate a Server field in its responses.  

Source:
[Section 7.4.2](https://www.rfc-editor.org/rfc/rfc7231#section-7.4.2)
<br/><br/>


# Responses
### Unacceptable Content-Encoding:
>  An origin server MAY respond with a status code of 415 (Unsupported Media Type) if a representation in the request message has a content coding that is not acceptable.  

Source:
[Section 3.1.2.2](https://datatracker.ietf.org/doc/html/rfc7231#section-3.1.2.2)
<br/><br/>


# Method response codes
### DELETE:
> If a DELETE method is successfully applied, the origin server SHOULD send a 202 (Accepted) status code if the action will likely succeed but has not yet been enacted, a 204 (No Content) status code if the action has been enacted and no further information is to be supplied, or a 200 (OK) status code if the action has been enacted and the response message includes a representation describing the status. 

Source:
[Section 4.3.5](https://datatracker.ietf.org/doc/html/rfc7231#section-4.3.5)
<br/><br/>

### POST:
> POST is used for the following functions (among others):  
	- [...]  
	- Creating a new resource that has yet to be identified by the origin server;  
	- Appending data to a resource's existing representation(s).  

> If one or more resources has been created on the origin server as a result of successfully processing a POST request, the origin server SHOULD send a 201 (Created) response containing a Location header field that provides an identifier for the primary resource created (Section 7.1.2) and a representation that describes the status of the request while referring to the new resource(s).   

> If the result of processing a POST would be equivalent to a representation of an existing resource, an origin server MAY redirect the user agent to that resource by sending a 303 (See Other) response with the existing resource's identifier in the Location field.  This has the benefits of providing the user agent a resource identifier and transferring the representation via a method more amenable to shared caching, though at the cost of an extra request if the user agent does not already have the representation cached.

Source:
[Section 4.3.3](https://datatracker.ietf.org/doc/html/rfc7231#section-4.3.3)
<br/><br/>

### :
>  .  

Source:
[]()
<br/><br/>

### :
>  .  

Source:
[]()
<br/><br/>

### :
>  .  

Source:
[]()
<br/><br/>