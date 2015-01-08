/*
 * MDNS Plugin to handle addition and removal of MDNS Service Records
 * Copyright (c) 2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 */

/**
* @file iotkit-comm_mdns.c
 * @brief Implementation of iotkit-comm MDNS.
 *
 * Provides features to Advertise a Service, to Discover for the available Services with Filtering Mechanism.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <pthread.h>

#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include <cJSON.h>
#include <errno.h>              // For errno, EINTR
#include <dns_sd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex.h>
#include "util.h"
#include "iotkit-comm_mdns.h"

/* Helper method to return the last error
*/
char *getLastError() {
    return lastError;
}

/* Helper snippet to handle JSON parse errors
*/
#define handleParseError() \
{\
    if (specification) free(specification);\
    specification = NULL;\
    fprintf(stderr,"invalid JSON format for %s file\n", service_desc_file);\
    goto endParseSrvFile;\
}

/** Parses service specification.
* @param[in] service_desc_file file path to the service specification JSON
* @return returns service specification object upon successful parsing and NULL otherwise
*/
ServiceSpec *parseServiceSpec(char *service_desc_file) {
    ServiceSpec *specification = NULL;
    char *out;
    int i = 0;
    cJSON *json = NULL, *jitem, *child;
    bool status = true;

    FILE *fp = fopen(service_desc_file, "rb");
    if (fp == NULL) {
        fprintf(stderr,"Error can't open file %s\n", service_desc_file);
    }
    else {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        if (buffer != NULL) {
            fread(buffer, 1, size, fp);
            // parse the file
            json = cJSON_Parse(buffer);
        }
        if (json == NULL || !json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
        }
        else {
            #if DEBUG
                out = cJSON_Print(json, 2);
                printf("%s\n", out);
                free(out);
            #endif

            if (!isJsonObject(json))
                handleParseError();

            specification = (ServiceSpec *)malloc(sizeof(ServiceSpec));
            if (specification == NULL) {
                fprintf(stderr, "Can't alloc memory for service specification\n");
                goto endParseSrvFile;
            }

            specification->service_name = NULL;
            specification->type.name = NULL;
            specification->type.protocol = NULL;
            specification->address = NULL;
            specification->port = 0;
            specification->commParamsCount = 0;
            specification->comm_params = NULL;
            specification->numProperties = 0;
            specification->properties = NULL;
            specification->advertise.locally = NULL;
            specification->advertise.cloud = NULL;

            // initially set status to UNKNOWN
            specification->status = UNKNOWN;

            jitem = cJSON_GetObjectItem(json, "name");
            if (!isJsonString(jitem))
                handleParseError();

            specification->service_name = strdup(jitem->valuestring);
            #if DEBUG
                printf("service name %s\n", specification->service_name);
            #endif

            child = cJSON_GetObjectItem(json, "type");
            if (!isJsonObject(child)) {
                free(specification->service_name);
                handleParseError();
            }

            jitem = cJSON_GetObjectItem(child, "name");
            if (!isJsonString(jitem)) {
                free(specification->service_name);
                handleParseError();
            }

            specification->type.name = strdup(jitem->valuestring);
            #if DEBUG
                printf("type name %s\n", specification->type.name);
            #endif

            jitem = cJSON_GetObjectItem(child, "protocol");
            if (!isJsonString(jitem)) {
                free(specification->service_name);
                free(specification->type.name);
                handleParseError();
            }

            specification->type.protocol = strdup(jitem->valuestring);
            #if DEBUG
                printf("protocol %s\n", specification->type.protocol);
            #endif

            jitem = cJSON_GetObjectItem(json, "address");
            if (isJsonString(jitem)) {
                specification->address = strdup(jitem->valuestring);
                #if DEBUG
                    printf("host address %s\n", specification->address);
                #endif
            } else {
                specification->address = NULL;
            }
            // must have a port
            jitem = cJSON_GetObjectItem(json, "port");
            if (!jitem || !isJsonNumber(jitem)) {
                free(specification->service_name);
                free(specification->type.name);
                free(specification->type.protocol);
                free(specification->address);
                handleParseError();
            }
            specification->port = jitem->valueint;
            #if DEBUG
                printf("port %d\n", specification->port);
            #endif

            jitem = cJSON_GetObjectItem(json, "properties");
            if (!isJsonObject(jitem)) {
                free(specification->service_name);
                free(specification->type.name);
                free(specification->type.protocol);
                free(specification->address);
                handleParseError();
            }

            specification->numProperties = 0;
            child = jitem->child;
            while (child && specification->numProperties++ < MAX_PROPERTIES) {
                child = child->next;
            }
            if (specification->numProperties) {
                specification->properties = (Property **)malloc(sizeof(Property *) * specification->numProperties);
                if (specification->properties != NULL) {
                    i = 0;
                    child = jitem->child;
                    while (child && i < MAX_PROPERTIES) {
                        specification->properties[i] = (Property *)malloc(sizeof(Property));
                        if (specification->properties[i] != NULL) {
                            specification->properties[i]->key = strdup(child->string);
                            specification->properties[i]->value = strdup(child->valuestring);
                            #if DEBUG
                                printf("properties key=%s value=%s\n", specification->properties[i]->key,
                                specification->properties[i]->value);
                            #endif
                        }
                        i++;
                        child = child->next;
                    }
                }
            }

            jitem = cJSON_GetObjectItem(json, "comm_params");
            if (isJsonObject(jitem)) {

                specification->commParamsCount = 0;
                child = jitem->child;
                while (child && specification->commParamsCount++ < MAX_PROPERTIES) {
                    child=child->next;
                }
                if (specification->commParamsCount) {
                    specification->comm_params = (Property **)malloc(sizeof(Property *) * specification->commParamsCount);
                    if (specification->comm_params != NULL) {
                        i=0;
                        child = jitem->child;
                        while (child && i < MAX_PROPERTIES) {
                            specification->comm_params[i] = (Property *)malloc(sizeof(Property));
                            if (specification->comm_params[i] != NULL) {
                                specification->comm_params[i]->key = strdup(child->string);
                                #if DEBUG
                                    printf("Value type is : %d\n", child->type);
                                    printf("String value is: %s\n", child->valuestring);
                                    printf("Int value is: %d\n", child->valueint);
                                    printf("Double value is: %f\n", child->valuedouble);
                                #endif
                                if(child->type == cJSON_False) {
                                    specification->comm_params[i]->value = "false";
                                } else if(child->type == cJSON_True) {
                                    specification->comm_params[i]->value = "true";
                                } else {
                                    specification->comm_params[i]->value = strdup(child->valuestring);
                                }
                                #if DEBUG
                                    printf("Comm Param key=%s value=%s\n", specification->comm_params[i]->key,
                                    specification->comm_params[i]->value);
                                #endif
                            }
                            i++;
                            child=child->next;
                        }
                    }
                }
            }

            child = cJSON_GetObjectItem(json, "advertise"); // this is an optional parameter; so, ignore if absent
            if (isJsonObject(child)) {
                jitem = cJSON_GetObjectItem(child, "locally"); // this is an optional parameter; so, ignore if absent
                if (isJsonString(jitem)) {
                    specification->advertise.locally = strdup(jitem->valuestring);
                }

                jitem = cJSON_GetObjectItem(child, "cloud"); // this is an optional parameter; so, ignore if absent
                if (isJsonString(jitem)) {
                    specification->advertise.cloud = strdup(jitem->valuestring);
                }
                 #if DEBUG
                     printf("advertise locally=%s cloud=%s\n", specification->advertise.locally,
                     specification->advertise.cloud);
                 #endif
            }



endParseSrvFile:
            cJSON_Delete(json);
        }

        // free buffers
        fclose(fp);
        free(buffer);
    }

    return specification;
}

