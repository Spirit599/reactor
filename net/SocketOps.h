#pragma once

#include <arpa/inet.h>

int createNonblockingOrDie(sa_family_t fa);

// int connectOrDie(int fd, const struct sockaddr* addr);

void bindOrDie(int fd, const struct sockaddr* addr);

void listenOrDie(int fd);

int acceptAndDeal(int fd, struct sockaddr_in* addr);

void createAddrFromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);

void toIp(char* buf, size_t size, const struct sockaddr_in* addr);

void toIpPort(char* buf, size_t size, const struct sockaddr_in* addr);