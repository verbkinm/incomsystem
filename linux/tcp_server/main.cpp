#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */

char buffer[BUFSIZ];

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    const int port = { std::stoi(argv[1]) };

    auto sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    std::cout << "Starting echo server on the port " << port << "...\n";

    if (sock == -1)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };

    addr.sin_addr.s_addr = INADDR_ANY;
    int addrlen = sizeof(addr);

    if (bind(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        close(sock);

        return EXIT_FAILURE;
    }

    if (listen(sock, 0) < 0)
    {
        perror("listen");
        return EXIT_FAILURE;
    }

    std::cout << "Running echo server...\n" << std::endl;

    int client_sock = -1;
    if ( (client_sock = accept(sock, (sockaddr *)&addr, (socklen_t*)&addrlen)) < 0)
    {
        perror("accept");
        return EXIT_FAILURE;
    }

    while(true)
    {
        char buff[BUFSIZ];

        auto recv_bytes = recv(client_sock, (void *)buff, BUFSIZ - 1, 0);
        if (recv_bytes <= 0)
            exit(EXIT_FAILURE);

        buff[recv_bytes] = '\0';
        std::cout << "<< " << buff << '\n';
        send(client_sock, buff, recv_bytes, 0);
        std::cout << ">> " << buff << std::endl;
    }

//    char client_address_buf[INET_ADDRSTRLEN];

//    while (true)
//    {
//        // Read content into buffer from an incoming client.
//        recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
//                            reinterpret_cast<sockaddr *>(&client_address),
//                            &client_address_len);

//        if (recv_len > 0)
//        {
//            buffer[recv_len] = '\0';
//            std::cout
//                    << "Client with address "
//                    << inet_ntop(AF_INET, &client_address.sin_addr, client_address_buf, sizeof(client_address_buf) / sizeof(client_address_buf[0]))
//                    << ":" << ntohs(client_address.sin_port)
//                    << "\nClient name ";

//            char buff_client_name[BUFFER_SIZE];
//            if (getnameinfo(reinterpret_cast<sockaddr*>(&client_address), sizeof(client_address), buff_client_name, BUFFER_SIZE, nullptr, 0, NI_NAMEREQD))
//                std::cerr << "could not resolve hostname" << std::endl;
//            else
//                std::cout << buff_client_name << std::endl;


//            std::cout
//                    << " sent datagram "
//                    << "[length = "
//                    << recv_len
//                    << "]:\n'''\n"
//                    << buffer
//                    << "\n'''"
//                    << std::endl;
//            // Send same content back to the client ("echo").
//            sendto(sock, buffer, recv_len, 0, reinterpret_cast<const sockaddr *>(&client_address),
//                   client_address_len);

//            //EXIT
//            if (strcmp(buffer, "exit") == 0)
//            {
//                std::cout << "Exit\n";
//                close(sock);
//                return EXIT_SUCCESS;
//            }
//        }
//        std::cout << std::endl;
//    }

    return EXIT_SUCCESS;
}