/** Parses client query specification.
* @param[in] service_desc_file file path to the query specification JSON
* @return returns client query specification object upon successful parsing and NULL otherwise
*/
ServiceQuery *parseServiceQuery(char *service_desc_file) {
    ServiceQuery *specification = NULL;
    char *out;
    int i = 0;
    cJSON *json = NULL, *jitem, *child;
    bool status = true;

    FILE *fp = fopen(service_desc_file, "rb");
    if (fp == NULL) {
        fprintf(stderr,"Error can't open file %s\n", service_desc_file);
    }
    else {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        if (buffer != NULL) {
            fread(buffer, 1, size, fp);
            // parse the file
            json = cJSON_Parse(buffer);
        }
        if (json == NULL || !json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
        }
        else {
            #if DEBUG
                out = cJSON_Print(json, 2);
                printf("%s\n", out);
                free(out);
            #endif

            if (!isJsonObject(json))
                handleParseError();

            specification = (ServiceQuery *)malloc(sizeof(ServiceQuery));
            if (specification == NULL) {
                fprintf(stderr, "Can't alloc memory for service specification\n");
                goto endParseSrvFile;
            }

            specification->service_name = NULL;
            specification->type.name = NULL;
            specification->type.protocol = NULL;
            specification->address = NULL;
            specification->port = 0;
            specification->commParamsCount = 0;
            specification->comm_params = NULL;
            specification->numProperties = 0;
            specification->properties = NULL;
            specification->advertise.locally = NULL;
            specification->advertise.cloud = NULL;

            // initially set status to UNKNOWN
            specification->status = UNKNOWN;

            jitem = cJSON_GetObjectItem(json, "name");
            if (!isJsonString(jitem))
                handleParseError();

            specification->service_name = strdup(jitem->valuestring);
            #if DEBUG
                printf("service name %s\n", specification->service_name);
            #endif

            child = cJSON_GetObjectItem(json, "type");
            if (!isJsonObject(child)) {
                free(specification->service_name);
                handleParseError();
            }

            jitem = cJSON_GetObjectItem(child, "name");
            if (!isJsonString(jitem)) {
                free(specification->service_name);
                handleParseError();
            }

            specification->type.name = strdup(jitem->valuestring);
            #if DEBUG
                printf("type name %s\n", specification->type.name);
            #endif

            jitem = cJSON_GetObjectItem(child, "protocol");
            if (!isJsonString(jitem)) {
                free(specification->service_name);
                free(specification->type.name);
                handleParseError();
            }

            specification->type.protocol = strdup(jitem->valuestring);
            #if DEBUG
                printf("protocol %s\n", specification->type.protocol);
            #endif

endParseSrvFile:
            cJSON_Delete(json);
        }

        // free buffers
        fclose(fp);
        free(buffer);
    }

    return specification;
}

