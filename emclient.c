/* Name:  Malcolm M.
 * Date:  3/23/2022
 * Class: Computer Networking
 * Descr: connects to smtp2go, and creates an email client
 * Only works on Linux based OS
 * need to make an account with smtp2go.com
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAXSIZE 2000

int main() {
    
    // declare variables
    int sockfd = 0, portno_int;
    char portno[6];
    char message[MAXSIZE];
    char user[65];
    char passw[65];
    char serv[30];
    char email[50]; //sender email
    char email2[50]; //recipient email
    char sender[20]; //sender's name
    char recpt[20]; //recipient's name
    char subj[50]; //subject
    char body[1024]; //body of message
    char sendtoserv[1000];
    struct sockaddr_in servaddr;
    char *ip_addr;
    struct hostent *host;

    //command list for commands sent automatically
    char *ehlo = "ehlo\n";
    char *authlogin = "auth login\n";
    char *data = "data\n";

    //allocate space for a message
    bzero(&servaddr, sizeof(servaddr));
    memset(message, '\0', sizeof(message));

    //create socket for use and error check
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("socket error");
        exit(EXIT_FAILURE);
    }

    //get the server from the user
    printf("SMTP Server: ");
    fgets(serv, sizeof(serv), stdin);
    serv[strlen(serv)-1] = 0;
    printf("Port: ");
    fgets(portno, sizeof(portno), stdin);
    portno_int = atoi(portno);

    //assign IP, port
    servaddr.sin_family = AF_INET;
    host = gethostbyname(serv); //get ip address from the given string in serv
    servaddr.sin_port = htons(portno_int); //converts short integer host order to network order
    ip_addr = inet_ntoa(*(struct in_addr*)host->h_addr); //converts network order to dotted ipv4 notation
    inet_pton(AF_INET, ip_addr, &(servaddr.sin_addr)); //converts ip_addr to AF_INET family and copies to servaddr.sin_addr 

    //connect to the port and ip address
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("socket connect error\n");
        exit(EXIT_FAILURE);
    }
    
    read(sockfd, message, sizeof(message)); //read and print the return message from the server
    printf("Response: %s\n", message);
    
    //execute EHLO command
    write(sockfd, ehlo, strlen(ehlo));
    sleep(1);
    bzero(message, sizeof(message)); //zero the message out in preparation to receive other message
    read(sockfd, message, sizeof(message));
    printf("Response: %s\n", message);

    //execute AUTH LOGIN command
    write(sockfd, authlogin, strlen(authlogin));
    sleep(1);
    bzero(message, sizeof(message));    
    read(sockfd, message, sizeof(message)); //read the return message
    printf("Response: %s\n", message);

    //grab username from user, base64 encode
    printf("Username: ");
    fgets(user, sizeof(user), stdin);
    write(sockfd, user, strlen(user)); //write username to server

    sleep(1);
    bzero(message, sizeof(message));
    read(sockfd, message, sizeof(message)); //read the return message
    printf("\nResponse: %s\n", message);

    //grab password from the user, base64 encode
    printf("Password: ");
    fgets(passw, sizeof(passw), stdin);
    write(sockfd, passw, strlen(passw));

    sleep(1);
    read(sockfd, message, sizeof(message)); //read the return message
    printf("\nResponse: %s\n", message);
    memset(message, '\0', sizeof(message));

    //ask for email address of sender and get the response
    while(1){                               //while loop to keep asking for the email address
        printf("Sender Email Address: ");
        fgets(email, sizeof(email), stdin);
        if(strcmp(email, "quit\n") == 0){   //if quit signal is given, exit the while loop
            write(sockfd, email, strlen(email)); //because the server accepts the quit signal, simply write 
            bzero(message, sizeof(message));
            read(sockfd, message, sizeof(message));
            printf("\nResponse: %s\n", message);
            close(sockfd);                      //safely close the socket
            break;
        }
        email[strlen(email)-1] = 0; //set last char to NULL
        snprintf(sendtoserv, sizeof(sendtoserv), "MAIL FROM: %s\n", email);
        write(sockfd, sendtoserv, strlen(sendtoserv));
        sleep(1);
        bzero(message, sizeof(message));
        read(sockfd, message, sizeof(message));
        printf("\nResponse: %s\n", message);
        memset(message, '\0', sizeof(message));

        //ask for email address of recippient and get the response
        bzero(sendtoserv, sizeof(sendtoserv));  //clear out the sendtoserv buffer
        printf("Recipient's Email Address: ");
        fgets(email2, sizeof(email2), stdin);
        email2[strlen(email2)-1] = 0;           //set last char to NULL
        snprintf(sendtoserv, sizeof(sendtoserv), "RCPT TO: %s\n", email2);
        write(sockfd, sendtoserv, strlen(sendtoserv));
        sleep(1);
        bzero(message, sizeof(message));
        read(sockfd, message, sizeof(message));
        printf("\nResponse: %s\n", message);

        //send data command
        write(sockfd, data, strlen(data));
        sleep(1);
        bzero(message, sizeof(message));
        read(sockfd, message, sizeof(message));
        printf("Response: %s\n", message);

        //ask for sender's name and create a mailfrom header
        bzero(sendtoserv, sizeof(sendtoserv));
        printf("Sender: ");
        fgets(sender, sizeof(sender), stdin);
        sender[strlen(sender)-1] = 0; //sets the last character to NULL
        snprintf(sendtoserv, sizeof(sendtoserv), "FROM: \"%s\" <%s>\n", sender, email);
        write(sockfd, sendtoserv, strlen(sendtoserv));
        sleep(1);

        //ask for recipient's name and create a sendto header
        bzero(sendtoserv, sizeof(sendtoserv));
        printf("\nRecipient: ");
        fgets(recpt, sizeof(recpt), stdin);
        recpt[strlen(recpt)-1] = 0; //sets the last character to NULL
        snprintf(sendtoserv, sizeof(sendtoserv), "TO: \"%s\" <%s>\n", recpt, email2);
        write(sockfd, sendtoserv, strlen(sendtoserv));
        sleep(1);

        //ask for the subject of the email and create a subject header
        bzero(sendtoserv, sizeof(sendtoserv));
        printf("\nSubject: ");
        fgets(subj, sizeof(subj), stdin);
        subj[strlen(subj)-1] = 0; //sets the last character to NULL
        snprintf(sendtoserv, sizeof(sendtoserv), "SUBJECT: \"%s\"\n", subj);
        write(sockfd, sendtoserv, strlen(sendtoserv));
        sleep(1);

        //ask for the message and then append the necessary characters to send to server
        bzero(sendtoserv, sizeof(sendtoserv));
        printf("\nMessage: ");
        fgets(body, sizeof(body), stdin);
        body[strlen(body)-1] = 0; //sets the last character to NULL
        snprintf(sendtoserv, sizeof(sendtoserv), "%s\n.\n", body);
        write(sockfd, sendtoserv, strlen(sendtoserv));
        sleep(1);
        bzero(message, sizeof(message));
        read(sockfd, message, sizeof(message));
        printf("\nResponse: %s\n", message);
        sleep(1);
    }
    return 0;
}