/** Retrives IP Address for a given host name.
* @param[in] hostname denotes the host name
* @param[in] port denotes the port information
* @return returns IP Address
*/
char *getIPAddressFromHostName(char *hostname,char *PortAsNumber) {

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0; /* any type socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    int s = getaddrinfo(hostname, PortAsNumber, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        fprintf(stderr, "Name resolution failed for %s. Exiting the application.\n", hostname);
        exit(EXIT_FAILURE);
   }

    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *)result->ai_addr;

    // duplicate the IP address so that subsequent calls to 'inet_ntoa' does not overwrite the value
    char *ipaddress = strdup(inet_ntoa((struct in_addr)addr->sin_addr));
    #if DEBUG
        printf("IP Address = %s\n",ipaddress);
    #endif
    freeaddrinfo(result); // frees the memory that was dynamically allocated
    return ipaddress;
}

/** Handle events from DNS Server
* @param[in] client service ref object
* @param[in] callback callback function to be invoked upon any DNS errors
*/
void handleEvents(DNSServiceRef client, void (*callback)(void *, int32_t, void *)) {
    #if DEBUG
        printf("\n in handleevents now\n");
    #endif
    int dns_sd_fd  = client  ? DNSServiceRefSockFD(client) : -1;
    int nfds = dns_sd_fd + 1;
    fd_set readfds;
    struct timeval tv;
    int result;

    // 1. Set up the fd_set as usual here.
    FD_ZERO(&readfds);

    // 2. Add the fd for our client(s) to the fd_set
    if (client)
    FD_SET(dns_sd_fd , &readfds);

    // 3. Set up the timeout.
    tv.tv_sec  = timeOut;
    tv.tv_usec = 0;

    result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
    #if DEBUG
        printf("select result = %d\n", result);
    #endif
    if (result > 0) {
        DNSServiceErrorType err = kDNSServiceErr_NoError;
        if (client  && FD_ISSET(dns_sd_fd , &readfds)) {
            err = DNSServiceProcessResult(client );
        }
        if (err) {
            sprintf(lastError, "Failed waiting on DNS file descriptor");
            callback(client, err, NULL);
            stopBlockingState = 1;
        }
    }
    else if (result == 0) {
        DNSServiceErrorType err = DNSServiceProcessResult(client);
        if (err != kDNSServiceErr_NoError) {
            sprintf(lastError, "DNSService call failed");
            callback(client, err, NULL);
            stopBlockingState = 1;
        }
    }
    else {
        sprintf(lastError, "select() returned %d errno %s", result, strerror(errno));
        callback(client, errno, NULL);
        if (errno != EINTR)
            stopBlockingState = 1;
    }
}

/** Callback invoked upon resolving a service from DNS.
* @param[in] client service reference object
* @param[in] flags DNS serivce flags
* @param[in] ifIndex interface index
* @param[in] errorCode error code
* @param[in] fullservicename complete service name
* @param[in] hosttarget host on which the service is available
* @param[in] opaqueport port information
* @param[in] txtLen lenght of TXT Record
* @param[in] txtRecord TXT Record
* @param[in] context context information
*/
static void DNSSD_API discover_resolve_reply(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                    const char *fullservicename, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context) {
    union {
        uint16_t s;
        u_char b[2];
    }port = { opaqueport };

    uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

    #if DEBUG
        printf("%s can be reached at %s:%u (interface %d)", fullservicename, hosttarget, PortAsNumber, ifIndex);
    #endif
    if (errorCode)
        fprintf(stderr,"Error code %d\n", errorCode);

    DiscoverContext *discContext = (DiscoverContext *)context;
    ServiceQuery *query = discContext->serviceSpec;

    // perform service filter
    if(serviceQueryFilter(query, fullservicename, PortAsNumber, txtLen, txtRecord) == false)
        return;

    char *filteredServiceAddress = serviceAddressFilter(query, hosttarget, fullservicename, PortAsNumber);

    if(!filteredServiceAddress)
        return;

    // there is a user filterCB, so call it. If it returns false then donothing
    if (discContext->userFilterCB && discContext->userFilterCB(query) == false) {
        free(filteredServiceAddress);
        return;
    }

    /*// check whether user has configured any host address
    if (query->address == NULL)
        query->address = hosttarget;*/

    query->address = filteredServiceAddress;
    query->port = PortAsNumber;
    #if DEBUG
        printf("\nquery->port: %d\n",query->port);
    #endif
    discContext->callback(client, errorCode, createClient(query));
}

/** Callback invoked upon discovering a query reply from DNS.
* @param[in] client service reference object
* @param[in] flags DNS serivce flags
* @param[in] interfaceIndex interface index
* @param[in] errorCode error code
* @param[in] name service name
* @param[in] regtype registered type
* @param[in] domain domain information
* @param[in] context context information
*/
static void DNSSD_API queryReply(DNSServiceRef client, 
                DNSServiceFlags flags,
                uint32_t interfaceIndex,
                DNSServiceErrorType errorCode,
                const char *name,
                const char *regtype,
                const char *domain,
                void *context) {
    DiscoverContext *discContext = (DiscoverContext *)context;
    ServiceSpec *desc = (ServiceSpec *) discContext->serviceSpec;
    DNSServiceRef newclient = NULL;
    DNSServiceErrorType err;

#if DEBUG
    printf("Got a reply for %s.%s.%s\n", name, regtype, domain);
#endif
    if (errorCode == kDNSServiceErr_NoError) {
        if (flags & kDNSServiceFlagsAdd)
            desc->status = ADDED;
        else
            desc->status = REMOVED;

        #if DEBUG
            printf("desc status %d\n", desc->status);
        #endif

        err = DNSServiceResolve(&newclient, 0, interfaceIndex, name, regtype, domain, discover_resolve_reply, context);
        if (!newclient || err != kDNSServiceErr_NoError) {
            sprintf(lastError, "queryReply DNSServiceResolve call failed %ld\n", (long int)err);
            discContext->callback(client, err, NULL);
            if (newclient) {
                DNSServiceRefDeallocate(newclient);
            }
        }
        else {
            handleEvents(newclient,discContext->callback);
        }
    }
    else {
        sprintf(lastError, "MDNS unexpected error");
        discContext->callback(newclient, errorCode, NULL);
    }
}

/** Creates a client for the given ServiceSpec object
* @param[in] queryDesc service specification
* @param[in] callback callback to be invoked upon successful client creation
*/
void createClientForGivenService(ServiceQuery *queryDesc, void (*callback)(void *, int32_t, void *)) {
    if(queryDesc->address == NULL) {
        queryDesc->address = "127.0.0.1"; // defaults to localhost
    }

    if(queryDesc->port == 0) {
        fprintf(stderr, "Error: Unknown Port information\n");
        return;
    }

    callback(NULL, 0, createClient(queryDesc));
}

/** Browse or Discover a service from MDNS. This is a blocking call.
* @param[in] queryDesc service specification
* @param[in] userFilterCB callback method for user filter
* @param[in] callback callback to be invoked upon successful client creation
*/
void discoverServicesBlockingFiltered(ServiceQuery *queryDesc,
        bool (*userFilterCB)(ServiceQuery *),
        void (*callback)(void *, int32_t, void *)) {
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;      // thread to handle events from DNS server
    char regtype[128];
    DiscoverContext *context = (DiscoverContext *)malloc(sizeof(DiscoverContext));

    setMyAddresses(); // initialize my IP Addresses

    if (!context) return;
    context->userFilterCB = userFilterCB;
    context->callback = callback;
    context->serviceSpec = queryDesc;

    // register type
    strcpy(regtype, "_");
    strcat(regtype, queryDesc->type.name);
    strcat(regtype, "._");
    strcat(regtype, queryDesc->type.protocol); 

    err = DNSServiceBrowse
        (&client,
        0,
        opinterface,
        regtype,    // registration type
        "",     // domain (null = pick sensible default = local)
        queryReply, // callback
        context);   // param to pass as context into queryReply

    if (!client || err != kDNSServiceErr_NoError) {
        sprintf(lastError, "DNSServiceBrowse call failed %ld\n", (long int)err);
        callback(client, err, NULL);
        if (client)
            DNSServiceRefDeallocate(client);
    }
    else {
        stopBlockingState = 0;
        while (!stopBlockingState) {
            handleEvents(client, callback);
        }
    }
}


/** Match the service name against user supplied service query by resolving regular expression (if any).
* @param[in] srvQry service query
* @param[in] fullservicename complete service name
*/
bool getServiceNameMatched(ServiceQuery *srvQry, char *fullservicename) {
    regex_t regex;
    int res;
    char msgbuf[100];
    #if DEBUG
        printf("\nFull Service name %s\n",fullservicename);
    #endif
    // searching for character '.' where the service name ends
    char *end = strchr(fullservicename,'.');
    if (end == NULL) {
        printf ("searched character NOT FOUND\n");
        return false;
    }

    char servicename[256];
    int i = 0;

    while (fullservicename != end) {
        servicename[i++] = *fullservicename++;
    }
    servicename[i] = '\0';

    #if DEBUG
        printf("\nReal Service name %s\n",servicename);
    #endif

    /* Compile regular expression */
    res = regcomp(&regex, srvQry->service_name, REG_EXTENDED);
    if(res ) {
        fprintf(stderr, "Could not compile regex\n");
    }

    /* Execute regular expression */
    res = regexec(&regex, servicename, 0, NULL, 0);
    if( !res ) {
        return true;
    }
    else if(res == REG_NOMATCH ) {
        return false;
    }
    else {
        regerror(res, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    }

    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);

    return false;
}


/** Address filtering for service available locally.
* @param[in] srvQry service query
* @param[in] hosttarget target host name
* @param[in] fullname full name of the service
* @param[in] portAsNumber port information
*/
char* serviceAddressFilter(ServiceQuery *srvQry, const char *hosttarget, const char *fullname, uint16_t portAsNumber) {

    if (!hosttarget || !fullname) {
        if (!fullname) {
            printf("WARN: Discovered a service without a name. Dropping.\n");
        } else {
            printf("WARN: Discovered a service without addresses. Dropping.\n");
        }
        return NULL;
    }

    char *serviceName;
    char portAsChar[256];
    sprintf(portAsChar, "%d", portAsNumber);

    if(strstr(fullname, "._")) {
        //copy only the service name
        serviceName = strndup(fullname, strstr(fullname, "._") - fullname);
    } else {
        // looks like we actually got the service name instead of fullname
        serviceName = strdup(fullname);
    }

    char *address = getIPAddressFromHostName(hosttarget, portAsChar);

    ServiceCache *traverse = serviceCache;
    bool isPresentInCache = false;
    bool isServiceSeenBefore = false;
    if (serviceName != NULL && address != NULL) {
        while(traverse != NULL) {
            if(strcmp(traverse->servicename, serviceName) == 0) {

                if(strcmp(traverse->address, address) == 0) {
                    isPresentInCache = true;
                }else {
                    // service is already known; but got callback on new address/interface
                    isServiceSeenBefore = true;
                }
            }
            traverse = traverse->next;
        }

        if(isPresentInCache == true) {
            #if DEBUG
                puts("Info: service info already present in cache");
            #endif
            free(serviceName);
            free(address);
            return NULL;
        }

        ServiceCache *newService = (ServiceCache *)malloc(sizeof(ServiceCache));
        if (newService != NULL) {
            newService->next = NULL;
            newService->servicename = serviceName;
            newService->address = address;

            if(serviceCache == NULL) {
                serviceCache = newService;
            } else {
                traverse = serviceCache;
                while(traverse->next != NULL) {
                    traverse = traverse->next;
                }

                traverse->next = newService;
            }
        }
        free(serviceName);
        serviceName = NULL;
        free(newService);
        if(isServiceSeenBefore) {
            #if DEBUG
                puts("Info: service already seen before");
            #endif
            free(address);
            return NULL;
        }

        if(isServiceLocal(address)) {
            #if DEBUG
                puts("Info: This is a local service; so returning local address");
            #endif
            free(address);
            return LOCAL_ADDRESS;
        }
        #if DEBUG
            printf("This is a non-local service; so returning IP address of non-local host:%s\n", address);
        #endif
    }
    if (serviceName != NULL) {
        free(serviceName);
    }
    // return the IP address of non-local host
    return address;
}

/** Verifies whether the service is available locally or not.
* @param[in] address host address information
* @return returns true if local address and false otherwise
*/
bool isServiceLocal(const char *address) {
    int i;
    for(i = 0; i < myaddressesCount; i++) {
        if(strcmp(myaddresses[i], address) == 0) {
            return true;
        }
    }

    return false;
}

/** Service Query Filter.
* @param[in] srvQry service query
* @param[in] fullservicename complete service name
* @param[in] PortAsNumber port information
* @param[in] txtLen length of TXT Record
* @param[in] txtRecord TXT Record
*/
bool serviceQueryFilter(ServiceQuery *srvQry, char *fullservicename, uint16_t PortAsNumber, uint16_t txtLen, const unsigned char *txtRecord) {

    bool isNameMatched = false;
    bool isPortMatched = false;
    bool isPropertiesMatched = false;

    Property **properties;

    // check whether service name in ServiceQuery matches with service name reported by discover_resolve_reply
    if(getServiceNameMatched(srvQry,fullservicename)) { // check whether
    #if DEBUG
        printf("Yes %s:matches with:%s\n", fullservicename, srvQry->service_name);
    #endif
        isNameMatched = true;
    }

   if(srvQry->port) { // if port details present in discovery query
        if(PortAsNumber == srvQry->port) {
        #if DEBUG
            printf("Yes port:%u:matched with:%u\n", PortAsNumber, srvQry->port);
        #endif
            isPortMatched = true;
        }
    } else {
        // port not defined by the service query; so consider as matched successfully
        isPortMatched = true;
    }

    uint16_t propertiesCountInTxtRecord = TXTRecordGetCount(txtLen, txtRecord);
    int bufferKeySize = 256;
    char bufferKey[bufferKeySize];
    void *bufferValue;
    uint8_t bufferValueSize;
    int i, j;

    properties = (Property **)malloc(sizeof(Property *) * propertiesCountInTxtRecord);
    if (properties != NULL) {
        for(i = 0; i < propertiesCountInTxtRecord; i++) {
            properties[i] = (Property *)malloc(sizeof(Property));
            if (properties[i] != NULL) {
                TXTRecordGetItemAtIndex(txtLen, txtRecord, i, bufferKeySize -1, bufferKey, &bufferValueSize, &bufferValue);

                properties[i]->key = strdup(bufferKey);
                properties[i]->value = strndup(bufferValue, bufferValueSize);

                #if DEBUG
                    printf("READ Property:%s:%s; from TXT Record\n", properties[i]->key, properties[i]->value);
                #endif
            }
        }

        if(srvQry->numProperties > 0 && srvQry->properties != NULL) {
            // look for atleast one property match
            for(i = 0; i < srvQry->numProperties; i++) {
                for(j = 0; j < propertiesCountInTxtRecord; j++) {
                    if(strcmp(srvQry->properties[i]->key, properties[j]->key) == 0 && \
                        strcmp(srvQry->properties[i]->value, properties[j]->value) == 0) {
                            isPropertiesMatched = true; // yes found atleast one matching property
                            break;
                        }
                }
            }
        } else {
            // there are no properties defined by the service query; so consider as matched successfully
            isPropertiesMatched = true;
        }

        // free the memory
        for(i = 0; i < propertiesCountInTxtRecord; i++) {
            free(properties[i]);
        }
        free(properties);
    }
    if(isNameMatched && isPortMatched && isPropertiesMatched) {
    #if DEBUG
        printf("Returning TRUE --- Match found for service:%s\n", fullservicename);
    #endif
        return true;
    }

#if DEBUG
    printf("Returning FALSE --- No Match found for query %s with service name :%s\n",srvQry->service_name,fullservicename);
#endif
    return false;
}

/** Verify whether a property is present in communication parameters or not.
* @param[in] srvQry service Query
* @param[in] paramName name of the property to be verified
* @return returns true if present and false otherwise
*/
bool isPresentPropertyInCommParams(ServiceQuery *srvQry, char *paramName){
    int i;
    if(srvQry->commParamsCount > 0 && srvQry->comm_params != NULL){
        for(i = 0; i < srvQry->commParamsCount; i ++){
            if(strcmp(srvQry->comm_params[i]->key, paramName) == 0) {
                return true;
            }
        }
    }

    return false;
}

/** Return the value of a property if present in communication parameters list.
* @param[in] srvQry service Query
* @param[in] paramName name of the property to be verified
* @return returns value if property is present and NULL otherwise
*/
char* getValueInCommParams(ServiceQuery *srvQry, char *paramName){
    int i;
    if(srvQry->commParamsCount > 0 && srvQry->comm_params != NULL){
        for(i = 0; i < srvQry->commParamsCount; i ++){
            if(strcmp(srvQry->comm_params[i]->key, paramName) == 0) {
                return srvQry->comm_params[i]->value;
            }
        }
    }

    return NULL;
}

/** Browse or Discover a service from MDNS. This is a blocking call.
 * @param[in] queryDesc service specification
 * @param[in] callback callback to be invoked upon successful client creation
 */
void discoverServicesBlocking(ServiceQuery *queryDesc,
    void (*callback)(void *, int32_t, void *)) {
    discoverServicesBlockingFiltered(queryDesc, NULL, callback);
}

/** Callback invoked upon resolving an advertised service through DNS.
* @param[in] client service reference object
* @param[in] flags DNS serivce flags
* @param[in] ifIndex interface index
* @param[in] errorCode error code
* @param[in] fullname complete service name
* @param[in] hosttarget host on which the service is available
* @param[in] opaqueport port information
* @param[in] txtLen lenght of TXT Record
* @param[in] txtRecord TXT Record
* @param[in] context context information
*/
static void DNSSD_API advertise_resolve_reply(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                    const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context) {
    union {
        uint16_t s;
        u_char b[2];
    }port = { opaqueport };

    uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

    #if DEBUG
        printf("%s can be reached at %s:%u (interface %d)\n", fullname, hosttarget, PortAsNumber, ifIndex);
    #endif

    if (errorCode)
        printf(stderr,"advertise_resolve_reply Error code %d\n", errorCode);

    DiscoverContext *discContext = (DiscoverContext *)context;
    ServiceSpec *specification = discContext->serviceSpec;
    // check whether user has explicitly specified the host address
    if (specification->address != NULL) {
        char portarr[128];
        sprintf(portarr, "%d",specification->port);
        char *ipaddress = getIPAddressFromHostName(specification->address,portarr);
        if (ipaddress != NULL) {
            specification->address = ipaddress;
            discContext->callback(client, errorCode,createService(specification));
        } else {
            printf("\nIn advertise_resolve_reply Host Name to IP Address Conversion Failed\n");
        }
    } else {
        discContext->callback(client, errorCode,createService(specification));
    }

}

/** Callback invoked upon registering a service via DNS.
* @param[in] client service reference object
* @param[in] flags DNS serivce flags
* @param[in] errorCode error code
* @param[in] name service name
* @param[in] regtype registered type
* @param[in] domain domain information
* @param[in] context context information
*/
static void DNSSD_API regReply(DNSServiceRef client,
                const DNSServiceFlags flags,
                DNSServiceErrorType errorCode,
                const char *name,
                const char *regtype,
                const char *domain,
                void *context) {
    (void)flags;    // Unused

    DiscoverContext *discContext = (DiscoverContext *)context;
    DNSServiceErrorType err;
    ServiceSpec desc;
    desc.service_name = (char *)name;

#if DEBUG
    printf("Got a reply for %s.%s.%s\n", name, regtype, domain);
#endif
    if (errorCode == kDNSServiceErr_NoError) {
        desc.status = REGISTERED;
        err = DNSServiceResolve(&client, 0, 0, name, regtype, domain, advertise_resolve_reply, context);
        if (!client || err != kDNSServiceErr_NoError) {
            sprintf(lastError, "regReply DNSServiceResolve call failed %ld\n", (long int)err);
            discContext->callback(client, err, NULL);
            if (client) {
                DNSServiceRefDeallocate(client);
            }
        }
        else {
            handleEvents(client,discContext->callback);
        }
    }
    else if (errorCode == kDNSServiceErr_NameConflict) {
        sprintf(lastError, "Name in use, please choose another %s.%s.%s", name, regtype, domain);
        desc.status = IN_USE;
        discContext->callback(client, errorCode, NULL);
    }
    else {
        sprintf(lastError, "MDNS unexpected error");
        discContext->callback(client, errorCode, NULL);
    }
}

/** Advertise a service. Return an opaque object.
* @param[in] specification service specification
* @param[in] callback callback to be invoked upon successful service creation
*/
bool advertiseService(ServiceSpec *specification) {
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid; // thread to handle events from DNS server
    char regtype[128];
    TXTRecordRef txtRecord;

    setMyAddresses(); // initialize my IP Addresses

    // register type
    strcpy(regtype, "_");
    strcat(regtype, specification->type.name);
    strcat(regtype, "._");
    strcat(regtype, specification->type.protocol);

    if (specification->numProperties) {
        uint8_t txtLen, i = 0;
        TXTRecordCreate(&txtRecord, 0, NULL);
        for (i = 0; i<specification->numProperties; i++) {
            txtLen = (uint8_t)strlen(specification->properties[i]->value);
            TXTRecordSetValue(&txtRecord, specification->properties[i]->key,
                    txtLen, specification->properties[i]->value );
        }
    }

    err = DNSServiceRegister
        (&client,
        0,
        opinterface,
        specification->service_name,  // service name
        regtype,    // registration type
        "",     // default = local
        NULL,   // only needed when creating proxy registrations
        htons(specification->port),   // Must have a port
        TXTRecordGetLength(&txtRecord),
        TXTRecordGetBytesPtr(&txtRecord),
        NULL,   // callback
        NULL);  // param to pass as context into regReply

    if (specification->numProperties) {
        TXTRecordDeallocate(&txtRecord);
    }

    if (!client || err != kDNSServiceErr_NoError) {
        sprintf(lastError, "DNSServiceRegister call failed %ld\n", (long int)err);
        if (client)
            DNSServiceRefDeallocate(client);

        return false;
    }

    return true;
}


/** Advertise a service. Return an opaque object, which is passed along to callback.
 * Note: This is a blocking call.
 * @param[in] specification service specification
 * @param[in] callback callback to be invoked upon successful service creation
 */
void advertiseServiceBlocking(ServiceSpec *specification,
        void (*callback)(void *, int32_t, void *)) {
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;      // thread to handle events from DNS server
    char regtype[128];
    TXTRecordRef txtRecord;

    setMyAddresses(); // initialize my IP Addresses

    DiscoverContext *context = (DiscoverContext *)malloc(sizeof(DiscoverContext));
    if (!context) return;
    context->callback = callback;
    context->serviceSpec = specification;

    // register type
    strcpy(regtype, "_");
    strcat(regtype, specification->type.name);
    strcat(regtype, "._");
    strcat(regtype, specification->type.protocol);

    if (specification->numProperties) {
        uint8_t txtLen, i = 0;
        TXTRecordCreate(&txtRecord, 0, NULL);
        for (i = 0; i<specification->numProperties; i++) {
            txtLen = (uint8_t)strlen(specification->properties[i]->value);
            TXTRecordSetValue(&txtRecord, specification->properties[i]->key,
                    txtLen, specification->properties[i]->value );
        }
    }

    err = DNSServiceRegister
        (&client,
        0,
        opinterface,
        specification->service_name,  // service name
        regtype,    // registration type
        "",     // default = local
        NULL,   // only needed when creating proxy registrations
        htons(specification->port),   // Must have a port
        TXTRecordGetLength(&txtRecord),
        TXTRecordGetBytesPtr(&txtRecord),
        regReply,   // callback
        context);   // param to pass as context into regReply

    if (specification->numProperties) {
        TXTRecordDeallocate(&txtRecord);
    }

    if (!client || err != kDNSServiceErr_NoError) {
        sprintf(lastError, "DNSServiceRegister call failed %ld\n", (long int)err);
        callback(client, err, NULL);
        if (client)
            DNSServiceRefDeallocate(client);
    }
    else {
        stopBlockingState = 0;
        while (!stopBlockingState) {
            handleEvents(client, callback);
        }
    }
}

/** Initialize local addresses on various network interfaces
*/
bool setMyAddresses(void) {

    int iSocket;
    struct if_nameindex *if_ni, *i;
    int j;

    if ((iSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return false;
    }

    if_ni = if_nameindex();
    if (if_ni != NULL) {
        // verify how many addresses do we have for the network interfaces
        for (i = if_ni, myaddressesCount; ! (i->if_index == 0 && i->if_name == NULL); i++) {
            struct ifreq req;
            strncpy(req.ifr_name, i->if_name, IFNAMSIZ);
            if (ioctl(iSocket, SIOCGIFADDR, &req) < 0) {
                if (errno == EADDRNOTAVAIL) {
                    continue;
                }
                perror("ioctl");
                close(iSocket);
                if_freenameindex(if_ni);
                return false;
            }
            myaddressesCount++;
        }

        myaddresses = (char **)malloc(sizeof(char *) * myaddressesCount);
        if (myaddresses != NULL) {
            for (i = if_ni, j = 0; ! (i->if_index == 0 && i->if_name == NULL); i++) {
                struct ifreq req;
                strncpy(req.ifr_name, i->if_name, IFNAMSIZ);
                if (ioctl(iSocket, SIOCGIFADDR, &req) < 0) {
                    if (errno == EADDRNOTAVAIL) {
                        myaddresses[j] = NULL;
                        continue;
                    }
                    perror("ioctl");
                    close(iSocket);
                    if_freenameindex(if_ni);
                    return false;
                }

                myaddresses[j++] = strdup(inet_ntoa(((struct sockaddr_in*)&req.ifr_addr)->sin_addr));
                #if DEBUG
                    printf("Got IP Address:%s\n", myaddresses[j-1]);
                #endif
            }
        }
        if_freenameindex(if_ni);
    }
    close(iSocket);

    return true;
}

#if DEBUG
void callback(void *handle, int32_t error_code, ServiceSpec *desc) {
    printf("message error=%d error_string=%s\nservice status=%d service name=%s\n", 
        error_code,
        getLastError(),
        desc ? desc->status : -1,
        desc ? desc->service_name : "");
}

// Test code for advertisement
void testAdvertise() {
    ServiceSpec *specification = parseServiceSpec("../../examples/serviceSpecs/temperatureServiceMQTT.json");
    if (specification)
        advertiseServiceBlocking(specification, callback);

    printf ("Done advertise\n");
}

// Test code for discover
void testDiscover() {
    ServiceQuery *query = parseServiceQuery("../../examples/serviceQueries/temperatureServiceQueryMQTT.json");
    if (query)
        discoverServicesBlocking(query, callback);
    printf("Done discover\n");
}

int main(void) {
    //testAdvertise();
    testDiscover();
}

#endif
